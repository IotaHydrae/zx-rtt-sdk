# 构建与烧录

> 这个 SDK 的构建与烧录和普通 RT-Thread 工程有四处不同，四处都会让人白花时间：
> 工具链不在 PATH、配置的源头是 defconfig 而不是 `.config`、烧录工具在仓库里、
> 而板子上有**两个脾气相反的串口**。

## TL;DR

- 构建：`export PATH="$(pwd)/toolchain/bin:$PATH" && scons -j8`
- **改配置要改 `target/configs/*_defconfig`** —— `scons --menuconfig` 会从它重生成 `.config`
- 烧录：板子进升级模式（shell 里 `aicupg`，或 BOOT+RESET）→ `tools/scripts/upgcmd -p image <img>`
- 控制台是 WCH 那个口，**DTR 必须拉高**；CH340 那个口相反且写方向不可靠

## 构建

```bash
export PATH="$(pwd)/toolchain/bin:$PATH"   # 工具链不在系统 PATH 里
scons -j8
```

- 交叉工具链是 **Xuantie-900 GCC 10.2.0**（RISC-V），由 `tools/toolchain/*.tar.gz` 解压到
  仓库根的 `toolchain/`，**已被 gitignore**，不要提交。
- 产物：`output/<板子>/images/*.img`（ArtInChip 固件容器，magic `AIC.FW`）。

### 配置的源头是 defconfig，不是 `.config`

```bash
# 正确的改法
vim target/configs/<板子>_defconfig

# 错误的改法：手改 .config 后跑 --menuconfig，改动会被 defconfig 覆盖掉
scons --menuconfig          # ← 会重新生成 .config
```

实测：手改 `.config` 打开 USB 选项后再跑一次 `--menuconfig`，**改动全部消失**，
而构建照样成功 —— 症状是"我明明开了，但代码没编进去"。

板级配置与芯片级配置分两处：

| 层 | 文件 |
| --- | --- |
| 选板子 | `target/configs/<板子>_defconfig`、`.defconfig` |
| 板级选项（引脚、外设使能） | `target/<chip>/common/Kconfig.board` |
| 芯片级选项（驱动全局开关） | `bsp/zx/sys/<chip>/Kconfig.chip` |

## 烧录

工具在仓库里，是**原生 Linux 可执行文件**（不是 Windows 工具）：

```bash
tools/scripts/upgcmd -l                                    # 列出设备
tools/scripts/upgcmd -p image output/<板子>/images/<img>    # 烧录，-p 打进度
tools/scripts/upgcmd shcmd reset                           # ← BROM 阶段不实现 shcmd
```

**让板子进升级模式**：在它的 RT-Thread shell 上敲 `aicupg`（回显 `Enter upgrade mode by
software`），或者按住 BOOT 复位。

### 两个阶段：BROM 与第二阶段代理

`aicupg` 进的是 **BROM（Boot ROM）** 模式 —— `bsp/common/include/aic_reboot_reason.h` 里
那句注释就是证据：

```c
REBOOT_REASON_UPGRADE = 4, /* Goto BROM upgrade mode */
```

| 能力 | BROM 阶段 | 第二阶段（引导程序代理） |
| --- | --- | --- |
| `image`（烧写） | ✅ | ✅ |
| 内存读写（`write`/`read`/`exec`） | ✅ | ✅ |
| **读 flash**（`lspart`/`lsmedia`/`dump`） | ❌ | ✅ |

读 flash 用的协议命令（`UPG_PROTO_CMD_GET_STORAGE_MEDIA` 0x16、`GET_PARTITION_TABLE` 0x17、
`READ_FWC_DATA` 0x18）实现在 `application/baremetal/bootloader/cmd/aicupg.c` —— 那是**第二阶段**的
活。所以在 BROM 阶段 **`upgcmd dump` 做不了，备份 flash 也就做不了**。

> **未验证**：`upgcmd dump` 是否自己会驱动 BROM 去加载第二阶段代理，从而让备份可行。
> 当时没有在 BROM 模式下试 `dump`（只试了 `lspart` 并失败），BROM 阶段到底该怎么读 flash 未定论。

### 厂商烧写是**临界**的：失败就重试，不要找理由

实测同一命令**第一次失败、第二次成功**，之后连续成功 ✓。失败形态固定：

```text
[ERROR] aicupg_cmd_set_upg_end(): Send command failed
[ERROR] image_do_upgrade_inner(): Burn ... failed!, ret -4
```

**5926912 字节的原版镜像同样失败** ✓ ⇒ 与我们的改动无关 ✓。
**动作是重试** ✓（两次通常就够）—— 不要去找供电、Hub、协议的解释 ✗。

### 读设备日志时**不要过滤**

排查 OTA/烧写时，真正的原因往往由**设备自己打印** ✓。我因为只 `grep` 了自己关心的关键字 ✗，
把 `E/NO_TAG: Queue overflow...` 过滤掉了 ✗，白绕一轮 ✓。**先抓全量，再筛** ✓。

### 烧完之后怎么让它启动（可全自动，不必手按 RESET）

`upgcmd image` 之后板子停在 **BROM 的 USB 循环**里，不会自己启动。
整条链路可以全自动，**每次两步**：

```bash
upgcmd continue            # BROM → U-Boot（会报 Read RESP failed，无害）
upgcmd shcmd "reset"       # U-Boot → 启动固件（会报 No such device，无害）
```

两条命令都会**报错但实际生效**，这是最容易误判的地方：

| 命令 | 报错 | 实际 |
| --- | --- | --- |
| `continue` | `aicupg_trans_recv_pkt(): Read len 13/16` | 阶段已从 `Boot ROM` 变为 **`U-Boot`** ✓ |
| `shcmd "reset"` | `CSW tag 0x2 ... No such device` | 板子正在重启、命令在途，**属预期** ✓ |

**为什么单发 `continue` 不够**：它把板子交给 **U-Boot 的升级代理**（`upgcmd -l` 会显示
`Boot stage: U-Boot`），而不是直接启动应用。要引导应用还得再发一次 `shcmd reset` ✓。

**为什么早期全部失败**：我在 **BROM 阶段**发 `shcmd` ✗ —— `shcmd` 属于 U-Boot，
BROM 不实现它 ✓。命令能否用取决于**当前阶段**，参见上面的能力表 ✓。

### `scons --aicupg` 的坑

`tools/scripts/aic_build.py` 的 `aicupg_cmd()` 是这么挑镜像的：

```python
for file in sorted(os.listdir(img_path)):
    if file.endswith('.img'):
        img_file = file
        break
```

**按字母序取第一个 `.img`**，目录里不止一个镜像时挑的不一定是你要的。用
`upgcmd image <完整路径>` 更稳。

## 两个串口，脾气相反

这块板上有两个 USB 串口，**DTR/RTS 的期望正好相反**：

| 口 | 识别 | DTR/RTS | 写方向 |
| --- | --- | --- | --- |
| CH340 | `1a86:7523` | **必须拉低** | ❌ 多字节必乱 |
| WCH `UART+SPI+I2C+JTAG` | `1a86:55de` | **必须拉高** | ✅ 正常 |

前者的症状极具误导性：**读方向完全干净、写方向多字节损坏**，看起来像"波特率不对"，
其实是断言 DTR/RTS 往板子的 RX 注入了噪声。实测四组对照：

```
dtr=True  rts=True  -> 乱码 + 提示符
dtr=False rts=False -> 干净            ← CH340 要这个
dtr=True  rts=True  -> 干净且有回应    ← WCH 要这个
```

**用 `tio` 默认设置读 WCH 那个口会全是乱码**（它的默认映射/流控与此口不合）。

### 读控制台必须先发一个回车

**只读不写会得到 0 字节，看起来像控制台死了，其实不是** ✗ —— RT-Thread 的 shell 不会主动打印，
得先敲一次回车它才回提示符。可复用脚本：

```python
s = serial.Serial("/dev/ttyACM1", 115200, timeout=0.3, rtscts=False, dsrdtr=False)
s.dtr = s.rts = True       # 这个口必须拉高
time.sleep(0.4); s.reset_input_buffer()
s.write(b"\r\n"); s.flush()   # ← 关键，缺了这一步永远是 0 字节
```

区分"控制台哑了"与"没发回车"的办法：**发回车后能读到 `ZXM47D0N />` 就说明通道是好的** ✓。

### 分区布局

`nlist`（RT-Thread shell）可读，实测：

| 分区 | 块 | 大小 |
| --- | --- | --- |
| `spl` | 0–7 | 1024 KB |
| `refresh` | 8–15 | 1024 KB |
| `env` | 16–17 | 256 KB |
| `env_r` | 18–19 | 256 KB |
| `os` | 20–51 | — |

每块 64 页 × 2048 B，spare 64 B（与镜像头的 `P=2K B=128K` 一致）。

## 相关

- USB 通路的约定与实测见 [usb.md](usb.md)。
