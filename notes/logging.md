# USB 日志环与命令

> `zxlogctl.py` 通过 interface 1 的 EP3 IN 读取启动以来的日志；超时或短响应必须视为失败。

## TL;DR

```bash
python3 tools/zxlogctl.py read
python3 tools/zxlogctl.py stats
python3 tools/zxlogctl.py run "list_thread" --follow
```

- 日志环由 `zx_logring.c` 保存，默认容量 256 KiB；控制台设备名应为 `zxconsole`。
- 日志请求使用 `wIndex=1`、EP `0x83`；PUD handler 必须只处理 `wIndex=0`。
- `read` 以零长度包结束；USB 超时、USB 错误、stats 少于 16 bytes 均返回非零。
- 实机启动后可见显示初始化、`zxcmd` 启动和 `dropped=0`。

## 机制

`zx_usb_flash.c` 提供 `LOG_READ (0x82)`、`RUN_CMD (0x83)` 和 `LOGSTAT (0x84)`。`RUN_CMD` 只在 USB 回调中复制命令，由线程调用 `msh_exec()`；输出再进入日志环。工具 `run --follow` 随后读取同一环，不依赖固定 `sleep`。

控制台必须在 BSP 注册 `uart0` 后再次抢占，否则 `rt_kprintf` 不会进入日志环。诊断时看 `stats` 中的 `dropped`，并确认控制台设备名。

## 诊断

```bash
python3 tools/zxlogctl.py info
python3 tools/zxlogctl.py stats
```

`info` 应显示 interface 1 和 `0x83/in`。若请求超时，先检查设备是否已配置、`wIndex` 是否为 1、PUD handler 是否错误地接受了 interface 1。不要把超时解释为“日志读完”；正常结束是零长度读。

UART 控制台为 115200 baud，适合恢复和早期启动，不适合完整日志采集；串口输出也会改变被测时序。

## 证据与边界

已验证 `stats` 返回 `head/tail/seq/dropped`，启动日志包含 `zxdisp fb 480x800 stride 1600 bpp 16 smem 768000`、`zxdisp started` 和数据分区挂载。日志环是诊断通道，不是持久化日志；设备重启后环内容重新建立。

## 相关

- USB 请求分流：[usb.md](usb.md)
- OTA 后验证：[usb-flash.md](usb-flash.md)
- 实现：`application/os/widgets/zx_logring.c`、`application/os/widgets/zx_usb_flash.c`
