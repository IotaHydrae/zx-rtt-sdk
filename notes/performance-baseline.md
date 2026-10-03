# USB 与 OTA 性能基线

> 下列数值是当前 ZXM47D0N、当前固件和 High-Speed 配置下的观察值，用于回归比较，不是协议上限。

## TL;DR

- PUD EP1 高速链路曾测得约 39.3 MB/s；它与 OTA 写入速度不是同一个指标。
- OTA 完整 cpio 实测约 1.55--1.58 MB/s，包含设备端 OTA/NAND 写入和自动切换。
- UART 115200 baud 约 11.5 KB/s，持续打印会改变 USB/烧写时序；性能测试应减少串口输出。

## 条件

结果依赖板卡、USB 主机控制器、固件构建、传输大小和是否同时写 NAND。比较时记录：端点、Speed、数据量、是否包含控制传输、是否包含闪存写入。

## 解释

EP1 数值反映 USB bulk 传输能力；`zxflashctl.py` 数值还包含 cpio 解析、擦除、分区写入、CRC 校验和 A/B 收尾。不要用其中一个推断另一个，也不要把一次观测写成标准速率。

## 验证

工具会在完成后报告发送字节数和耗时；结合 `start -> 8`、`stop -> 0`、重枚举和启动日志判断一次 OTA 是否完整成功。日志环的 `dropped` 应为 0，避免输出丢失掩盖设备状态。

## 相关

- OTA 流程：[usb-flash.md](usb-flash.md)
- 日志采集：[logging.md](logging.md)
- USB 端点：[usb.md](usb.md)
