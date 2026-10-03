# zx-rtt-sdk 知识库

> 本目录记录本 SDK 的可复用工程结论；协议定义以源码和对应上游文档为准，历史调试过程不在此保存。

## 先看这里

| 文档 | 回答的问题 |
| --- | --- |
| [build-and-flash.md](build-and-flash.md) | 如何构建、进入 BROM 并恢复完整镜像，或使用运行态 USB OTA |
| [usb.md](usb.md) | USB 启用链、端点限制和接口请求分流 |
| [pud-port.md](pud-port.md) | PUD 协议在 ZXM47D0N 上的接口、能力值和验证状态 |
| [usb-flash.md](usb-flash.md) | `zxflashctl.py` 的 cpio OTA 流程、A/B 切换和边界 |
| [logging.md](logging.md) | `zxlogctl.py` 日志环、命令执行和诊断方法 |
| [boot-flow.md](boot-flow.md) | BROM、启动分区和 OTA 后的启动证据 |
| [performance-baseline.md](performance-baseline.md) | 当前板卡配置下的 USB 与 OTA 观察值 |

## 维护规则

- 文档先写结论和可执行步骤，再写原因与证据；只保留可复用信息。
- 默认值、请求号、端点和路径每次修改都要与代码做漂移检查；冲突时以代码和实测为准。
- 速率是带条件的观察值，不是协议上限；未验证内容明确标注。
- 不写本机绝对路径、地址、口令、序列号或临时日志；不提交构建产物和工具链。
