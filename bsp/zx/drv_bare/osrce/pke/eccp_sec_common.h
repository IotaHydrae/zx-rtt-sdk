#ifndef ECCP_SEC_COMMON_H
#define ECCP_SEC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_pke_eccp_curve.h>

#if defined(SUPPORT_BN638)
#define ECCP_WORD_COUNT      (20)
#elif defined(SUPPORT_SECP521R1)
#define ECCP_WORD_COUNT      (17)
#elif defined(SUPPORT_BRAINPOOLP512R1)
#define ECCP_WORD_COUNT      (16)
#elif defined(SUPPORT_SECP384R1)
#define ECCP_WORD_COUNT      (12)
#elif defined(SUPPORT_BRAINPOOLP320R1)
#define ECCP_WORD_COUNT      (10)
#else
#define ECCP_WORD_COUNT      (8)
#endif

typedef struct {
    eccp_curve_t curve[1];

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    u32 eccp_curve_mem[(ECCP_WORD_COUNT << 3) + 2];
#else
    u32 eccp_curve_mem[(ECCP_WORD_COUNT << 3) + (ECCP_WORD_COUNT << 1)];
#endif
} eccp_sec_ctx_t;

u16 calc_eccp_curve_crc16(const eccp_curve_t *curve);
u32 ecc_crc16_check(const eccp_curve_t *curve, u16 curve_crc16);
eccp_curve_t *eccp_curve_init(eccp_sec_ctx_t *ctx, const eccp_curve_t *curve);
void eccp_curve_uninit(eccp_sec_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif

