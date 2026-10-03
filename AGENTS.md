# AGENTS.md

> 本仓库是启明智显（ZX）开发板的 **RT-Thread SDK**（SoC：**ArtInChip，CPU 核 T-Head SMART**），
> 面板驱动 + 触摸 + LVGL。工作区根 [`../AGENTS.md`](../AGENTS.md) 的通用约定（知识库、测试、敏感信息）
> 全部适用，本文只写本仓特有的铁律与入口；细节见 [`notes/README.md`](notes/README.md)。

## 铁律

1. **未经明确指令，不要 `git commit`，更不要 `git push`。**
2. **仓库内不得出现内网/个人信息**：本机绝对路径、内网 IP、口令、代理地址、板子序列号。
3. **改配置必须改 `target/configs/*_defconfig`，不是 `.config`。**
   `scons --menuconfig` 会**从 defconfig 重新生成 `.config`**，手改 `.config` 会被静默冲掉
   （实测：改了 USB 选项后跑一次 `--menuconfig`，改动全部消失，构建照旧）。
4. **`toolchain/` 与 `output/` 不进 git**（前者是解压出来的交叉工具链，后者是构建产物）。
   工具链由 `tools/toolchain/*.tar.gz` 解压而来，**不要在仓库里提交它**。
5. **只写已验证的结论**；推测显式标注"未验证"。

## 提交与身份

- `user.name` = `Wooden Chair`，`user.email` = `hua.zheng@embeddedboys.com`；`git commit -s`
- 内核风格提交信息（`模块: 组件: 简述`），正文写清改了什么、为什么、实测效果
- 一个逻辑改动一个提交

## 构建 / 烧录 / 验证入口

```bash
# 构建（工具链不在 PATH 里，要显式加）
export PATH="$(pwd)/toolchain/bin:$PATH"
scons -j8                       # 产物: output/<板子>/images/*.img

# 让板子进升级模式：在它的 RT-Thread shell 上敲 `aicupg`，或 BOOT+RESET
tools/scripts/upgcmd -l                                   # 确认识别到设备
tools/scripts/upgcmd -p image output/<板子>/images/<img>   # 烧录
tools/scripts/upgcmd shcmd reset                          # hmm: BROM 阶段不实现 shcmd
```

- **BROM 阶段只实现烧写路径**（`image`/内存读写）；读 flash 的命令（分区表、dump）属于
  **第二阶段代理**，在 BROM 阶段不可用 ✗
- **`scons --aicupg` 会烧 `output/<板子>/images/` 下按字母序第一个 `.img`**，
  目录里不止一个镜像时挑的不一定是你要的 —— 用 `upgcmd image <完整路径>` 更稳
- 细节与踩过的坑见 [notes/build-and-flash.md](notes/build-and-flash.md)

## 架构不变量（动了就坏）

1. **USB 的启用是一条链，且板级的 `select` 不落地。**
   顺序是 `AIC_USING_USB0`(Device) → `AIC_USB_DEVICE_DRV` → `LPKG_USING_CHERRYUSB` +
   `LPKG_CHERRYUSB_DEVICE`。但 `target/m4/common/Kconfig.board` 里那句
   `select AIC_USB_DEVICE_DRV` **不会出现在生成的 `rtconfig.h` 里**，
   于是 `packages/third-party/cherryusb/SConscript` 的 `GetDepend(['AIC_USB_DEVICE_DRV'])`
   判假、**整个 CherryUSB 被静默跳过**（一行 USB 代码都不编，构建却成功）。
   **必须在 defconfig 里显式写 `CONFIG_AIC_USB_DEVICE_DRV=y`。**
   详见 [notes/usb.md](notes/usb.md)。
2. **CherryUSB 是本平台的 USB 栈**，且带专门的 ArtInChip 移植（`port/aic/`）。
   它与 Pico-USB-Display 固件用的是同一套栈，协议层可以搬运而不是重写。
3. **面板驱动一屏一文件**（`bsp/zx/drv/display/panel/panel_*.c`），
   与 pico-display-lib 的 `tft_<model>.c` 同构。
4. **触摸坐标只在 `indev` 那一层变换**（沿用 pico-display-lib 的约定）；
   各触摸驱动（`bsp/peripheral/touch/`，含 FT6236）只返回控制器原始值，
   不要往驱动里塞 `set_dir()`。

## 当前配置（改前先读 notes）

| 配置 | 值 |
| --- | --- |
| 目标板 | `ZXM47D0N`（芯片 `m4`，内核 `rt-thread`，应用 `benchmark`） |
| 面板 | **lg4572b 480×800，无触摸** |
| 存储 | SPI NAND（`P=2K B=128K`） |
| 控制台 | WCH `UART+SPI+I2C+JTAG` 那个口，**DTR 必须拉高** |
| USB | 已启用：USB0 设备模式 + CherryUSB（HS 实测，见 notes/usb.md） |

## 相关

- 知识库索引：[notes/README.md](notes/README.md)
- 通用约定（知识库/测试/退出码/敏感信息）：工作区根 [`../AGENTS.md`](../AGENTS.md)
- 同平台姊妹工作：`Pico-USB-Display`（PUD 固件，同样的协议与 CherryUSB）
