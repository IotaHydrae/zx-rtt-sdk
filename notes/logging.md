# 从板子取日志

> UART 控制台只有 **115200 baud ≈ 11.5 KB/s**，既读不完一次启动的日志，又会**改变被测行为**
> （实测：每帧一行 `rt_kprintf` ≈ 5.2 ms，把 USB 通道从 20.34 打到 1.37 MB/s）。
> 现在日志进**环**，主机用 USB 读：**毫秒级**、可读**启动以来**的全部、且能**执行 shell 命令**。

## TL;DR

```bash
zxlogctl.py read                     # 读环（启动以来 + 运行时），毫秒级 ✓
zxlogctl.py run "list_thread" --follow   # 从 USB 执行命令并读回输出 ✓（全程不碰串口）
zxlogctl.py stats                    # 环内部计数，排查用 ✓
```

三块实现：

| 位置 | 作用 |
| --- | --- |
| `zx_logring.c` | 静态 256 KB 环 + `ulog` 后端 + **控制台设备** + `logcat` 命令 |
| `zx_usb_flash.c` | interface 1 上的 **EP3 IN `0x83`** 流式送环；`LOG_READ`/`LOGSTAT`/`RUN_CMD` |
| `zxlogctl.py` | 主机工具 |

## 为什么不能靠 UART

| 事实 | 数值 |
| --- | --- |
| 控制台速率 | 115200 baud ≈ **11.5 KB/s** ✓ |
| 256 KB 环 dump 时间 | ≈ **22 秒** ✓（实测被截断 ✓） |
| 每帧打印代价 | **≈ 5.2 ms** ✓（≈ 60 B ÷ 11.5 KB/s ✓，与实测吻合 ✓） |
| USB 读环 | **290–380 KB/s** ✓（快 25–33 倍 ✓） |

**它还会饿死别的输出** ✗：烧写时坏块扫描每块打两行错误 ✓，把 UART 占满 ✓ ⇒ 连 `logcat` 的结果都拿不到 ✓。
**在 UART 上验证日志系统本身自相矛盾** ✓ —— 这是当初转向 USB 的直接原因 ✓。

## 两个必须记住的机制陷阱

### ① 控制台会被 BSP 抢回去 ⇒ 必须在最后再抢一次

`rt_kprintf` 的路由由 `_console_device` 决定 ✓：

```c
if (_console_device == RT_NULL)  rt_hw_console_output(str);              /* 直接写硬件 */
else                             rt_device_write(_console_device, 0, ...); /* 走控制台设备 */
```

在 `INIT_BOARD_EXPORT` 注册控制台设备**不够** ✗ —— BSP 之后会把 **`uart0`** 装上 ✓，**后装的赢** ✓。

**判据**：`rt_console_get_device()` 的名字 ✓。实测过 `console='uart0'` ✗（此时 `writes=5` ✓，机制是通的但路由不在我们这 ✓），补上 `INIT_APP_EXPORT` 的再次抢占后变成 `console='zxconsole'` ✓、`seq 12→56` ✓、`writes 5→49` ✓。

### ② 端点号超范围会让 SET_CONFIGURATION 直接 stall

给日志流先用 **EP5 IN `0x85`** ✗ ⇒ 设备枚举不正常 ✓：

```text
kernel: usb 3-6: can't set config #1, error -32        ← -32 = -EPIPE
libusb_set_configuration → USBError: Other error
```

**已知可用的端点只到 EP4** ✓（协议用 1/2/4 ✓，我们加的 EP3 可用 ✓）。改用 **EP3 IN `0x83`** ✓ 即好 ✓ ——
IN/OUT 在 AIC 端口里是**分离的寄存器组** ✓，所以 EP3 的两个方向互不冲突 ✓。

**诊断要点**：内核那句 `can't set config` 说明**不是 libusb 的问题** ✓，是设备在配置阶段 stall ✓。

## 从 USB 执行命令

```
控制请求 0x83（数据段 = 命令行）→ 回调只拷贝并置标志 → zxcmd 任务执行 msh_exec()
                                      ↓
                          命令输出经控制台设备自动进环
                                      ↓
                          主机用同一个 read 拿结果
```

**为什么必须交给任务** ✗：在 USB 回调里跑 `msh_exec` 会**占住那条通道** ✓（实测：紧接着的 `LOGSTAT` 超时 ✗）。
与"回调里烧 NAND 掉 15 倍"是同一个教训 ✓（PUD 固件注释也写着同一句 ✓）。

**忙时不要 stall** ✗：忙就 `return -1` 会让主机看到"设备死了" ✓ ⇒ 已改为**最新命令覆盖** ✓。

**启动结果要自报** ✓：任务启动的返回码写进环 ✓（`[zxflash] zxcmd started (startup=0)` ✓）。
这一条价值很直接 ✓ —— 我曾据 stall 推断"任务没起来" ✗，正是这行自报把它**推翻**了 ✓。

## 调试方法论：先确认仪器，再相信读数

有一个"写入函数必然执行过、却看不到结果"的**矛盾** ✓，读代码解决不了 ✗。加两个探针后**一次读数**定案 ✓：

- `console_writes` 计数 ✓ ⇒ 证明机制通 ✓
- **控制台设备名** ✓ ⇒ 直接指出"被 `uart0` 顶掉" ✓

⇒ **探针要能回答"谁在接管"** ✓，而不只是"有没有发生" ✓。

## 其它踩过的

- **pyusb 不会替你设配置** ✗：不 `set_configuration()` 就 `Configuration not set` ✓；且**不要用 `except: pass` 吞掉失败** ✗（会让工具在连不上的设备上"成功" ✓）
- **字符串描述符 `bLength` 写大一位** ✗ ⇒ 主机多读一个字符 ✓（产品名显示成 `PUD Display氭` ✓），11 字符应为 `2 + 11*2 = 24 = 0x18` ✓
- **`RT_USING_DEVICE_OPS` 打开的构建里**，`rt_device` 用 **`ops` 表** ✓，没有直接的 `open`/`write` 成员 ✗（adbd 就是栽在这 ✓）

## 一条取证纪律（踩过 ✓）

**日志环在 RAM 里** ✓，设备一重启就没了 ✓ ⇒ **任何可能触发重启的实验，必须一边做一边读** ✓。
我因为"做完再读" ✗，白丢过一整轮证据 ✓。

## 相关

- 本地烧写通道：[usb-flash.md](usb-flash.md)
- USB 通路与端口缺陷：[usb.md](usb.md)
- 启动流程：[boot-flow.md](boot-flow.md)
