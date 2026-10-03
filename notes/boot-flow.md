# 启动、BROM 与 A/B OTA

> BROM 负责完整镜像恢复；运行态 OTA 写入非活动 A/B 侧，成功后由设备复位进入新侧。

## TL;DR

- BOOT+RESET 进入 BROM 后，只使用厂商 `upgcmd image` 写完整 `.img`；BROM 阶段不提供读 flash 或 `shcmd` 代理。
- 运行态 USB OTA 使用 `ota.cpio`，由 `aic_upgrade_start/end` 管理 A/B 目标和切换。
- OTA 成功的启动证据是设备重新枚举、显示初始化成功、数据分区从交替的 `blk_data`/`blk_data_r` 挂载，且日志无丢失。
- 若写入中断后不能启动，用 BROM 完整镜像恢复，不依赖应用态工具。

## 两条路径

### BROM/厂商恢复

```bash
tools/scripts/upgcmd -l
tools/scripts/upgcmd -p image output/<board>/images/<完整镜像>.img
```

进入 BROM 的可靠方式是 BOOT+RESET。镜像目录有多个 `.img` 时必须写完整路径；`scons --aicupg` 按字母序选择第一个，不能作为选择依据。

### 运行态 OTA

`zxflashctl.py` 发送 cpio 数据流。`aic_upgrade_start()` 根据 `osAB_now` 将目标设为非活动侧；验证通过后 `aic_upgrade_end()` 更新下一侧，随后 `rt_hw_cpu_reset()` 重新启动。详见 [usb-flash.md](usb-flash.md)。

## 启动检查

```bash
python3 tools/zxlogctl.py read
python3 tools/zxlogctl.py stats
```

确认 `zxdisp` framebuffer、`zxcmd` 线程、数据分区挂载和 `dropped=0`。具体版本字符串和日期属于一次性构建信息，不作为长期判断条件。

## 边界

应用态 `shcmd reset` 不是 BROM 阶段的可靠必需步骤。BROM 只能执行其实现的烧写和内存路径；读分区表、dump 等代理命令属于后续阶段。

## 相关

- 可复现构建和恢复：[build-and-flash.md](build-and-flash.md)
- USB OTA：[usb-flash.md](usb-flash.md)
