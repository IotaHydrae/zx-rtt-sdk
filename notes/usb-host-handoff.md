# USB 主机更换交接

## 已验证结论

- LG4572B 固件可被主机枚举为 `33c3:7788`，产品字符串为 `ZXRTT PUD Display`。
- 失败发生在 USB `SET_CONFIGURATION`：Linux 日志为 `can't set config #1, error -110`。
- `video_player.py` 或 `zxflashctl.py` 的 `cannot claim`/配置错误是后续症状，视频数据尚未开始传输。
- 将 ILI9488 defconfig 镜像烧到 LG4572B 后仍复现配置超时；因此面板配置差异尚未证明是原因。
- 同一仓库提交在 ILI9488 板上已知可工作；LG4572B 的数据线、主机 USB 控制器、USB PHY/供电和板级状态仍需交叉复测。

## 换电脑后的顺序

1. 使用另一根确认支持高速数据的 USB 线，并直接连接电脑主板 USB 口，暂时不要经过扩展坞或 Hub。
2. 让板子启动应用态，确认：

   ```bash
   lsusb -d 33c3:7788
   dmesg --ctime | tail -30
   ```

3. 先读取完整描述符；命令需要秒级超时，避免卡住终端：

   ```bash
   timeout 8 lsusb -v -d 33c3:7788
   ```

4. 在设备已配置时运行：

   ```bash
   python3 tools/zxflashctl.py info
   ```

   期望看到 interface 0（PUD）和 interface 1（本地烧写/日志）。如果这里触发配置超时，先保留
   `dmesg`，不要继续测试视频传输。

5. 只有 `zxflashctl info` 成功后，才使用运行态 OTA：

   ```bash
   python3 tools/zxflashctl.py flash output/<板子>/images/ota.cpio
   ```

6. 若需要恢复完整镜像，使用 BROM 的 `upgcmd image <完整 .img 路径>`。BROM 阶段只负责烧写，
   `shcmd` 不一定可用；烧录过程中设备切换阶段出现一次断开属于正常现象，应重新运行 `upgcmd -l`
   确认当前阶段后再操作。

## 记录格式

每台主机至少记录：USB 线和端口类型、`lsusb -d 33c3:7788`、`timeout 8 lsusb -v -d 33c3:7788`
是否完成、`dmesg` 中的配置错误、`zxflashctl info` 退出码。不要把本机绝对路径、内网地址、序列号
或临时日志原文提交到仓库。
