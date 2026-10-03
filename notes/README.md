# zx-rtt-sdk 知识库

本目录只放**这个 SDK 自己的**知识：构建/烧录的真实流程、USB 与显示通路的约定、
以及踩过的坑。上游 RT-Thread 的文档在 `doc/`，那是上游内容，不在这里重写。

## 文档索引

| 文档 | 一句话内容 |
| --- | --- |
| [build-and-flash.md](build-and-flash.md) | 构建与烧录的可复现流程，含"改配置要改 defconfig"和两个串口 DTR 相反这两个坑 |
| [usb.md](usb.md) | USB 启用链、为什么板级 select 不生效、以及 High Speed 的实测结论 |
| [boot-flow.md](boot-flow.md) | 上电到应用的启动链、存储布局、A/B 切换与升级模式；含"未知"清单 |
| [logging.md](logging.md) | 从板子取日志：日志环 + USB 读取 + **从 USB 执行命令**；两个机制陷阱与调试方法论 |
| [usb-flash.md](usb-flash.md) | 本地 USB 烧写通道：架构、五个坑与证据、A/B 切换实测、边界 |
| [performance-baseline.md](performance-baseline.md) | EP1 链路基线（HS 实测 39.3 MB/s）、测量方法与 oracle/干扰项注意事项 |
| [pud-port.md](pud-port.md) | PUD 协议移植到本板的实现结构、构建接线与分项状态（协议权威定义在另一仓库） |
| | ↑ 其中含一条 **TODO：本工程临时的 USB 烧写通道**（不进 PUD 协议，待 RP2350 侧验证后再议） |

## 维护约定

- **构建/烧录/串口**的坑进 `build-and-flash.md`；**USB/显示/触摸**的通路约定进各自文档。
- 只写已验证的结论，推测显式标"未验证"；引用速率必须带条件（哪块板、哪个口）。
- 单篇超过约 150 行做压缩审查，超过约 300 行考虑拆分。
- **每篇都要做代码漂移检查**：文档里的默认值、选项名、路径与代码不一致时，以代码为准。
- 通用约定（提交身份、铁律、测试分层）见仓库根 `AGENTS.md` 与工作区根 `../AGENTS.md`。
