#if (defined(AIC_PKE_SM2_SUPPORT) && defined(SM2_SEC))

#include <sm2.h>
#include <hash_kdf.h>
#include <trng.h>
#include <utility_sec.h>
#include "eccp_sec_common.h"

extern const u32 sm2p256v1_n_1[8];
extern const eccp_curve_t sm2_curve[1];

#define SM2_SEC_SIGN_COUNTER        (0x64D0B4FEU)
#define SM2_SEC_SIGN_COUNTER1       (0x3A53F102U)
#define SM2_SEC_SIGN_COUNTER2       (0x9F758C1EU)
#define SM2_SEC_DEC_COUNTER         (0x157A396AU)
#define SM2_SEC_EXC_COUNTER         (0xF5264A87U)

/* function: Generate SM2 Signature s from k,dA,r(secure version)
 * parameters:
 *     sm2_curve ------------------ input, SM2_CURVE struct pointer
 *     k[8]   --------------------- input, random number k, 8 words, little-endian
 *     dA[8]  --------------------- input, private key, 8 words, little-endian
 *     r[8]   --------------------- input, Signature r, 8 words, little-endian
 *     s[8]   --------------------- output, Signature s, 8 words, little-endian
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. please make sure the inputs are all valid
 *     2. s = ((1+dA)^(-1))*(k-r*dA) mod n
 */
static u32 sm2_sign_get_sec_s(eccp_curve_t *curve, u32 k[8], u32 dA[8], u32 r[8], u32 s[8])
{
    u32 tmp1[SM2_WORD_LEN], tmp2[SM2_WORD_LEN];
    u32 t1[SM2_WORD_LEN], t2[SM2_WORD_LEN];
    u32 ret = SM2_ERROR_S;
    volatile u32 count = SM2_SEC_SIGN_COUNTER1;

    //make t1 < n
    do {
        ret = get_rand((u8 *)t1, SM2_BYTE_LEN);
        if(TRNG_SUCCESS != ret)
        {
            return SM2_ERROR_S;
        }
        else
        {;}
    }while(uint32_BigNumCmp_sec(t1, SM2_WORD_LEN, curve->eccp_n, SM2_WORD_LEN) >= 0);

    (void)get_rand_fast((u8 *)tmp2, 8);

    count++;

    //sleep random number
    uint32_sleep(tmp2[0]&0x1FF, (u8)(tmp2[0] >> 24));

    count++;

    /////////// tmp1 = (dA - t1) mod n
    ret = pke_modsub(curve->eccp_n, dA, t1, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //sleep random number
    uint32_sleep(tmp2[1]&0x1FF, (u8)(tmp2[1] >> 24));

    count++;

    /////////// tmp1 = (dA - t1 + 1) mod n
    pke_set_operand_uint32_value(tmp2, SM2_WORD_LEN, 1);
    ret = pke_modadd(curve->eccp_n, tmp1, tmp2, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    count++;

    uint32_clear(t2, SM2_WORD_LEN);
    ret = get_rand((u8 *)t2, 8);
    if(TRNG_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    /////////// tmp1 = (dA - t1 + 1)*t2 mod n
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_modmul_internal(tmp1, t2, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp2 = t1*t2 mod n
    ret = pke_modmul_internal(t1, t2, tmp2, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp1 = (tmp1+tmp2) mod n = ((dA+1)*t2) mod n
    ret = pke_modadd(curve->eccp_n, tmp1, tmp2, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp1 = (tmp1)^(-1) mod n = ((dA+1)*t2)^(-1) mod n
    ret = pke_modinv(curve->eccp_n, tmp1, tmp1, SM2_WORD_LEN, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp2 = (k+r) mod n
    ret = pke_modadd(curve->eccp_n, k, r, tmp2, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp2 = (k+r)*(t2) mod n
    //pke_load_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
    ret = pke_modmul_internal(tmp2, t2, tmp2, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// tmp2 = (tmp1)*(tmp2) mod n = (((dA+1)*t2)^(-1))*((k+r)*(t2)) mod n = ((dA+1)^(-1))*(k+r) mod n
    ret = pke_modmul_internal(tmp1, tmp2, tmp2, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    /////////// s = (tmp2 - r) mod n = (((dA+1)^(-1))*(k+r) - r) mod n = ((dA+1)^(-1))*(k+r-(dA+1)r) mod n = ((dA+1)^(-1))*(k-r*dA) mod n
    ret = pke_modsub(curve->eccp_n, tmp2, r, s, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    if(count != SM2_SEC_SIGN_COUNTER1 + 0x0FU)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS_S;

END:

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)s, SM2_BYTE_LEN);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)tmp1, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)tmp2, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)t1, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)t2, SM2_BYTE_LEN);

    return ret;
}



/* function: Generate SM2 Signature r and s with rand k
 * parameters:
 *     e[8]   --------------------- input, e value, 8 words, little-endian
 *     k[8]   --------------------- input, random number k, 8 words, little-endian
 *     dA[8]  --------------------- input, private key, 8 words, little-endian
 *     r[8]   --------------------- output, Signature r, 8 words, little-endian
 *     s[8]   --------------------- output, Signature s, 8 words, little-endian
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. e and dA can not be modified
 *     2. e must be less than n(order of the SM2 curve)
 *     3. dA must be in [1, n-2]
 */
u32 sm2_sign_with_k_s(eccp_curve_t *curve, u32 e[8], u32 k[8], u32 dA[8], u32 r[8], u32 s[8])
{
    u32 k_bak[SM2_WORD_LEN];
    u32 tmp1[SM2_WORD_LEN], tmp2[SM2_WORD_LEN];
    u32 ret = SM2_ERROR_S;
    volatile u32 count = SM2_SEC_SIGN_COUNTER2;

    if(NULL == curve || NULL == e || NULL == k || NULL == dA || NULL == r || NULL == s)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    //make sure k in [1, n-1]
    ret = uint32_integer_check_sec(k, curve->eccp_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        return ret;
    }
    else
    {;}

    //backup k
    uint32_copy(k_bak, k, SM2_WORD_LEN);

    count++;

#ifdef SM2_SEC
    ret = eccp_pointMul_sec(curve, k, curve->eccp_Gx, curve->eccp_Gy, tmp1, tmp2);
#else
    ret = eccp_pointMul_base(curve, k, tmp1, tmp2);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //check the point [k]G
    if(PKE_SUCCESS != eccp_pointVerify(curve, tmp1, tmp2))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    //tmp1 = x1 mod n
    uint32_set(tmp2, 0xFFFFFFFF, SM2_WORD_LEN);
    if(uint32_BigNumCmp_sec(tmp1, SM2_WORD_LEN, curve->eccp_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_modsub(tmp2, tmp1, curve->eccp_n, tmp1, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(tmp2, e, curve->eccp_n, tmp2, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    count++;

    //r = e + x1 mod n
    ret = pke_modadd(curve->eccp_n, e, tmp1, r, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    count++;

    //make sure r is not zero
    if(uint32_BigNum_Check_Zero_sec(r, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

    count++;

    //tmp1 = r + k mod n
    ret = pke_modadd(curve->eccp_n, r, k, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    count++;

    //make sure r+k is not n
    if(uint32_BigNum_Check_Zero_sec(tmp1, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

    count++;

    //get tmp1 = s
    ret = sm2_sign_get_sec_s(curve, k, dA, r, tmp1);
    if(SM2_SUCCESS_S != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(uint32_BigNum_Check_Zero_sec(tmp1, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

    count++;

    //get s
    ret = sm2_sign_get_sec_s(curve, k, dA, r, s);
    if(SM2_SUCCESS_S != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    (void)get_rand_fast((u8 *)tmp2, 3<<2);

    //sleep random number & securely cmp
    uint32_sleep(tmp2[0] & 0x0F, (u8)(tmp2[0]>>16));
    if(uint32_cmp_sec(tmp1, s, SM2_WORD_LEN, (u8)(tmp2[0]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //sleep random number & securely cmp
    uint32_sleep(tmp2[1] & 0x0F, (u8)(tmp2[1]>>16));
    if(uint32_cmp_sec(tmp1, s, SM2_WORD_LEN, (u8)(tmp2[1]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //sleep random number & securely cmp
    uint32_sleep(tmp2[2] & 0x0F, (u8)(tmp2[2]>>16));
    if(uint32_cmp_sec(tmp1, s, SM2_WORD_LEN, (u8)(tmp2[2]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //make sure s is not zero
    if(uint32_BigNum_Check_Zero_sec(tmp1, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

    count++;

    //securely cmp k and backup
    if(uint32_cmp_sec(k_bak, k, SM2_WORD_LEN, (u8)(tmp2[0]>>8)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(count != SM2_SEC_SIGN_COUNTER2 + 0x10U)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS_S;

END:

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)r, SM2_BYTE_LEN);
        (void)get_rand_fast((u8 *)s, SM2_BYTE_LEN);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)k_bak, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)tmp1, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)tmp2, SM2_BYTE_LEN);

    return ret;
}


/* function: Generate SM2 Signature
 * parameters:
 *     E[32] ---------------------- input, E value, 32 bytes, big-endian
 *     rand_k[32] ----------------- input, random big integer k in signing, 32 bytes, big-endian,
 *                                  if you do not have this integer, please set this parameter to be NULL,
 *                                  it will be generated inside.
 *     priKey[32] ----------------- input, private key, 32 bytes, big-endian
 *     signature[64] -------------- output, Signature r and s, 64 bytes, big-endian
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 */
u32 sm2_sign_s(u8 E[32], u8 rand_k[32], u8 priKey[32], u8 signature[64])
{
    u32 e[SM2_WORD_LEN], k[SM2_WORD_LEN], dA[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN];
    u32 ret = SM2_ERROR_S;
    uint16_t eccp_curve_crc16 = 0;
    eccp_curve_t *curve;
    eccp_sec_ctx_t ctx[1];
    volatile u32 count = SM2_SEC_SIGN_COUNTER;

    if(NULL == E || NULL == priKey || NULL == signature)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    //init sm2 curve
    curve = eccp_curve_init(ctx, sm2_curve);
    if(NULL == curve)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //e = e mod n
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(E, e, SM2_WORD_LEN);
#else
    reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);
#endif
    uint32_set(k, 0xFFFFFFFF, SM2_WORD_LEN);
    if(uint32_BigNumCmp(e, SM2_WORD_LEN, curve->eccp_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_modsub(k, e, curve->eccp_n, e, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(k, e, curve->eccp_n, dA, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    count++;

    //make sure priKey in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(priKey, dA, SM2_WORD_LEN);
#else
    reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check_sec(dA, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(NULL == rand_k)
    {
        count++;

        do {
            ret = get_rand((u8 *)k, SM2_BYTE_LEN);
            if(TRNG_SUCCESS != ret)
            {
                ret = SM2_ERROR_S;
                break;
            }
            else
            {;}

            ret = sm2_sign_with_k_s(curve, e, k, dA, r, s);
        } while((SM2_ZERO_ALL == ret) || (SM2_INTEGER_TOO_BIG == ret));

        count++;
    }
    else
    {
        count++;

        reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);
        ret = sm2_sign_with_k_s(curve, e, k, dA, r, s);

        count++;
    }

    count++;

    if(SM2_SUCCESS_S != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {
#ifdef PKE_BIG_ENDIAN
        if(((u32)(signature)) & 3)
        {
            reverse_word_array((u8 *)r, r, SM2_WORD_LEN);
            reverse_word_array((u8 *)s, s, SM2_WORD_LEN);
            memcpy_(signature, r, SM2_BYTE_LEN);
            memcpy_(signature+SM2_BYTE_LEN, s, SM2_BYTE_LEN);
        }
        else
        {
            reverse_word_array((u8 *)r, (u32 *)signature, SM2_WORD_LEN);
            reverse_word_array((u8 *)s, (u32 *)(signature+SM2_BYTE_LEN), SM2_WORD_LEN);
        }
#else
        reverse_byte_array((u8 *)r, signature, SM2_BYTE_LEN);
        reverse_byte_array((u8 *)s, signature+SM2_BYTE_LEN, SM2_BYTE_LEN);
#endif
    }

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(count != SM2_SEC_SIGN_COUNTER + 0x0AU)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

END:
    eccp_curve_uninit(ctx);

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)signature, SM2_BYTE_LEN<<1);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)e, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)k, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)dA, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)r, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)s, SM2_BYTE_LEN);

    return ret;
}


/* function: Verify SM2 Signature
 * parameters:
 *     E[32] ---------------------- input, E value, 32 bytes, big-endian
 *     pubKey[65] ----------------- input, public key(0x04 + x + y), 65 bytes, big-endian
 *     signature[64] -------------- input, Signature r and s, 64 bytes, big-endian
 * return:
 *     SM2_SUCCESS_S(success, the signature is valid); other(error or the signature is invalid)
 * caution:
 */
u32 sm2_verify_s(u8 E[32], u8 pubKey[65], u8 signature[64])
{
    u32 e[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN], tmp[SM2_WORD_LEN*4];
    u32 *t = e;
    eccp_curve_t *curve;
    uint16_t eccp_curve_crc16 = 0;
    u32 ret = SM2_ERROR_S;
    eccp_sec_ctx_t ctx[1];

    if(NULL == E || NULL == pubKey || NULL == signature)
    {
        return SM2_ERROR_S;
    }
    else if(POINT_UNCOMPRESSED != pubKey[0])    //make sure pubKey[0] is POINT_UNCOMPRESSED
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    //init sm2 curve
    curve = eccp_curve_init(ctx, sm2_curve);
    if(NULL == curve)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //get PA and check PA
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(pubKey+1, tmp+2*SM2_WORD_LEN, SM2_WORD_LEN);
    reverse_word_array(pubKey+1+SM2_BYTE_LEN, tmp+3*SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(pubKey+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
    reverse_byte_array(pubKey+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify(curve, (u32 *)(tmp+2*SM2_WORD_LEN), (u32 *)(tmp+3*SM2_WORD_LEN));
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    //make sure r in [1, n-1]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(signature, r, SM2_WORD_LEN);
#else
    reverse_byte_array(signature, (u8 *)r, SM2_BYTE_LEN);
#endif

    ret = uint32_integer_check(r, curve->eccp_n, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //make sure s in [1, n-1]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(signature+SM2_BYTE_LEN, s, SM2_WORD_LEN);
#else
    reverse_byte_array(signature+SM2_BYTE_LEN, (u8 *)s, SM2_BYTE_LEN);
#endif

    ret = uint32_integer_check(s, curve->eccp_n, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //t = (r+s) mod n
    ret = pke_modadd((u32 *)curve->eccp_n, r, s, t, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //if t is 0, refuse the signature
    if(uint32_BigNum_Check_Zero(t, SM2_WORD_LEN))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifdef SM2_HIGH_SPEED
    ret = eccp_pointMul_Shamir_safe(curve,
                                    s, (u32 *)curve->eccp_Gx, (u32 *)curve->eccp_Gy,
                                    t, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                                    tmp, NULL);
#else
    //[s]G
    ret = eccp_pointMul(curve, s, curve->eccp_Gx, curve->eccp_Gy, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //[t]PA
    ret = eccp_pointMul(curve, t, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN, tmp+2*SM2_WORD_LEN,
                        tmp+3*SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //[s]G + [t]PA
    ret = eccp_pointAdd(curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                        tmp, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    //e = e mod n
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(E, e, SM2_WORD_LEN);
#else
    reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);
#endif

    uint32_set(tmp+SM2_WORD_LEN, 0xFFFFFFFF, SM2_WORD_LEN);
    if(uint32_BigNumCmp(e, SM2_WORD_LEN, curve->eccp_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_modsub(tmp+SM2_WORD_LEN, e, curve->eccp_n, e, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(tmp+SM2_WORD_LEN, s, curve->eccp_n, s, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    //tmp = x1 mod n
    if(uint32_BigNumCmp(tmp, SM2_WORD_LEN, curve->eccp_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_modsub(tmp+SM2_WORD_LEN, tmp, curve->eccp_n, tmp, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(tmp+SM2_WORD_LEN, s, curve->eccp_n, s, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    //tmp = e + x1 mod n
    ret = pke_modadd((u32 *)curve->eccp_n, e, tmp, tmp, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    (void)get_rand_fast((u8 *)e, 3<<2);

    //sleep random number & securely cmp
    uint32_sleep(e[0] & 0x0F, (u8)(e[0] >> 16));
    if(uint32_cmp_sec(tmp, r, SM2_WORD_LEN, (u8)(e[0] >> 24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(e[1] & 0x0F, (u8)(e[1] >> 16));
    if(uint32_cmp_sec(tmp, r, SM2_WORD_LEN, (u8)(e[1]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(e[2] & 0x0F, (u8)(e[2] >> 16));
    if(uint32_cmp_sec(tmp, r, SM2_WORD_LEN, (u8)(e[2]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //success
    ret = SM2_SUCCESS_S;

END:

    (void)get_rand_fast((u8 *)e, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)r, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)s, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)tmp, SM2_BYTE_LEN*4);

    t = NULL;
    eccp_curve_uninit(ctx);

    return ret;
}


/* function: SM2 Encryption with rand k
 * parameters:
 *     M -------------------------- input, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- input, byte length of M
 *     k[8] ----------------------- input, random number k, 8 words, little-endian
 *     pubkey_x ------------------- input, x coordinate of public key point, 8 words, little-endian
 *     pubkey_y ------------------- input, y coordinate of public key point, 8 words, little-endian
 *     order ---------------------- input, either SM2_C1C3C2 or SM2_C1C2C3
 *     C -------------------------- output, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- output, byte length of C, should be MByteLen+97 if success
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 *     2. please make sure pubkey_x and pubkey_y are valid
 */
u32 sm2_encrypt_with_k_s(eccp_curve_t *curve, u8 *M, u32 MByteLen, u32 *k,
                            u32 *pubkey_x, u32 *pubkey_y,
                            sm2_cipher_order_e order,
                            u8 *C, u32 *CByteLen)
{
    u8 counter[4] = {0,0,0,1};
    u32 xy[SM2_WORD_LEN<<1];
    u8 *C2, *C3;
    int32_t i;
    u32 ret = SM2_ERROR_S;

    HASH_NODE hash_node[3];  //since M and C may point the same address, please do not initialize hash_node here.

    if(NULL == curve || NULL == M || NULL == k || NULL == pubkey_x || NULL == pubkey_y || NULL == C || NULL == CByteLen)
    {
        return SM2_ERROR_S;
    }
    else if(0 == MByteLen)
    {
        return SM2_ERROR_S;
    }
    else if(order > SM2_C1C2C3)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    C2 = C+1+2*SM2_BYTE_LEN + ((SM2_C1C2C3 == order)?0:SM2_BYTE_LEN);
    C3 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?MByteLen:0);

    //not support M and C crossing, but support M = C
    if(M > C)
    {
        if(C + MByteLen+1+3*SM2_BYTE_LEN > M)
        {
            return SM2_ERROR_S;
        }
        else
        {;}
    }
    else if(M < C)
    {
        if(M + MByteLen > C)
        {
            return SM2_ERROR_S;
        }
        else
        {;}
    }
    else  //M = C
    {
        //move M to C2, and now M = C2
        for(i=MByteLen-1; i>=0; i--)
        {
            C2[i] = M[i];
        }

        M = C2;
    }

    //make sure k in [1, n-1]
    ret = uint32_integer_check_sec(k, curve->eccp_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        return ret;
    }
    else
    {;}

    //get [k]G
#ifdef SM2_SEC
    ret = eccp_pointMul_sec(curve, k, curve->eccp_Gx, curve->eccp_Gy, xy, xy+SM2_WORD_LEN);
#else
    ret = eccp_pointMul_base(curve, k, xy, xy+SM2_WORD_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //output C1
    C[0] = POINT_UNCOMPRESSED;
#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)xy, xy, SM2_WORD_LEN);
    reverse_word_array((u8 *)(xy+SM2_WORD_LEN), xy+SM2_WORD_LEN, SM2_WORD_LEN);
    memcpy_(C+1, xy, SM2_BYTE_LEN);
    memcpy_(C+1+SM2_BYTE_LEN, xy+SM2_WORD_LEN, SM2_BYTE_LEN);
#else
    reverse_byte_array((u8 *)xy, C+1, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), C+1+SM2_BYTE_LEN, SM2_BYTE_LEN);
#endif

    //get [k]PB
    ret = eccp_pointMul_sec(curve, k, pubkey_x, pubkey_y, xy, xy+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //get x2||y2
#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)xy, xy, SM2_WORD_LEN);
    reverse_word_array((u8 *)(xy+SM2_WORD_LEN), xy+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)xy, (u8 *)xy, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif

    //get C3
    hash_node[0].msg_addr  = (u8 *)xy;
    hash_node[0].msg_bytes = SM2_BYTE_LEN;
    hash_node[1].msg_addr  = (u8 *)M;
    hash_node[1].msg_bytes = MByteLen;
    hash_node[2].msg_addr  = (u8 *)(xy+SM2_WORD_LEN);
    hash_node[2].msg_bytes = SM2_BYTE_LEN;
    ret = hash_node_steps(HASH_SM3, hash_node, 3, C3);
    if(HASH_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //get C2
    //hash_node[0].msg_addr  = (u8 *)xy;
    hash_node[0].msg_bytes = SM2_BYTE_LEN<<1;
    hash_node[1].msg_addr  = (u8 *)counter;
    hash_node[1].msg_bytes = 4;
    ret = ansi_x9_63_kdf_node_with_xor_in(HASH_SM3, hash_node, 2, 1, M, C2, MByteLen, 1);
    if(HASH_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    *CByteLen = MByteLen+1+3*SM2_BYTE_LEN;

    ret = SM2_SUCCESS_S;

END:

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast(C, MByteLen+1+3*SM2_BYTE_LEN);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)xy, SM2_BYTE_LEN<<1);
    C2 = NULL;
    C3 = NULL;
    (void)get_rand_fast((u8 *)hash_node, sizeof(hash_node));

    return ret;
}


/* function: SM2 Encryption
 * parameters:
 *     M -------------------------- input, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- input, byte length of M
 *     rand_k[32] ----------------- input, random big integer k in encrypting, 32 bytes, big-endian,
 *                                  if you do not have this integer, please set this parameter to be NULL,
 *                                  it will be generated inside.
 *     pubKey[65] ----------------- input, public key, 65 bytes, big-endian
 *     order ---------------------- input, either SM2_C1C3C2 or SM2_C1C2C3
 *     C -------------------------- output, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- output, byte length of C, should be MByteLen+97 if success
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 *     2. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 *     3. please make sure pubKey is valid
 */
u32 sm2_encrypt_s(u8 *M, u32 MByteLen, u8 rand_k[32], u8 pubKey[65],
        sm2_cipher_order_e order, u8 *C, u32 *CByteLen)
{
    u32 k[SM2_WORD_LEN];
    u32 pubkey_x[SM2_WORD_LEN],pubkey_y[SM2_WORD_LEN];
    eccp_curve_t *curve;
    uint16_t eccp_curve_crc16 = 0;
    u32 ret = SM2_ERROR_S;
    eccp_sec_ctx_t ctx[1];

    if(NULL == M || NULL == pubKey || NULL == C || NULL == CByteLen)
    {
        return SM2_ERROR_S;
    }
    else if(MByteLen == 0)
    {
        return SM2_ERROR_S;
    }
    else if(order > SM2_C1C2C3)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    if(POINT_UNCOMPRESSED != pubKey[0])
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    *CByteLen = 0;

    //init sm2 curve
    curve = eccp_curve_init(ctx, sm2_curve);
    if(NULL == curve)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(pubKey+1, pubkey_x, SM2_WORD_LEN);
    reverse_word_array(pubKey+1+SM2_BYTE_LEN, pubkey_y, SM2_WORD_LEN);
#else
    reverse_byte_array(pubKey+1, (u8 *)pubkey_x, SM2_BYTE_LEN);
    reverse_byte_array(pubKey+1+SM2_BYTE_LEN, (u8 *)pubkey_y, SM2_BYTE_LEN);
#endif

    if(PKE_SUCCESS != eccp_pointVerify(curve, pubkey_x, pubkey_y))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    if(NULL == rand_k)
    {
        do {
            ret = get_rand((u8 *)k, SM2_BYTE_LEN);
            if(TRNG_SUCCESS != ret)
            {
                ret = SM2_ERROR_S;
                break;
            }
            else
            {;}

            ret = sm2_encrypt_with_k_s(curve, M, MByteLen, k, pubkey_x, pubkey_y, order, C, CByteLen);
        } while((SM2_ZERO_ALL == ret) || (SM2_INTEGER_TOO_BIG == ret));
    }
    else
    {
        reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);
        ret = sm2_encrypt_with_k_s(curve, M, MByteLen, k, pubkey_x, pubkey_y, order, C, CByteLen);
    }

    if(SM2_SUCCESS_S != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

END:

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)C, 1+2*SM2_BYTE_LEN+SM2_BYTE_LEN+MByteLen);
    }
    else
    {;}

    eccp_curve_uninit(ctx);
    (void)get_rand_fast((u8 *)k, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)pubkey_x, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)pubkey_y, SM2_BYTE_LEN);

    return ret;
}


/* function: SM2 Decryption
 * parameters:
 *     C -------------------------- input, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- input, byte length of C, make sure MByteLen>97
 *     priKey[32] ----------------- input, private key, 32 bytes, big-endian
 *     M -------------------------- output, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- output, byte length of M, should be CByteLen-97 if success
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 */
u32 sm2_decrypt_s(u8 *C, u32 CByteLen, u8 priKey[32],
        sm2_cipher_order_e order, u8 *M, u32 *MByteLen)
{
    u8 counter[4] = {0,0,0,1};
    u32 i, temLen;
    u32 dA[SM2_WORD_LEN], xy[SM2_WORD_LEN<<1];
    u32 digest[SM2_WORD_LEN];
    u8 C3_buf[SM2_BYTE_LEN];
    u8 *C2, *C3;
    eccp_curve_t *curve;
    uint16_t eccp_curve_crc16 = 0;
    u32 ret = SM2_ERROR_S;
    eccp_sec_ctx_t ctx[1];
    volatile u32 count = SM2_SEC_DEC_COUNTER;

    HASH_NODE hash_node[3] = {
        {(u8 *)xy, SM2_BYTE_LEN<<1},
        {counter, 4},
        {(u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN},
    };

    if(NULL == C || NULL == priKey || NULL == M || NULL == MByteLen)
    {
        return SM2_ERROR_S;
    }
    else if(CByteLen <= 1+3*SM2_BYTE_LEN)                                        //97 = 1+3*ECCP_BYTELEN
    {
        return SM2_ERROR_S;
    }
    else if(order > SM2_C1C2C3)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    *MByteLen = 0;
    temLen = CByteLen-1-(3*SM2_BYTE_LEN);

    C2 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?0:SM2_BYTE_LEN);
    C3 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?temLen:0);

    //not support M and C crossing, but support M = C
    if(M > C)
    {
        if(C + CByteLen > M)
        {
            return SM2_ERROR_S;
        }
        else
        {;}
    }
    else if(M < C)
    {
        if(M + temLen > C)
        {
            return SM2_ERROR_S;
        }
        else
        {;}
    }
    else  //M = C
    {;}

    count++;

    //init sm2 curve
    curve = eccp_curve_init(ctx, sm2_curve);
    if(NULL == curve)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //make sure C1 is on the SM2 curve
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(C+1, xy, SM2_WORD_LEN);
    reverse_word_array(C+1+SM2_BYTE_LEN, xy+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(C+1, (u8 *)xy, SM2_BYTE_LEN);
    reverse_byte_array(C+1+SM2_BYTE_LEN, (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    //check the C1 point
    if(PKE_SUCCESS != eccp_pointVerify(curve, xy, xy+SM2_WORD_LEN))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    if(M == C)  //M = C
    {
        //keep C3
        memcpy_(C3_buf, C3, SM2_BYTE_LEN);
        C3 = C3_buf;

        //move C2 to M, and now M = C2
        for(i=0; i<temLen; i++)
        {
            M[i] = C2[i];
        }

        C2 = M;
    }
    else
    {;}

    count++;

    //make sure priKey in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(priKey, dA, SM2_WORD_LEN);
#else
    reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check_sec(dA, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //[dA]C1
    ret = eccp_pointMul_sec(curve, dA, xy, xy+SM2_WORD_LEN, xy, xy+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    count++;

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)xy, xy, SM2_WORD_LEN);
    reverse_word_array((u8 *)(xy+SM2_WORD_LEN), xy+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)xy, (u8 *)xy, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif

    ret = ansi_x9_63_kdf_node_with_xor_in(HASH_SM3, hash_node, 2, 1, C2, M, temLen, 1);
    if(SM2_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //hash_node[0].msg_addr  = (u8 *)xy;
    hash_node[0].msg_bytes = SM2_BYTE_LEN;
    hash_node[1].msg_addr  = (u8 *)M;
    hash_node[1].msg_bytes = temLen;
    //hash_node[2].msg_addr  = (u8 *)(xy+SM2_WORD_LEN);
    //hash_node[2].msg_bytes = SM2_BYTE_LEN;
    ret = hash_node_steps(HASH_SM3, hash_node, 3, (u8 *)digest);
    if(HASH_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    memcpy_((u8 *)xy, C3, SM2_BYTE_LEN);

    (void)get_rand_fast((u8 *)(dA), 3<<2);

    //sleep random number & securely cmp
    uint32_sleep(dA[0] & 0x0F, (u8)(dA[0]>>16));
    if(uint32_cmp_sec(xy, digest, SM2_WORD_LEN, (u8)(dA[0]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //sleep random number & securely cmp
    uint32_sleep(dA[1] & 0x0F, (u8)(dA[1]>>16));
    if(uint32_cmp_sec(xy, digest, SM2_WORD_LEN, (u8)(dA[1]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //sleep random number & securely cmp
    uint32_sleep(dA[2] & 0x0F, (u8)(dA[2]>>16));
    if(uint32_cmp_sec(xy, digest, SM2_WORD_LEN, (u8)(dA[2]>>24)))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(count != SM2_SEC_DEC_COUNTER + 0x0EU)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    *MByteLen = temLen;

    ret = SM2_SUCCESS_S;

END:

    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast(M, CByteLen-1-(3*SM2_BYTE_LEN));
    }
    else
    {;}

    (void)get_rand_fast((u8 *)&temLen, 4);
    (void)get_rand_fast((u8 *)dA, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)xy, SM2_BYTE_LEN<<1);
    (void)get_rand_fast((u8 *)digest, SM2_BYTE_LEN);
    C2 = NULL;
    C3 = NULL;
    (void)get_rand_fast((u8 *)hash_node, sizeof(hash_node));

    eccp_curve_uninit(ctx);

    return ret;
}


/* function: SM2 Key Exchange
 * parameters:
 *     role ----------------------- input, SM2_Role_Sponsor - sponsor, SM2_Role_Responsor - responsor
 *     dA[32] --------------------- input, local's permanent private key
 *     PB[65] --------------------- input, peer's permanent public key
 *     rA[32] --------------------- input, local's temporary private key
 *     RA[65] --------------------- input, local's temporary public key
 *     RB[65] --------------------- input, peer's temporary public key
 *     ZA[32] --------------------- input, local's Z value
 *     ZB[32] --------------------- input, peer's Z value
 *     kByteLen ------------------- input, byte length of output key, should be less than (2^32 - 1)bit
 *     KA[kByteLen] --------------- output, output key
 *     S1[32] --------------------- output, sponsor's S1, or responsor's S2, this is optional
 *     SA[32] --------------------- output, sponsor's SA, or responsor's SB, this is optional
 * return:
 *     SM2_SUCCESS_S(success); other(error)
 * caution: 
 *     1. please make sure the inputs are valid
 *     2. S1 and SA are optional, if you don't need, please set S1 and SA as NULL
 *     3. in case that S1(S2) and SA(SB) exist, if S1=SB,S2=SA, then exchange success.
 */
u32 sm2_exchangekey_s(sm2_exchange_role_e role,
                        u8 *dA, u8 *PB,
                        u8 *rA, u8 *RA,
                        u8 *RB,
                        u8 *ZA, u8 *ZB,
                        u32 kByteLen,
                        u8 *KA, u8 *S1, u8 *SA)
{
    u8 counter[4] = {0,0,0,1};
    u32 x1[SM2_WORD_LEN], t1[SM2_WORD_LEN], tmp[SM2_WORD_LEN<<2];
    HASH_NODE hash_node[5];
    uint16_t eccp_curve_crc16 = 0;
    eccp_curve_t *curve;
    u32 ret = SM2_ERROR_S;
    eccp_sec_ctx_t ctx[1];
    volatile u32 count = SM2_SEC_EXC_COUNTER;

    if(NULL == dA || NULL == PB || NULL == rA || NULL == RA || NULL == RB)
    {
        return SM2_ERROR_S;
    }
    else if(NULL == ZA || NULL == ZB || NULL == KA)
    {
        return SM2_ERROR_S;
    }
    else if(role > SM2_Role_Responsor)
    {
        return SM2_ERROR_S;
    }
    else if(0 == kByteLen)
    {
        return SM2_ERROR_S;
    }
    else if((POINT_UNCOMPRESSED != PB[0]) || (POINT_UNCOMPRESSED != RA[0]) || (POINT_UNCOMPRESSED != RB[0]))
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    //init sm2 curve
    curve = eccp_curve_init(ctx, sm2_curve);
    if(NULL == curve)
    {
        return SM2_ERROR_S;
    }
    else
    {;}

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(RA+1, x1, SM2_WORD_LEN);
    reverse_word_array(RA+1+SM2_BYTE_LEN, t1, SM2_WORD_LEN);
#else
    reverse_byte_array(RA+1, (u8 *)x1, SM2_BYTE_LEN);
    reverse_byte_array(RA+1+SM2_BYTE_LEN, (u8 *)t1, SM2_BYTE_LEN);
#endif
    if(PKE_SUCCESS != eccp_pointVerify(curve, x1, t1))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 curve paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    //get x1
    uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);
    x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

    //make sure rA in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(rA, t1, SM2_WORD_LEN);
#else
    reverse_byte_array(rA, (u8 *)t1, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check_sec(t1, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //t1 = x1*rA mod n
    //sleep random number
    ret = get_rand((u8 *)tmp, 12);
    if(TRNG_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(tmp[2]&0x1FF, (u8)(tmp[2] >> 24));
    uint32_clear(tmp+2, SM2_WORD_LEN-2);

    count++;

    ret = pke_modsub(curve->eccp_n, t1, tmp, tmp+SM2_WORD_LEN, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_modmul_internal(tmp, x1, tmp, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //pke_load_modulus_and_pre_monts((u32 *)curve->eccp_n, (u32 *)curve->eccp_n_h, (u32 *)curve->eccp_n_n0, curve->eccp_n_bitLen);
    //pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_modmul_internal(tmp+SM2_WORD_LEN, x1, tmp+SM2_WORD_LEN, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //make sure dA in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(dA, x1, SM2_WORD_LEN);
#else
    reverse_byte_array(dA, (u8 *)x1, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check_sec(x1, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ERROR_S, SM2_ERROR_S, 
            SM2_SUCCESS_S);
    if(SM2_SUCCESS_S != ret)
    {
        //ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //t1 = (dA + x1*rA) mod n, and it must not be 0
    ret = pke_modadd((u32 *)curve->eccp_n, tmp, x1, tmp, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    ret = pke_modadd((u32 *)curve->eccp_n, tmp, tmp+SM2_WORD_LEN, t1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2_curve->n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
#endif

    (void)get_rand_fast((u8 *)tmp, 4);
    uint32_sleep(tmp[0]&0x1FF, (u8)(tmp[0] >> 24));

    if(uint32_BigNum_Check_Zero_sec(t1, SM2_WORD_LEN))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //make sure RB on the SM2 curve
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(RB+1, tmp, SM2_WORD_LEN);
    reverse_word_array(RB+1+SM2_BYTE_LEN, tmp+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(RB+1, (u8 *)tmp, SM2_BYTE_LEN);
    reverse_byte_array(RB+1+SM2_BYTE_LEN, (u8 *)(tmp+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify(curve, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    uint32_copy(x1, tmp, SM2_WORD_LEN/2);
    uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);
    x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

#ifdef SM2_SEC
    ret = eccp_pointMul_sec(curve, x1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(PB+1, tmp+2*SM2_WORD_LEN, SM2_WORD_LEN);
    reverse_word_array(PB+1+SM2_BYTE_LEN, tmp+3*SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(PB+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
    reverse_byte_array(PB+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify(curve, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    ret = eccp_pointAdd(curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                        tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 curve paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
#endif

    ret = eccp_pointMul_sec(curve, t1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);
#else
    //x1 = tA*x2 mod n
    ret = pke_load_modulus_and_pre_monts((u32 *)curve->eccp_n, (u32 *)curve->eccp_n_h, (u32 *)curve->eccp_n_n0, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    ret = pke_modmul_internal(t1, x1, x1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(PB+1, tmp+2*SM2_WORD_LEN, SM2_WORD_LEN);
    reverse_word_array(PB+1+SM2_BYTE_LEN, tmp+3*SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(PB+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
    reverse_byte_array(PB+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
#endif

    count++;

    //[tA]PB +[tA*x2 mod n]RB
    ret = eccp_pointMul_Shamir_safe(curve,
                                    t1, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                                    x1, tmp, tmp+SM2_WORD_LEN,
                                    tmp, tmp+SM2_WORD_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

#ifndef PKE_RAM_GUARD
    //copy back sm2 paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,SM2_STEPS)), SM2_WORD_LEN);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,SM2_STEPS)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,SM2_STEPS)), SM2_WORD_LEN);
#endif

    //xU||yU
#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)tmp, tmp, SM2_WORD_LEN);
    reverse_word_array((u8 *)(tmp+SM2_WORD_LEN), tmp+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)tmp, (u8 *)tmp, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(tmp+SM2_WORD_LEN), (u8 *)(tmp+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif

    hash_node[0].msg_addr  = (u8 *)tmp;
    hash_node[0].msg_bytes = SM2_BYTE_LEN<<1;
    if(SM2_Role_Sponsor == role)
    {
        hash_node[1].msg_addr  = (u8 *)ZA;
        hash_node[2].msg_addr  = (u8 *)ZB;
    }
    else
    {
        hash_node[1].msg_addr  = (u8 *)ZB;
        hash_node[2].msg_addr  = (u8 *)ZA;
    }
    hash_node[1].msg_bytes = SM2_BYTE_LEN;
    hash_node[2].msg_bytes = SM2_BYTE_LEN;
    hash_node[3].msg_addr  = (u8 *)counter;
    hash_node[3].msg_bytes = 4;

    //KA
    ret = ansi_x9_63_kdf_node(HASH_SM3, hash_node, 4, 3, KA, kByteLen, NULL, 0);
    if(HASH_SUCCESS != ret)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    //check value is optional
    if((NULL != S1) && (NULL != SA))
    {
        //t1 = hash(xu||ZA||ZB||x1||y1||x2||y2)
        hash_node[0].msg_addr  = (u8 *)tmp;
        hash_node[0].msg_bytes = SM2_BYTE_LEN;

        if(SM2_Role_Sponsor == role)
        {
            hash_node[1].msg_addr  = ZA;
            hash_node[2].msg_addr  = ZB;
            hash_node[3].msg_addr  = RA+1;
            hash_node[4].msg_addr  = RB+1;
        }
        else
        {
            hash_node[1].msg_addr  = ZB;
            hash_node[2].msg_addr  = ZA;
            hash_node[3].msg_addr  = RB+1;
            hash_node[4].msg_addr  = RA+1;
        }

        hash_node[1].msg_bytes = SM2_BYTE_LEN;
        hash_node[2].msg_bytes = SM2_BYTE_LEN;
        hash_node[3].msg_bytes = SM2_BYTE_LEN<<1;
        hash_node[4].msg_bytes = SM2_BYTE_LEN<<1;

        ret = hash_node_steps(HASH_SM3, hash_node, 5, (u8 *)t1);
        if(HASH_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}

        //get SA = hash(0x03||yu||t1)
        ((u8 *)(tmp))[SM2_BYTE_LEN-1] = 0x03;
        hash_node[0].msg_addr  = &((u8 *)(tmp))[SM2_BYTE_LEN-1];
        hash_node[0].msg_bytes = SM2_BYTE_LEN+1;
        hash_node[1].msg_addr  = (u8 *)t1;
        hash_node[1].msg_bytes = SM2_BYTE_LEN;
        if(SM2_Role_Sponsor == role)
        {
            ret = hash_node_steps(HASH_SM3, hash_node, 2, (u8 *)SA);
        }
        else
        {
            ret = hash_node_steps(HASH_SM3, hash_node, 2, (u8 *)S1);
        }

        if(HASH_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}

        //get S1 = hash(0x02||yu||t1)
        ((u8 *)(tmp))[SM2_BYTE_LEN-1] = 0x02;
        if(SM2_Role_Sponsor == role)
        {
            ret = hash_node_steps(HASH_SM3, hash_node, 2, (u8 *)S1);
        }
        else
        {
            ret = hash_node_steps(HASH_SM3, hash_node, 2, (u8 *)SA);
        }

        if(HASH_SUCCESS != ret)
        {
            ret = SM2_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else
    {;}

    count++;

    //check crc16 of sm2 paras
    if(0 != ecc_crc16_check(curve, eccp_curve_crc16))
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    count++;

    if(count != SM2_SEC_EXC_COUNTER + 0x16U)
    {
        ret = SM2_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS_S;

END:
    if(SM2_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)KA, kByteLen);
        (void)get_rand_fast((u8 *)S1, SM2_BYTE_LEN);
        (void)get_rand_fast((u8 *)SA, SM2_BYTE_LEN);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)x1, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)t1, SM2_BYTE_LEN);
    (void)get_rand_fast((u8 *)tmp, SM2_BYTE_LEN<<2);
    (void)get_rand_fast((u8 *)hash_node, sizeof(hash_node));

    eccp_curve_uninit(ctx);

    return ret;
}
#endif

