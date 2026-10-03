# PUD 协议移植到 ZXM47D0N

> 本板以厂商应用 USB ID `33c3:7788` 提供 PUD 接口；协议字段以 `PUD-kernel-drivers/notes/usb-protocol.md` 和 `pud_vendor.h` 为准。

## TL;DR

- USB High-Speed 枚举成功，PUD 使用 interface 0：EP1 bulk OUT、EP2 bulk IN、EP4 interrupt IN。
- `GET_CAPS` 当前能力是 `800x480`、`rotation=90`、`bpp=16`、QOI (`decoder_type=3`)、`frame_max=65536`、无触摸。
- 主机应先挂 EP2 读，再发送控制请求；本地日志/烧写通道属于 interface 1，不是 PUD 协议。
- 能力查询和 EP1 软件 QOI 解码已端到端验证；硬件解码格式暂不在本轮范围。

## 接口事实

| 项 | 值 |
| --- | --- |
| VID:PID | `0x33c3:0x7788` |
| 接口 0 | class `0xff`，PUD 协议 |
| EP1 | bulk OUT，图像流，HS MPS 512 / FS MPS 64 |
| EP2 | bulk IN，查询应答 |
| EP4 | interrupt IN，64 bytes，interval 8 |
| 初始化 | `INIT_DEVICE_EXPORT(pud_vendor_init)` |

控制请求的命令在 `struct req_ep2_in` 数据阶段，不能放入 `wValue/wIndex`。设备收到请求后另起一次 EP2 IN 发送应答。

## 能力值

`packages/third-party/cherryusb/demo/pud_vendor.h` 定义了板级常量：

```text
xres=800, yres=480, rotation=90, bpp=16
decoder_type=3 (QOI), frame_max=65536, touch=false
```

这些值描述帧缓冲坐标系。面板物理方向与帧缓冲方向不同，不能把面板型号的 480x800 直接写成协议能力。

## 构建与验证

- `CONFIG_LPKG_CHERRYUSB_DEVICE_PUD=y` 时，构建日志应出现 `demo/pud_vendor.c`。
- `lsusb` 应看到 `ZXRTT PUD Display` 并报告 High-Speed；`pudctl caps` 退出码为 0 且字段与上表一致。
- interface 0 的处理器必须检查 `setup->wIndex == 0`。否则同一 request number 可能抢截 interface 1 的日志请求。

## 软件解码验证

`application/os/widgets/pud_qoi.c` 在独立主机测试中通过压缩/整块解码/回调解码回环，并拒绝截断流、坏 magic、坏 padding 和保留 chunk。板端验证使用 Pico-USB-Display 的 `pud_usb.py` 发送同格式 QOI：

```text
64x64 渐变带，QOI payload 4203 bytes
submitted 1 drawn 1 dropped 0 decode_failed 0
```

EP1 回调只搬运帧，解码和 framebuffer 写入在 `zxdisp` 任务中完成；`zxdisp_stats` 是当前板端验证 oracle。主机 odd payload 的单字节 USB 补齐由设备解码器接受，除此之外的尾部数据仍拒绝。

## 边界

`CONFIG_ZX_LOCAL_USB_FLASH` 打开时，interface 1 增加本地日志和 OTA；其请求号、端点和文件格式不属于 PUD，详见 [usb-flash.md](usb-flash.md) 与 [logging.md](logging.md)。

## 相关

- 协议权威定义：`PUD-kernel-drivers/notes/usb-protocol.md`
- USB 构建与端点限制：[usb.md](usb.md)
- 图像路径实现：`application/os/widgets/zx_pud_disp.c`
