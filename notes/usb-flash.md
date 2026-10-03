# 运行态 USB OTA

> `zxflashctl.py` 发送构建生成的 `ota.cpio`，设备校验传输后写入非活动 A/B 分区并自动重启。

## TL;DR

- 输入必须是 `images/ota.cpio`，不能把 ArtInChip 的 `.img` 容器直接交给该工具。
- 主机流程是 `START(size, CRC32)`、2048 字节分块、`STOP`；设备只有在大小和 CRC32 都匹配时才结束 OTA 并复位。
- 成功标准包括 `start -> 8`、`stop -> 0`、设备重新枚举，以及启动日志中的新 A/B 分区和正常挂载。
- 失败时工具返回非零并尝试发送 `STOP` 清理会话；若设备仍无法启动，用 BROM 完整镜像恢复。

## 用法

```bash
python3 tools/zxflashctl.py flash \
  output/ZXM47D0N_rtt_lg4572b_pud/images/ota.cpio
```

工具发送前检查 cpio magic（`070701` 或 `070702`），并将文件补齐到 2048 字节倍数。当前构建的归档通常包含：

```text
m4_os.itb
rodata.fatfs
data.fatfs
```

设备端由 SDK OTA 层负责解析、擦除、写入和 A/B 目标选择；本地通道不直接操作裸 flash。`aic_upgrade_start()` 选择非活动侧，成功 `STOP` 后调用 `aic_upgrade_end()` 和 `rt_hw_cpu_reset()`。

## 证据

最近一次完整实测：归档三个成员正确，补齐后发送 `5117952` bytes，`start -> 8`、`stop -> 0`；设备立即重新枚举。启动日志包含 `zxdisp fb 480x800`、`zxdisp started`、`mount fs[elm] ... blk_data`，`zxlogctl stats` 显示 `dropped=0`。本板配置下 OTA 观察速率约 1.55--1.58 MB/s。

## 为什么有这些限制

- OTA 解析器读取 cpio，而不是 `.img` 容器。
- OTA 内部队列按 2048 字节工作；更大的主机块会导致队列溢出。
- 归档传输完整不等于设备已切换。必须检查 STOP 返回、重枚举和启动日志。
- interface 1 的请求处理器检查 `wIndex == 1`，与 PUD interface 0 分流。

## 故障恢复

传输中断或验证失败不会触发切换。若设备已处于不可启动状态，使用 BOOT+RESET 进入 BROM，再用厂商 `upgcmd image <完整 .img>` 恢复全部分区；BROM 阶段不能使用读 flash 或 `shcmd` 代理命令。

## 边界

这是 `CONFIG_ZX_LOCAL_USB_FLASH` 保护的项目开发通道，不属于 PUD 协议，也不替代 BROM 完整镜像恢复。当前接口仍在 USB 回调中喂 OTA，速度与 NAND 写入行为取决于板上 SDK 实现。

## 相关

- 总体构建与恢复：[build-and-flash.md](build-and-flash.md)
- 日志和状态验证：[logging.md](logging.md)
- 本地实现：`application/os/widgets/zx_usb_flash.c`、`tools/zxflashctl.py`
