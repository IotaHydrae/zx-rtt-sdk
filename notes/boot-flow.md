# 板子的启动流程

> 从上电到应用起来的完整链条，以及 A/B 双系统是怎么切换的。
> **本文只写实测或源码/配置支撑的事实**；没查过的一律标「未知」。

## TL;DR

```text
上电 → BROM → PBP（Pre-Boot Program，初始化 DDR2）
     → tinySPL（第二阶段）
     → 依据 env 里的 osAB_now 选择固件侧（A 或 B）
     → ZX-RTT 应用
```

- 固件在 **SPI NAND** 上（`P=2K B=128K` ✓），分区见下 ✓
- **A/B 双系统**：`os` / `os_r` 两套，靠 **env 分区里的 `osAB_now` / `osAB_next`** 选择 ✓
- 升级模式下不进应用：**BROM** 只做烧写 ✓，**U-Boot** 阶段才有读 flash / 分区等能力 ✓

## 实测的启动日志

```text
Pre-Boot Program ... (24-05-28 19:57 741f670)     ← PBP，版本与日期在括号里
USB
DDR2 64MB
Going to init DDR2. freq: 504MHz
Open Spread Spectrum
DDR2 initialized
PBP return                                        ← PBP 交棒
tinySPL [Built on Jun 17 2026 14:09:20]           ← 第二阶段
Startup reason: Power-On-Reset                    ← 复位原因（见"别踩"）
Selecting default config 'ZX-RTT firmware'
Welcome to ZX-RTT 1.3.0 [M4 Inside]
```

## 存储布局

**镜像容器**（`*.img` ✓，magic `AIC.FW` ✓，`upgcmd` 可解析 ✓）：

```text
image.info            @0x0
image.updater.ddr     @0x1800      ← 升级器用的 DDR 初始化
image.updater.spl     @0x9000      ← 升级器用的 SPL
image.target.spl      @0x3F800     → 分区 spl
image.target.env                   → 分区 env
image.target.os/.rodata/.data      → 各自分区
```

**分区表**（`partition.json` ✓，与实测 `nlist` 一致 ✓）：

```text
mtd : spl(1m) refresh(1m) env(256k) env_r(256k)
      os(4m) os_r(4m) rodata(8m) rodata_r(8m) data(15m) data_r(15m)
nftl: data:-(data)  data_r:-(data)          ← data 走文件系统层
```

每块 64 页 × 2048 B，spare 64 B ✓。

## A/B 双系统是怎么切的

```c
aic_upgrade_start()   // asystem.c:38 —— 读 osAB_now，把升级目标指向**非活动**那一侧
aic_upgrade_end()     // asystem.c:71 —— 把 osAB_next 写成另一侧，**不重启**
rt_hw_cpu_reset()     // rthw.h:65   —— 重启由调用者触发
```

**实测全程**：升级前 `osAB_now=A` / `osAB_next=A` → 写入非活动侧 → `osAB_next` 翻转 →
重启后日志出现 `mount fs[elm] device[blk_data_r] to /data ok.` ✓ 且 `osAB_now=B` ✓。

⇒ **`osAB_next` 是 bootloader 选择固件侧的依据** ✓，两个 API 必须成对调用 ✓。

## 升级模式（不进应用）

| 阶段 | 能力 | 怎么进 |
| --- | --- | --- |
| **BROM** | 烧写、内存读写 | shell 里 `aicupg` ✓，或 BOOT+RESET ✓ |
| **U-Boot**（第二阶段代理） | 上面那些 **+ 读 flash**（分区表 / dump）✓ | 在 BROM 下发 `upgcmd continue` ✓ |
| 正常启动 | 应用 | 在 U-Boot 下发 `upgcmd shcmd reset` ✓ |

`aicupg` 进的是 **BROM** —— 依据是源码注释：
`bsp/common/include/aic_reboot_reason.h` 的 `REBOOT_REASON_UPGRADE = 4, /* Goto BROM upgrade mode */` ✓。

## 别踩

`Startup reason: Power-On-Reset` **不能**当掉电证据 ✗ —— 它只是 SoC 的原因寄存器值 ✓，
据此推断过供电问题 ✗，实测供电正常 ✓。

## 未知（**没有查过，不要当结论用** ✗）

- **`refresh` 分区**的用途 ✗
- **`blk_data` / `blk_data_r` 与 `os` / `os_r` 的对应** ✗（OTA 层用前者、分区表用后者 ✓）
- **tinySPL 读哪个分区的什么字段决定 A/B** ✗（只知道结果：`osAB_now` 生效 ✓）
- **`Selecting default config 'ZX-RTT firmware'`** 的配置从哪来 ✗
- 环境变量的**冗余**（`env`/`env_r`）切换条件 ✗

## 相关

- 本地烧写通道与 A/B 实测：[usb-flash.md](usb-flash.md)
- 构建与烧录、控制台读法：[build-and-flash.md](build-and-flash.md)
- USB 通路与端口缺陷：[usb.md](usb.md)
