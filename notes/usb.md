# USB

> 本平台的 USB 栈是 **CherryUSB**（与 Pico-USB-Display 固件同一套），带专门的 ArtInChip 移植。
> 启用它有一条链，而链上**有一处板级 `select` 不落地**，会让 CherryUSB 被静默跳过 ——
> 构建成功、一个 USB 文件都不编。

## TL;DR

- **实测：本板 USB 是 High Speed**（`dmesg` 报 `new high-speed USB device`）
- 启用链：`AIC_USING_USB0`(Device) → **`AIC_USB_DEVICE_DRV`（必须显式写）** → `LPKG_USING_CHERRYUSB` + `_DEVICE`
- 只写板级那句 `select` 不够：`rtconfig.h` 里不会出现 `AIC_USB_DEVICE_DRV`，
  `cherryusb/SConscript` 的 `GetDepend` 判假，**CherryUSB 整个被跳过**

## 启用链与那处陷阱

`target/m4/common/Kconfig.board`：

```kconfig
config AIC_USING_USB0            bool "Using Usb0"        default n
if AIC_USING_USB0
    choice "Select Usb0 mode"
        config AIC_USING_USB0_DEVICE  bool "Device"  select AIC_USB_DEVICE_DRV
```

看起来 `AIC_USING_USB0_DEVICE` 会自动带出 `AIC_USB_DEVICE_DRV`，但**实测它没有落到
生成的 `rtconfig.h` 里**，于是：

```python
# packages/third-party/cherryusb/SConscript:21
if GetDepend(['LPKG_CHERRYUSB_DEVICE']) and GetDepend(['AIC_USB_DEVICE_DRV']):
```

判假 → 整个包被跳过。**症状**：`scons` 成功、镜像照旧生成，但镜像里没有任何 USB 代码
（构建日志里 grep 不到 `cherryusb`）。

**修法**：在 defconfig 里显式写

```
CONFIG_AIC_USING_USB0=y
CONFIG_AIC_USING_USB0_DEVICE=y
CONFIG_AIC_USB_DEVICE_DRV=y          # ← 关键，不能依赖 select
CONFIG_AIC_USB_DEVICE_DRV_V10=y
CONFIG_AIC_USB_DEVICE_DEV_NUM=1
CONFIG_LPKG_USING_CHERRYUSB=y
CONFIG_LPKG_CHERRYUSB_DEVICE=y
CONFIG_LPKG_CHERRYUSB_DEVICE_HS=y
CONFIG_LPKG_CHERRYUSB_DEVICE_AIC=y
CONFIG_LPKG_CHERRYUSB_DEVICE_AIC_DMA=y
# 再挂一个功能类，验证时用最简的 CDC
CONFIG_LPKG_CHERRYUSB_DEVICE_CDC=y
CONFIG_LPKG_CHERRYUSB_DEVICE_CDC_TEMPLATE=y
```

**验证两步缺一不可**（只验一步会漏）：

1. `rtconfig.h` 里出现 `#define AIC_USB_DEVICE_DRV` ✓
2. **构建日志里出现 `CC .../cherryusb/port/aic/usb_dc_aic.c`** ✓
   —— 第 1 步过了第 2 步没过，就是 SConscript 的依赖判断出了问题

## 实测结论（已验证）

挂 CDC 模板构建烧录后：

```
usb 3-6: new high-speed USB device number 69 using xhci_hcd
usb 3-6: Product: CherryUSB CDC DEMO
cdc_acm 3-6:1.0: ttyACM3: USB ACM device
```

- **速度：High Speed** ✓ —— 与 RP2350 只有 Full-Speed 形成对比
- 固件**自己**枚举出了 USB 设备（`Product: CherryUSB CDC DEMO`，不是 BROM 的 `33c3:6677`）✓
- `cdc_acm` 正常绑定 ✓

> CDC 模板**不回应**写入（它是 demo，行为不代表栈有问题）。要验证收发得换成自己的类。

## 为什么这很重要

在 Pico-USB-Display 上实测过：Full-Speed 链路 **1.148 MB/s**，已经是该速率物理天花板
（1.216 MB/s）的 94% —— **那条路没有优化空间**。换成 HS 之后链路不再是墙，瓶颈会搬到
解码器一侧。这正是把 PUD 移植到本平台的动机。

## 陷阱：端口会报"传输完成"，而主机什么都没收到

**本板（AIC UDC）在收到 NAK 时也会上报 IN 传输完成**，所以"完成回调"不能当作"主机收到了"的证据。

现象链（每一环都有证据）：

```text
主机读 EP2 超时（500 ms / 2 s / 5 s 都超时）
  ↕ 而设备侧日志:
PUD: req 03 wLength=4           ← handler 被调用 ✓
PUD: EP2 write n=32 ret=0       ← 武装成功 ✓
PUD: EP2 in done ep=82 n=32     ← 完成回调，且 n 是从硬件剩余长度寄存器算出的 ✓
```

设备的 32 字节在**控制传输进行中**就被武装 ✓，此时主机**还没有** EP2 的读请求 ✓ ——
正常 USB 行为是主机回 NAK、设备保持挂起，但这个端口**照样报完成** ✗，数据因此丢失 ✗。

**主机侧的正确写法：先挂上读，再发控制传输。**

```python
t = threading.Thread(target=reader)   # reader 里阻塞读 EP2
t.start(); time.sleep(0.3)
dev.ctrl_transfer(0x40, 0x03, 0, 0, struct.pack("<HH", cmd, size))
t.join()
```

这个顺序在 **RP2350 与 AIC 两端都正确** ✓（RP2350 本来就会正确保持 IN 数据，先挂读只是更严谨）；
而"先发请求、再读"只在 RP2350 上侥幸成立 ✓。

> **未验证**：这是端口实现的缺陷还是本 SoC 的硬件行为。判据是 `port/aic/usb_dc_aic.c` 里
> `actual_xfer_len = xfer_len - (硬件剩余寄存器)` 得到 0，即端口认为已发完。

## 陷阱：USB 日志宏在默认配置下是空实现

```c
// common/usb_log.h
#if (CONFIG_USB_DBG_LEVEL >= USB_DBG_ERROR)
#define USB_LOG_ERR(fmt, ...) usb_dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define USB_LOG_ERR(...) {}          /* ← 本构建走这一支 */
#endif

#define USB_LOG_RAW(...) CONFIG_USB_PRINTF(__VA_ARGS__)   /* 不受等级限制 */
```

`CONFIG_USB_DBG_LEVEL` 在本构建里**未定义** ✗ ⇒ `USB_LOG_ERR` 被编译成空语句 ✗。
调试时用 `USB_LOG_RAW` ✓（在 `usb_config.h` 里展开为 `printf` ✓）。

**代价**：我曾把"日志没打印"当成"函数返回了 0" ✗ —— 而日志根本没被编译进去 ✗。
**诊断要看它是否真的会输出，再解读它的缺席。**

## 加一个新的设备类（Kconfig + SConscript）

1. `packages/third-party/cherryusb/Kconfig`：加一个 `menuconfig LPKG_CHERRYUSB_DEVICE_<名字>` ✓
2. `packages/third-party/cherryusb/SConscript`：加
   `if GetDepend([...]): src += Glob('demo/<文件>.c')` ✓
   —— **缩进必须与同级 `if` 对齐（4 空格）** ✗；插成 8 空格会嵌进上一个块里，
   外层条件不成立时**源文件根本不参与编译**，而构建依然成功 ✗
3. `target/configs/*_defconfig`：打开新选项、**关掉会抢控制器的旧类** ✓

**验证两步缺一不可**：`rtconfig.h` 里有宏 ✓ **且**构建日志里出现 `CC .../该文件.c` ✓。

## 可用资源

- `packages/third-party/cherryusb/` —— 栈本体 + `port/aic/` 移植 + `demo/` 示例
- 功能类可选：`cdc` / `hid` / `msc` / `mtp` / `audio` / `video` / `midi` / `dfu` / `template`
- PUD 的 USB 协议权威定义：`PUD-kernel-drivers/notes/usb-protocol.md`（另一仓库）

## 相关

- 构建与烧录流程见 [build-and-flash.md](build-and-flash.md)。
