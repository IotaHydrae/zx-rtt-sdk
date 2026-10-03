# PUD 显示路径交接

> 当前代码可以离线构建，但目标板尚未验证显示输出。板端验证前必须先恢复 USB 设备并读取日志。

## 当前状态

| 项目 | 结论 | 依据 |
| --- | --- | --- |
| PUD 协议 | 已接入 EP1 band 回调 | `packages/third-party/cherryusb/demo/pud_vendor.c` |
| 显示解码 | 已实现独立任务、QOI 解码和 framebuffer blit | `application/os/widgets/zx_pud_disp.c` |
| QOI 实现 | 宿主回环通过 | 4096 像素压缩后解压与输入逐项相等 |
| SDK 构建 | 通过 | `scons -j8`，镜像已重新生成 |
| 编译证据 | 显示对象确实进入链接 | 增量日志含 `CC .../zx_pud_disp.c`；`m4.detail.csv` 含 `pud_qoi.o` 和 `zx_pud_disp.o` |
| 目标板显示 | 未验证 | 当前环境 `libusb_init()` 失败，无法连接设备 |
| adbd | 未完成 | 本仓库的 RT-Thread API 仍需移植适配 |

显示路径的静态对象约为 118 KiB：band 约 64 KiB、任务栈 8 KiB、解码缓冲约 43 KiB。该数值来自链接 map；它不能证明目标板启动一定成功。

## 实现边界

- EP1 回调只复制 band 并设置就绪标志，QOI 解码和 framebuffer 操作在 `zxdisp` 任务中执行。
- band 最大载荷为 `65524` 字节，最大解码像素数为 `21837`，与 64 KiB PUD frame 上限匹配。
- 提交入口检查矩形边界、面积和载荷长度；解码得到的像素数必须等于矩形面积。
- 未启用 `CONFIG_ZX_WIDGETS_DEMO` 时，PUD 协议仍可编译，显示入口不会产生未定义符号。
- framebuffer 使用 `AICFB_GET_SCREENINFO` 返回的 stride；写入后执行 cache clean、pan、power-on 和 vsync。
- PUD 当前身份为厂商 `0x33C3:0x7788`；驱动仍兼容旧 Pico 身份 `0x2E8A:0x0001`。端点保持不变：EP1 OUT `0x01`、EP2 IN `0x82`、EP4 IN `0x84`。本地通道使用 EP3。

## 下一步验证

1. 让板子进入 Boot ROM，确认 `tools/scripts/upgcmd -l` 能识别设备；不能识别时先按硬件流程执行 BOOT+RESET。
2. 烧写新镜像前确认镜像时间晚于源码和构建日志；不要依赖目录字母序选择镜像。
3. 物理复位后先读完整日志，再执行 PUD `caps` 请求。日志中应出现：

   ```text
   [zxdisp] fb ...
   [zxdisp] started (startup=0)
   ```

4. 发送一条已知颜色 band，检查矩形边界、方向和颜色；再重复至少三次确认没有偶发丢帧。
5. 若板端无上述日志，记录完整日志并停止推断。此时只能确认故障发生在显示初始化之前或日志通道不可用。

常用离线检查：

```bash
export PATH="$(pwd)/toolchain/bin:$PATH"
scons -j8
git diff --check
```

主机侧 QOI 回环应至少覆盖：压缩、解压、尺寸不足和截断输入。没有可靠外部阈值时，测试只能报告 `INCONCLUSIVE`，不能把一次吞吐测量写成规范。

## 已知限制

- `zx_band_ready` 是单槽背压：任务忙时新 band 会被丢弃；这保证 USB 回调不会阻塞，但尚未测量丢帧率。
- 当前显示路径没有目标板实测结果，因此旋转、颜色顺序和实际 framebuffer 地址仍是未验证事实。
- `tools/zxflashctl.py` 会把发送流补齐到 2048 字节；补齐发生在 cpio 尾部之后，设备端校验的是补齐后的 size/CRC。
- 不要把完整 OTA cpio 作为本地通道的显示验证输入；数据分区会引入与显示无关的 NFTL 风险。优先使用只含 `m4_os.itb` 的输入。

## 工作区纪律

- 不提交 `.config` 的手工修改；配置改动写入 `target/configs/*_defconfig`。
- `toolchain/` 和 `output/` 只用于本地构建，不纳入提交。
- 未完成硬件验证前，不把“能枚举”写成“协议可用”，也不把静态内存估算写成启动故障原因。
