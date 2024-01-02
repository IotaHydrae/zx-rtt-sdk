//#include <stdio.h>

#include <hal_pke.h>

#ifdef AIC_PKE_SEC

#include "eccp_sec_common.h"
#include <utility_sec.h>

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
//crc16(p||p_h||p_n0||a||b||Gx||Gy||n||n_h||n_n0)
#define SM2_CURVE_CRC16                    (0x2A42)
#define SECP160K1_CURVE_CRC16              (0x2B5B)
#define SECP192K1_CURVE_CRC16              (0x3ACF)
#define SECP224K1_CURVE_CRC16              (0x77BA)
#define SECP256K1_CURVE_CRC16              (0xC0F9)
#define BRAINPOOLP160R1_CURVE_CRC16        (0x1A8C)
#define SECP160R1_CURVE_CRC16              (0x2814)
#define SECP160R2_CURVE_CRC16              (0x3CA8)
#define SECP192R1_CURVE_CRC16              (0xDFC4)
#define SECP224R1_CURVE_CRC16              (0x781C)
#define SECP256R1_CURVE_CRC16              (0xA389)
#define SECP384R1_CURVE_CRC16              (0xA477)
#define BRAINPOOLP512R1_CURVE_CRC16        (0x6A20)
#define SECP521R1_CURVE_CRC16              (0xDA47)
#else
//crc16(p||p_h||a||b||Gx||Gy||n||n_h||half_Gx||half_Gy)
#define SM2_CURVE_CRC16                    (0xED1F)
#define SECP160K1_CURVE_CRC16              (0x2E0F)
#define SECP192K1_CURVE_CRC16              (0xA280)
#define SECP224K1_CURVE_CRC16              (0xE6F4)
#define SECP256K1_CURVE_CRC16              (0x0B26)
#define BRAINPOOLP160R1_CURVE_CRC16        (0x5F15)
#define SECP160R1_CURVE_CRC16              (0xBCCA)
#define SECP160R2_CURVE_CRC16              (0xF36C)
#define SECP192R1_CURVE_CRC16              (0xC82F)
#define SECP224R1_CURVE_CRC16              (0x4B0B)
#define SECP256R1_CURVE_CRC16              (0xC360)
#define SECP384R1_CURVE_CRC16              (0x2BED)
#define BRAINPOOLP512R1_CURVE_CRC16        (0xAC2E)
#define SECP521R1_CURVE_CRC16              (0x4E3C)
#endif




/* function: check whether eccp curve defined internal
 * parameters:
 *     curve ---------------------- input, eccp_curve struct pointer
 * return: 1(yes), 0(no)
 * caution:
 *     1. curve should not be null
 */
u32 is_eccp_curve_defined_internal(const eccp_curve_t *curve)
{
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);

    if(160 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_BRAINPOOLP160R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, brainpoolp160r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP160K1
        if(0 == uint32_BigNumCmp(curve->eccp_Gy, pWordLen, secp160k1->eccp_Gy, pWordLen)) //since secp160k1 and secp160r2 have the same p
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP160R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp160r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP160R2
        if(0 == uint32_BigNumCmp(curve->eccp_Gy, pWordLen, secp160r2->eccp_Gy, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(192 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_SECP192R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp192r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP192K1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp192k1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(224 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_SECP224R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp224r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP224K1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp224k1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(256 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_SECP256R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp256r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef SUPPORT_SECP256K1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp256k1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

#ifdef AIC_PKE_SM2_SUPPORT
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, sm2_curve->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(384 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_SECP384R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp384r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(512 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_BRAINPOOLP512R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, brainpoolp512r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    if(521 == curve->eccp_p_bitLen)
    {
#ifdef SUPPORT_SECP521R1
        if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp521r1->eccp_p, pWordLen))
        {
            return 1;
        }
        else
        {;}
#endif

        return 0;
    }
    else
    {;}

    return 0;
}


/* function: calculate eccp curve crc16
 * parameters:
 *     curve ---------------------- input, eccp_curve struct pointer
 * return: crc value
 * caution:
 *     1. curve should not be null
 */
u16 calc_eccp_curve_crc16(const eccp_curve_t *curve)
{
    u32 pByteLen = ((curve->eccp_p_bitLen + 31)/32)*4;
    u32 nByteLen = ((curve->eccp_n_bitLen + 31)/32)*4;
    u8 *addr;
    u16 crc = 0xFFFF;

    addr = (u8 *)(curve->eccp_p);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_p_h);
    if (NULL != addr)
    {
        crc = crc16_calc(addr, pByteLen, crc);
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    addr = (u8 *)(curve->eccp_p_n0);
    if (NULL != addr)
    {
        crc = crc16_calc(addr, 4, crc);
    }
    else
    {;}
#endif

    addr = (u8 *)(curve->eccp_a);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_b);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_Gx);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_Gy);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_n);
    crc = crc16_calc(addr, nByteLen, crc);

    addr = (u8 *)(curve->eccp_n_h);
    if (NULL != addr)
    {
        crc = crc16_calc(addr, nByteLen, crc);
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    addr = (u8 *)(curve->eccp_n_n0);
    if (NULL != addr)
    {
        crc = crc16_calc(addr, 4, crc);
    }
    else
    {;}
#else
    addr = (u8 *)(curve->eccp_half_Gx);
    crc = crc16_calc(addr, pByteLen, crc);

    addr = (u8 *)(curve->eccp_half_Gy);
    crc = crc16_calc(addr, pByteLen, crc);
#endif

    return crc;
}


/* function: check crc16 value of ecc curve
 * parameters:
 *     curve ---------------------- input, eccp_curve struct pointer
 *     curve_crc16 ---------------- input, expected crc16
 * return: 0(success), 1(error)
 * caution:
 *     1. curve should not be null
 */
u32 ecc_crc16_check(const eccp_curve_t *curve, u16 curve_crc16)
{
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);

    if(is_eccp_curve_defined_internal(curve))
    {
        if(160 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_BRAINPOOLP160R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, brainpoolp160r1->eccp_p, pWordLen))
            {
                curve_crc16 = BRAINPOOLP160R1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP160K1
            if(0 == uint32_BigNumCmp(curve->eccp_Gy, pWordLen, secp160k1->eccp_Gy, pWordLen))
            {
                curve_crc16 = SECP160K1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP160R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp160r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP160R1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP160R2
            if(0 == uint32_BigNumCmp(curve->eccp_Gy, pWordLen, secp160r2->eccp_Gy, pWordLen))
            {
                curve_crc16 = SECP160R2_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(192 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_SECP192R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp192r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP192R1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP192K1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp192k1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP192K1_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(224 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_SECP224R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp224r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP224R1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP224K1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp224k1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP224K1_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(256 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_SECP256R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp256r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP256R1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef SUPPORT_SECP256K1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp256k1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP256K1_CURVE_CRC16;
            }
            else
            {;}
#endif

#ifdef AIC_PKE_SM2_SUPPORT
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, sm2_curve->eccp_p, pWordLen))
            {
                curve_crc16 = SM2_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(384 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_SECP384R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp384r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP384R1_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(512 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_BRAINPOOLP512R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, brainpoolp512r1->eccp_p, pWordLen))
            {
                curve_crc16 = BRAINPOOLP512R1_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}

        if(521 == curve->eccp_p_bitLen)
        {
#ifdef SUPPORT_SECP521R1
            if(0 == uint32_BigNumCmp(curve->eccp_p, pWordLen, secp521r1->eccp_p, pWordLen))
            {
                curve_crc16 = SECP521R1_CURVE_CRC16;
            }
            else
            {;}
#endif
        }
        else
        {;}
    }
    else
    {;}

    return (curve_crc16 == calc_eccp_curve_crc16(curve))?0:1;
}


/* function: init internal sec eccp curve struct eccp_curve_t
 * parameters:
 *     ctx ------------------------ input, eccp_sec_ctx_t struct pointer
 *     curve ---------------------- input, eccp_curve_t struct pointer
 * return: eccp_curve_t pointer(success), NULL(error)
 * caution:
 */
eccp_curve_t * eccp_curve_init(eccp_sec_ctx_t *ctx, const eccp_curve_t *curve)
{
    u32 pWordLen, nWordLen;
    eccp_curve_t * ctx_curve = ctx->curve;

    if((curve->eccp_p_bitLen < 160) || (curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN))
    {
        return NULL;
    }
    else
    {;}

    pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);

    ctx_curve->eccp_p_bitLen = curve->eccp_p_bitLen;
    ctx_curve->eccp_n_bitLen = curve->eccp_n_bitLen;
    ctx_curve->eccp_p        = ctx->eccp_curve_mem;
    ctx_curve->eccp_p_h      = ctx->eccp_curve_mem + pWordLen;

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ctx_curve->eccp_p_n0     = ctx->eccp_curve_mem + 0 + (pWordLen << 1);
    ctx_curve->eccp_a        = ctx->eccp_curve_mem + 1 + (pWordLen << 1);
    ctx_curve->eccp_b        = ctx->eccp_curve_mem + 1 + (pWordLen << 1) + pWordLen;
    ctx_curve->eccp_Gx       = ctx->eccp_curve_mem + 1 + (pWordLen << 2);
    ctx_curve->eccp_Gy       = ctx->eccp_curve_mem + 1 + (pWordLen << 2) + pWordLen;
    ctx_curve->eccp_n        = ctx->eccp_curve_mem + 1 + (pWordLen << 2) + (pWordLen << 1);
    ctx_curve->eccp_n_h      = ctx->eccp_curve_mem + 1 + (pWordLen << 2) + (pWordLen << 1) + nWordLen;
    ctx_curve->eccp_n_n0     = ctx->eccp_curve_mem + 1 + (pWordLen << 2) + (pWordLen << 1) + (nWordLen << 1);
#else
    ctx_curve->eccp_a        = ctx->eccp_curve_mem + (pWordLen << 1);
    ctx_curve->eccp_b        = ctx->eccp_curve_mem + (pWordLen << 1) + pWordLen;
    ctx_curve->eccp_Gx       = ctx->eccp_curve_mem + (pWordLen << 2);
    ctx_curve->eccp_Gy       = ctx->eccp_curve_mem + (pWordLen << 2) + pWordLen;
    ctx_curve->eccp_n        = ctx->eccp_curve_mem + (pWordLen << 2) + (pWordLen << 1);
    ctx_curve->eccp_n_h      = ctx->eccp_curve_mem + (pWordLen << 2) + (pWordLen << 1) + nWordLen;
    ctx_curve->eccp_half_Gx  = ctx->eccp_curve_mem + (pWordLen << 2) + (pWordLen << 1) + (nWordLen << 1);
    ctx_curve->eccp_half_Gy  = ctx->eccp_curve_mem + (pWordLen << 2) + (pWordLen << 1) + (nWordLen << 1) + pWordLen;
#endif

    uint32_copy(ctx_curve->eccp_p, curve->eccp_p, pWordLen);
    if (NULL != curve->eccp_p_h)
    {
        uint32_copy(ctx_curve->eccp_p_h, curve->eccp_p_h, pWordLen);
    }
    else
    {
        ctx_curve->eccp_p_h = NULL;
    }

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    if (NULL != curve->eccp_p_n0)
    {
        uint32_copy(ctx_curve->eccp_p_n0, curve->eccp_p_n0, 1);
    }
    else
    {
        ctx_curve->eccp_p_n0 = NULL;
    }
#endif

    uint32_copy(ctx_curve->eccp_a,  curve->eccp_a,  pWordLen);
    uint32_copy(ctx_curve->eccp_b,  curve->eccp_b,  pWordLen);
    uint32_copy(ctx_curve->eccp_Gx, curve->eccp_Gx, pWordLen);
    uint32_copy(ctx_curve->eccp_Gy, curve->eccp_Gy, pWordLen);
    uint32_copy(ctx_curve->eccp_n,  curve->eccp_n,  nWordLen);

    if (NULL != curve->eccp_n_h)
    {
        uint32_copy(ctx_curve->eccp_n_h,  curve->eccp_n_h, nWordLen);
    }
    else
    {
        ctx_curve->eccp_n_h = NULL;
    }

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    if (NULL != curve->eccp_n_n0)
    {
        uint32_copy(ctx_curve->eccp_n_n0, curve->eccp_n_n0, 1);
    }
    else
    {
        ctx_curve->eccp_n_n0 = NULL;
    }
#else
    uint32_copy(ctx_curve->eccp_half_Gx, curve->eccp_half_Gx, pWordLen);
    uint32_copy(ctx_curve->eccp_half_Gy, curve->eccp_half_Gy, pWordLen);
#endif

    return ctx_curve;
}


/* function: uninit eccp curve struct
 * parameters: 
 *     ctx ------------------------ input, eccp_sec_ctx_t struct pointer
 * return: none
 * caution:
 */
void eccp_curve_uninit(eccp_sec_ctx_t *ctx)
{
    uint32_clear((u32 *)ctx, (sizeof(eccp_sec_ctx_t))/4);
}

#endif

