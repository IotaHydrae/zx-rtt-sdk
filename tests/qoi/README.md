# RGB565 QOI 离线测试

编译并运行：

```bash
gcc -std=c99 -Wall -Wextra -Werror \
  -Iapplication/os/widgets \
  tests/qoi/test_qoi.c application/os/widgets/pud_qoi.c \
  -o /tmp/zx-qoi-test && /tmp/zx-qoi-test
```

测试 oracle 是原始 RGB565 像素数组：整块解码和回调解码都必须逐像素一致；截断、坏 magic、坏 padding、保留 chunk 和单字节传输补齐分别覆盖错误路径。
