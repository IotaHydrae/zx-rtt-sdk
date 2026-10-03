# 环逻辑的主机侧测试

```bash
gcc -std=gnu11 -O2 -I tests/logring -I application/os/widgets \
    -o /tmp/t tests/logring/test_logring.c && /tmp/t
```

**直接 `#include` 真实的 `zx_logring.c`** ✓ —— 测的是生产代码本身，不是副本 ✓；
`rt_stub.h` 等只提供 RT-Thread 的最小类型/宏 ✓，**不修改生产文件** ✓。

它守的不变量（`ORACLE: INVARIANT` ✓）：`head` 永不回退、记录长度合法、
序号严格递增、极大 `from_seq` 读不到东西、满环后丢弃必须**可见**（`dropped` 增长 ✓）。

**价值已验证**：它**几秒钟**就否掉了"回绕缺陷导致越界"这个假设 ✓ ——
旧版代码跑同一套断言结果**完全一致**（回绕分支是死代码 ✓），
而我在板子上为此折腾了好几轮 ✗。
