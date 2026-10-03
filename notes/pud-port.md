# PUD 协议移植到本板

> 目标：让这块 ArtInChip 板作为 **PUD 设备端**，主机侧（内核驱动 + `pudctl` 等工具）**一行不改**就能用它。
> 实现落在 `packages/third-party/cherryusb/demo/pud_vendor.{c,h}`。

**协议本身不在这里定义**：权威定义是另一仓库的 `PUD-kernel-drivers/notes/usb-protocol.md` ✓，
本文件只记**本移植的实现与状态** ✓。改协议字段必须两处同步 ✓。

## TL;DR

- 设备已能枚举为 **`2e8a:0001 ZXRTT PUD Display`**（High-Speed）✓，`GET_CAPS` 已能正确应答 ✓
- 刻意沿用 PUD 的 VID/PID 与描述符形状 ✓，所以主机侧无需改动 ✓
- **主机必须"先挂 EP2 读、再发控制传输"** ✓ —— 原因见 [usb.md](usb.md) 的端口缺陷一节
- 状态：P2.2 握手已通；EP1 图像流与测速（P2.3）尚未开始

## 实现结构

| 项 | 值 | 说明 |
| --- | --- | --- |
| VID:PID | `0x2E8A:0x0001` | **与 PUD 相同**，主机驱动按此匹配 ✓ |
| 接口 | 1 个，class `0xFF`（厂商专用） | 无类驱动 ✓ |
| 端点 | EP1 bulk OUT（图像）、EP2 bulk IN（查询）、EP4 interrupt 64B bInterval 8（触摸） | 与协议一致 ✓ |
| 批量 MPS | **512（HS）** / 64（FS） | 由 `CONFIG_USB_HS` 选择 ✓；RP2350 只有 FS 所以是 64 ✓ |
| 注册方式 | `INIT_DEVICE_EXPORT(pud_vendor_init)` | RT-Thread 自动初始化，与仓库其它设备类一致 ✓ |

**请求帧**（主机 → 设备的控制 OUT）：`bRequest = REQ_EP2_IN(0x03)`，载荷 4 字节
`struct req_ep2_in { u16 cmd; u16 size; }` ✓；设备把应答写进缓冲后**另起一次 EP2 IN** ✓。
命令号在**载荷里**，不在 `wValue`/`wIndex` ✓。

**能力上报**（`PUD_CMD_GET_CAPS`，32 字节）：`magic "PUDC"` / `proto_ver 2` / `frame_max` /
`decoder_type`，其后是面板参数 ✓。本板（A 板，lg4572b 480×800 无触摸）实测回读：

```text
50554443 02000000 00000100 03000000 e001 2003 ...   → "PUDC" / v2 / 65536 / QOI / 480 / 800 ✓
```

## 构建接线

| 文件 | 改动 |
| --- | --- |
| `packages/third-party/cherryusb/Kconfig` | 新增 `LPKG_CHERRYUSB_DEVICE_PUD` ✓ |
| `packages/third-party/cherryusb/SConscript` | `src += Glob('demo/pud_vendor.c')` ✓（缩进见 [usb.md](usb.md) 的陷阱 ✓） |
| `target/configs/ZXM47D0N_rtt_lg4572b_defconfig` | 开 PUD、**关 CDC 模板**（会抢控制器）✓ |

## 状态（带证据）

| 环节 | 状态 | 证据 |
| --- | --- | --- |
| 编译进固件 | ✅ | 构建日志有 `CC .../demo/pud_vendor.c` ✓ |
| 枚举 | ✅ | `lsusb` → `2e8a:0001 ZXRTT PUD Display`，`dmesg` 报 `high-speed` ✓ |
| 描述符 | ✅ | `lsusb -v`：厂商接口 `0xFF`、3 端点、bulk MPS **512**、intr 64B/bInterval 8 ✓ |
| 控制传输 | ✅ | 直发 `ctrl_transfer` 返回 4 ✓ |
| `GET_CAPS` 送达 | ✅ | 先挂读 → EP2 收到 32 字节，字段正确 ✓ |
| **`pudctl caps` 端到端** | ✅ | 退出码 0；`xres 480 / yres 800 / bpp 16 / rotation 0 / decoder 3 / touch False` 全部正确 ✓ |
| EP1 图像流 + 测速 | ⏳ 未开始 | P2.3 |

> **曾报过一个假警报**：我手工解码字节时误判 `rotation`/`bpp` 对调 ✗，实际是正确的 ✓。
> 当时标注为"待核对"而非结论 ✓，所以更正只需划掉一行 ✓。
> **主机侧需一处时序修正**：EP2 的读要**先挂、再发控制传输**（见 [usb.md](usb.md)）✓。

## 本地 USB 烧写通道（已实现 ✓，仍**不进 PUD 协议**）

迭代耗时的瓶颈是厂商路径（1.01 MB/s，一次 5.6 s ✗）。现在走一个**本工程临时**的通道：
第二个厂商接口 + EP3 bulk OUT ✓，落盘交给 SDK 自带的 OTA 层 ✓，实测 **1.5 s** ✓（移出中断后约 0.3 s ✓）。

**它不属于协议** ✓：命令号用自己的空间 ✓、端点自己定义 ✓、本地文件不 include 协议头 ✓；
受 `CONFIG_ZX_LOCAL_USB_FLASH` 保护 ✓ —— **宏关掉时 PUD 的描述符与命令逐字等于原协议** ✓。

架构、踩过的五个坑与证据、A/B 切换实测、边界：见 [usb-flash.md](usb-flash.md) ✓。

## 相关

- USB 启用链、端口 IN 缺陷、日志宏陷阱、构建接线：[usb.md](usb.md)
- 构建/烧录/控制台读法：[build-and-flash.md](build-and-flash.md)
