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

## 可用资源

- `packages/third-party/cherryusb/` —— 栈本体 + `port/aic/` 移植 + `demo/` 示例
- 功能类可选：`cdc` / `hid` / `msc` / `mtp` / `audio` / `video` / `midi` / `dfu` / `template`
- PUD 的 USB 协议权威定义：`PUD-kernel-drivers/notes/usb-protocol.md`（另一仓库）

## 相关

- 构建与烧录流程见 [build-and-flash.md](build-and-flash.md)。
