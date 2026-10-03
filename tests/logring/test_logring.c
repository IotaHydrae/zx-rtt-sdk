/*
 * ORACLE: INVARIANT
 * SOURCE: notes/logging.md -- 环的写入与读取必须始终自洽
 * EXPECTED: 读取步进永不越界；head 永不回退；坏长度不被信任
 */
#define ZXRING_HOST_TEST 1
#include "rt_stub.h"
#include <time.h>
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
#define rtthread_h_guard
#include "zx_logring.c"

static int failures;
#define CHECK(cond, ...) do { if (!(cond)) { printf("  FAIL: "); printf(__VA_ARGS__); \
    printf("  (%s:%d)\n", __FILE__, __LINE__); failures++; } } while (0)

int main(void)
{
    char line[64], out[512];
    rt_size_t n;
    rt_uint32_t seq, head_before, dropped;
    int i;

    printf("① 写入大量记录（远超环容量），head 必须永不回退\n");
    head_before = 0;
    for (i = 0; i < 20000; i++) {
        snprintf(line, sizeof(line), "record %d\n", i);
        zxring_write(line, strlen(line));
        { rt_uint32_t h,t,s,d; zxring_stats(&h,&t,&s,&d);
          CHECK(h >= head_before, "head 回退了: %u -> %u", head_before, h);
          CHECK(h <= ZXRING_SIZE, "head 越出缓冲: %u", h);
          head_before = h; }
    }
    { rt_uint32_t h,t,s,d; zxring_stats(&h,&t,&s,&d);
      dropped = d;
      printf("   head=%u seq=%u dropped=%u（dropped>0 说明满了在可见地丢 ✓）\n", h,s,d);
      CHECK(d > 0, "写满后 dropped 仍为 0，说明丢弃是静默的"); }

    printf("② 逐条读回：序号必须严格递增、长度必须合法\n");
    {
        double t0, t1; int calls = 0;
        seq = 0; t0 = now();
        for (i = 0; i < 200000; i++) {
            rt_uint32_t got = zxring_next(seq + 1, out, sizeof(out) - 1, &n);
            calls++;
            if (!got) break;
            seq = got;
        }
        t1 = now();
        printf("   全量 dump：%d 次调用（每次内部线性扫描）耗时 **%.3f 秒**\n", calls, t1 - t0);
        printf("   单次调用平均 %.1f 微秒；记录数约 %u\n", (t1-t0)/calls*1e6, seq);
    }
    seq = 0;
    for (i = 0; i < 200000; i++) {
        rt_uint32_t got = zxring_next(seq + 1, out, sizeof(out) - 1, &n);
        if (!got) break;
        CHECK(n <= ZXRING_MAX_LINE, "读回长度 %zu 超过 ZXRING_MAX_LINE", n);
        CHECK(got == seq + 1, "序号跳变: %u -> %u", seq, got);
        out[n] = 0;
        seq = got;
    }
    printf("   读回 %d 条，最后 seq=%u\n", i, seq);
    CHECK(seq > 0, "一条都没读出来");

    printf("③ 越界请求：from_seq 极大时不得读到东西\n");
    CHECK(zxring_next(0xFFFFFF00u, out, sizeof(out), &n) == 0, "极大 seq 竟然读到了记录");

    printf("\n%s（失败 %d 项）\n", failures ? "FAIL" : "PASS", failures);
    return failures ? 1 : 0;
}
