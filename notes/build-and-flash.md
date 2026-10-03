# 构建与烧录

> 配置写入目标 `defconfig`；完整镜像走厂商 BROM，`ota.cpio` 走运行态 USB OTA。

## TL;DR

- 当前目标配置：`ZXM47D0N_rtt_lg4572b_pud`，lg4572b 480x800 面板，无触摸，SPI NAND。
- 改配置编辑 `target/configs/*_defconfig`，不要直接改 `.config`；menuconfig 会重新生成 `.config`。
- 构建前将仓库工具链加入 PATH；构建产物位于 `output/<board>/images/`。
- `.img` 是 BROM 完整恢复输入；`ota.cpio` 是 `zxflashctl.py` 的运行态 OTA 输入。

## 构建

```bash
export PATH="$(pwd)/toolchain/bin:$PATH"
scons -j8
```

构建后检查目标目录中实际生成的文件，不要依赖目录字母序选择镜像。配置链中 USB 必须显式包含 `CONFIG_AIC_USB_DEVICE_DRV=y`，否则板级 `select` 不会落入生成的 `rtconfig.h`，CherryUSB 可能被静默跳过。

## BROM 完整恢复

1. 在板上执行 `aicupg` 或使用 BOOT+RESET 进入升级模式。
2. 用 `tools/scripts/upgcmd -l` 确认设备。
3. 写入指定完整镜像：

```bash
tools/scripts/upgcmd -p image output/<board>/images/<完整镜像>.img
```

BROM 只实现烧写和内存路径；分区读取、dump、`shcmd` 属于应用或后续代理阶段。半写 A/B 或不可启动状态应回到此流程恢复全部分区。

## 运行态 USB OTA

```bash
python3 tools/zxflashctl.py flash \
  output/<board>/images/ota.cpio
```

主机工具校验 cpio、补齐 2048 字节并发送 START/数据/STOP；成功后设备自动复位。必须再用 `zxlogctl.py read` 或 `stats` 检查重新枚举和新侧启动。完整流程与限制见 [usb-flash.md](usb-flash.md)。

## 串口与排查

控制台是 WCH 的 UART+SPI+I2C+JTAG 口，DTR 必须拉高。UART 仅用于恢复和早期启动；持续日志优先使用 USB 日志环。若 USB 未枚举，先检查 defconfig、CherryUSB 编译日志和端点配置，再检查硬件是否处于 BROM 或应用态。

## 相关

- 启动与恢复边界：[boot-flow.md](boot-flow.md)
- USB 配置：[usb.md](usb.md)
- 日志验证：[logging.md](logging.md)
