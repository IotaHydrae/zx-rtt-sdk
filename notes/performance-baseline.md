# EP1 链路性能基线（High-Speed）

> 本板 USB 是 **High-Speed**，实测 EP1 批量接收 **39.3 MB/s**（重复性 0.5%）。
> RP2350 那条 Full-Speed 路线是 **1.148 MB/s**，已经吃到 FS 物理界的 94% —— 换平台的目的就是离开那面墙。

## TL;DR

| 量 | 值 |
| --- | --- |
| 线速（原始比特率） | 480 Mb/s = **60 MB/s**（**载荷达不到** ✗） |
| HS bulk 文档上限 | **53.248 MB/s**（`13 × 512 B / 125 µs 微帧`） |
| 真实主机常见区间 | ~40–45 MB/s |
| **本板实测（`height_320`）** | **≈39.3 MB/s**，运行间离散度 **0.5%** |
| RP2350 对比（FS） | 1.148 MB/s（= FS 界 1.216 MB/s 的 94%） |

**60 MB/s 不可达**：线速要扣掉每包的 SYNC/PID/CRC/EOP 成帧 ✓，以及每个数据包的
IN token 与 ACK 各成一体 ✓。53.248 MB/s 才是 bulk 的界，实测 ~40 MB/s 属正常。

## 测量方法（重要）

- **小载荷点不能用来下结论** ✗：`height_8`（11 KB）的运行间跳动达 **25%**，固定开销主导 ✓。
- 报数必须带**运行间重复性**（同条件连跑 N 次）✓，**不能拿"跨尺寸离散度"顶替** ✗ ——
  两者是不同来源的方差：跨尺寸差异是**固有的**（每笔传输的固定开销），运行间差异才是噪声 ✓。
- `height_320`（444 KB）重复性 **0.5%** ✓ ⇒ 判断差异要用这一档 ✓。

## 扫描数据（干净基线，LVGL 已停）

| 高度 | 载荷 | 中位耗时 | 速率 |
| --- | --- | --- | --- |
| 8 | 11115 B | 0.29–0.36 ms | 30.8–38.5 MB/s（噪声大 ✗） |
| 64 | 88896 B | 2.24–2.43 ms | 36.6–39.6 MB/s |
| 320 | 444352 B | 11.28–11.34 ms | **39.2–39.4 MB/s** ✓ |

## Oracle 不适用于本板（已知）

`Pico-USB-Display/tests/test_ep1_throughput.py` 的 oracle 是

```text
[ORACLE] SPEC  [EXPECTED] no sweep height may exceed 1.216 MB/s
```

那是 **Full-Speed 的物理界** ✓，作用在 HS 设备上必然报 `FAIL` ✗ —— **设备没问题，是 oracle 需要按平台分档** ✓
（FS → 物理界 `SPEC` ✓；HS → 实测基线 `BASELINE` ✓，不编造阈值 ✓）。

## 干扰项：LVGL demo

widgets demo 会让 `zx_gui` 线程常驻 runnable，并且**场景切换导致 CPU 占用随时间波动** ✓。
对**这条链路**的测量，它的影响不可检出 ✓（`height_320` 上两组数值重叠 ✓）。但

- 测量应当**先排除干扰项** ✓，而不是事后用噪声去论证可以忽略 ✓；
- **下一步的解码/刷屏是 CPU 密集的** ✓ —— 那里干扰会现形 ✓，所以干净基线是必须的起点 ✓。

停掉它的正确位置：`application/os/widgets/main.c` 的 **`INIT_APP_EXPORT(zx_gui_app_init)`** ✓ ——
`benchmark/main.c` 里的 `zx_gui_init()` **不是**创建点（那里调了也不会有线程 ✓），
而 `CONFIG_ZX_WIDGETS_DEMO` 选的是**一整组应用文件** ✓，关掉它会连 `main()` 一起消失 ✗
（`undefined reference to 'main'`）。

## 相关

- USB 通路与端口缺陷：[usb.md](usb.md)
- 移植实现与状态：[pud-port.md](pud-port.md)
