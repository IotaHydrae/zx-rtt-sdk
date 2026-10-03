# 本地 USB 烧写通道

> 用**已经验证过的 PUD 高速通路**把固件写进板子，绕开厂商 `upgcmd`/BROM 路径。
> 迭代耗时从 **5.6 s 降到 1.5 s**（把烧写移出中断后可达 ~0.3 s）。
> 这是**本工程临时的开发工具**，**不属于 PUD 协议**。

## TL;DR

- 设备侧：第二个厂商接口（`interface 1` ✓）+ **EP3 bulk OUT** ✓，收到即喂 `ota_shard_download_fun()` ✓
- 落盘、解包、A/B 切换全部交给**SDK 自带的 OTA 层** ✓，我们不碰裸 flash ✓
- **主机发的必须是 `images/ota.cpio`**（构建产出 ✓），**不是 `images/*.img`** ✗
- 收尾必须自己调 **`aic_upgrade_start()`**（开始）与 **`aic_upgrade_end()` + `rt_hw_cpu_reset()`**（结束）✓
- **块大小必须是 2048**（OTA 层内部队列 ≤ `OTA_BUFF_LEN`）✓

## 五个坑，每个都是"设备自己打印出来"的

按踩到的顺序，每一环都有设备侧日志作证：

| # | 现象 | 设备原话 | 根因 |
| --- | --- | --- | --- |
| 1 | `chunk 0 failed at 0 bytes` | （无日志，直接返回非零） | 发的是 **`AIC.FW` 容器** ✗，而 `ota_shard_download_fun` **按 cpio 解析** ✓ |
| 2 | `chunk 1 failed at 4096 bytes` | `E/NO_TAG: Queue overflow,please increase buffer size` | 块 4096 > 内部队列 ✗ → **改 2048** ✓ |
| 3 | `chunk 705 failed at 1443840` | `E/ota.burn: Open MTD device failed!` | cpio 里的 **`data.fatfs` 是 NFTL 分区** ✓，却走了**裸 MTD** 分支 ✗ |
| 4 | （未暴露就修掉了） | — | **漏调 `aic_upgrade_start()`** ✗ → `target_offset` 恒 0 ✗ → 会写**活动**分区 ✗ |
| 5 | （同上） | — | 漏调 **`aic_upgrade_end()`** 与重启 ✗ → 数据写了但**不切换** ✗ |

第 1 条最贵：症状是"第 0 块就失败" ✗，看起来像通道坏了 ✗，实际是**输入文件类型不对** ✗。
`zxflashctl.py` 现在会**先校验 cpio magic** ✓（`070701`/`070702` ✓）并在传入 `.img` 时指明该发哪个文件 ✓。

## A/B 是怎么切过去的

```c
aic_upgrade_start()   // asystem.c:38 —— 读 osAB_now，把目标指向**非活动**那一侧
aic_upgrade_end()     // asystem.c:71 —— 只写 osAB_next，**不重启**
rt_hw_cpu_reset()     // rthw.h:65 —— 重启由我们触发
```

**实测全程**（`osAB_now=A` 起）：

```text
升级前:  osAB_now=A  osAB_next=A
本地通道: 770560 B（只含 m4_os.itb）/ 1.510 s
重启后:  日志 mount fs[elm] device[blk_data_r] to /data ok.   ← 从 B 侧启动 ✓
          osAB_now=B ✓
```

## 为什么只发 `m4_os.itb`

构建产出的 `ota.cpio` **含三项**（`ota-subimgs.cfg` ✓）：

```text
m4_os.itb       → os 分区      ← 固件本体
rodata.fatfs    → rodata 分区
data.fatfs      → data 分区     ← 15 MB 用户数据，且是 NFTL 分区（坑 #3）
```

固件升级只需第一项 ✓；`osAB_*` 也只管 `os`/`os_r` ✓。只发它有两个好处：
**绕开 NFTL 那条路径** ✓、体积从 5.1 MB 降到 **0.77 MB** ✓。

```bash
# 产出只含固件的 cpio（dev 产物，不进仓库）
cp <output>/<board>/images/m4_os.itb /tmp/pudcpio/
printf 'm4_os.itb\n' > /tmp/pudcpio/ota-subimgs.cfg
cd /tmp/pudcpio && cat ota-subimgs.cfg | cpio -ov -H crc > ota-os-only.cpio
```

## 已知未做（收益已实测）

**烧写目前在 USB 中断上下文里做** ✗ —— 设备日志会警告
`Current mode not supported run in ISR`，代价是实测的：

```text
不烧写（纯传输）: 5112320 B / 0.251 s = 20.34 MB/s
在 ISR 里烧 NAND: 5112320 B / 3.723 s =  1.37 MB/s   ← 慢 15 倍
```

正确做法是把烧写交给线程 ✓（PUD 固件里也是同一结论：重活不能在 USB 回调里做 ✓）。

## 致命缺陷：写不完整也会重启（**再次使用前必须修**）

实测代价：本地通道在一次写入的 **4096 字节**处失败 ✗（`write failed: [Errno 5]`），
**设备照样重启** ✓ ⇒ 它带着一个**半写的系统**启动 ✗ ⇒ 板子在 USB 与串口上**都不再回应** ✗
（USB 反复 `device descriptor read/64, error -110` ✓，串口发了回车也没有任何字节 ✓）。

机制：流程是 `start` → **擦除非活动分区** → 分块写 → `end` 时切 A/B 并自动重启 ✓。
写到一半失败 ⇒ 目标分区半写 ✓，而**重启不受写没写完的影响** ✗。

**必须补的保护（顺序写死）**：
1. 写满且**回读校验**通过 ✓ —— 才允许 `aic_upgrade_end()` 与重启 ✗
2. 任何一步失败 ⇒ **不切、不重启** ✓，并把失败写进日志环 ✓（环在这是好的 ✓，正是它证明过自己 ✓）

**恢复路径（已验证 ✓）**：断电重启 ✓ → **BOOT+RESET 进 BROM** ✓ →
用厂商路径烧**完整镜像** ✓（它写全部分区 ✓，能修复半写状态 ✓）；实测一次成功 ✓。

## 边界

- **不属于 PUD 协议** ✓：命令号用自己的空间（0x80/0x81 ✓），端点自己定义 ✓，
  本地文件**不 include 协议头** ✓
- 受 `CONFIG_ZX_LOCAL_USB_FLASH` 保护 ✓：**宏关掉时 PUD 的描述符与命令逐字等于原协议** ✓
- **BROM 路径保留** ✓ 作兜底；它偶发失败（实测约 2~4 次成功一次 ✗）—— **重试即可** ✓
- 完整协议定义仍在 `PUD-kernel-drivers/notes/usb-protocol.md` ✓

## 相关

- 移植实现与状态：[pud-port.md](pud-port.md)
- 构建/烧录/控制台：[build-and-flash.md](build-and-flash.md)
- 速率基线：[performance-baseline.md](performance-baseline.md)
