# USB 启用与端点约定

> 本板必须显式启用 USB device 驱动和 CherryUSB；请求处理器按 interface 分流，端点号必须服从控制器能力。

## TL;DR

- 启用链：`AIC_USING_USB0` -> `AIC_USB_DEVICE_DRV` -> `LPKG_USING_CHERRYUSB` + `LPKG_CHERRYUSB_DEVICE`。
- `target/m4/common/Kconfig.board` 的 `select AIC_USB_DEVICE_DRV` 不会落入 `rtconfig.h`；目标 defconfig 必须写 `CONFIG_AIC_USB_DEVICE_DRV=y`。
- PUD 使用 interface 0；本地日志/OTA 使用 interface 1。两个 handler 都必须检查 `setup->wIndex`。
- 当前 HS 端点为 PUD EP1/EP2/EP4 和本地 EP3；曾使用 EP5 导致 `SET_CONFIGURATION` stall，应避免超出控制器映射的端点。

## 配置与构建

检查目标 defconfig 后运行：

```bash
export PATH="$(pwd)/toolchain/bin:$PATH"
scons -j8
```

确认 `rtconfig.h` 和构建日志同时出现 USB device 与 CherryUSB 依赖；只看到 Kconfig 的 `select` 不足以证明代码已编译。

## 接口分流

PUD 的 `pud_vendor_control_request()` 只接受 `wIndex=0`；`zx_usb_flash.c` 只接受 `wIndex=1`。请求号在两个接口中可能重叠，CherryUSB 按 handler 顺序分发；缺少分流会让 PUD handler 抢截日志请求并表现为主机超时。

## 端点

| 功能 | 端点 |
| --- | --- |
| PUD 图像 | `0x01` bulk OUT |
| PUD 查询 | `0x82` bulk IN |
| PUD 触摸 | `0x84` interrupt IN |
| 本地 OTA | `0x03` bulk OUT |
| 本地日志 | `0x83` bulk IN |

HS bulk MPS 为 512，FS 为 64；具体描述符以 `pud_vendor.c` 生成结果为准。端点地址必须在控制器支持的范围内，不能因为协议上有空闲编号就直接使用。

## 诊断

```bash
python3 tools/zxlogctl.py info
```

若 `SET_CONFIGURATION` 返回 `-EPIPE`，优先检查端点地址、描述符长度和控制器端点映射。若仅日志超时，检查 interface `wIndex` 分流和 EP `0x83` 是否已配置。

## 相关

- PUD 能力与实现：[pud-port.md](pud-port.md)
- OTA：[usb-flash.md](usb-flash.md)
- 配置与构建：[build-and-flash.md](build-and-flash.md)
