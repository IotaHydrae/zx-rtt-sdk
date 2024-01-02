#if defined(AIC_PKE_SEC) && defined(ECDSA_SEC)

#include <hal_pke_ecdsa.h>
#include <utility_sec.h>
#include <trng.h>
#include "eccp_sec_common.h"

/* function: Generate ECDSA Signature s from k,dA,r(secure version)
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     k -------------------------- input, random number k, little-endian
 *     dA ------------------------- input, private key, little-endian
 *     r -------------------------- input, Signature r, little-endian
 *     s -------------------------- output, Signature s, little-endian
 * return:
 *     ECDSA_SUCCESS_S(success); other(error)
 * caution:
 *     1. please make sure the inputs are all valid
 */
static u32 ecdsa_sign_get_sec_s(const eccp_curve_t *curve, u32 *e, u32 *k, u32 *dA, u32 *r, u32 *s)
{
    u32 r1[ECCP_MAX_WORD_LEN], r2[ECCP_MAX_WORD_LEN];
    u32 d1[ECCP_MAX_WORD_LEN], d2[ECCP_MAX_WORD_LEN];
    u32 out1[ECCP_MAX_WORD_LEN], out2[ECCP_MAX_WORD_LEN], tmp[ECCP_MAX_WORD_LEN];
    u32 nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    u32 tmp_step;
    u32 ret = ECDSA_ERROR_S;

    uint32_clear(d1, nWordLen);
    uint32_clear(r1, nWordLen);

    //s = k^(-1) mod n
    ret = pke_modinv(curve->eccp_n, k, s, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    tmp_step = curve->eccp_n_bitLen & 0x1F;

    //make d1 < n
    do {
        ret = get_rand((u8 *)d1, nWordLen<<2);
        if(TRNG_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}

        if(tmp_step)
        {
            d1[nWordLen-1] &= (1<<tmp_step)-1;
        }
        else
        {;}
    }while(uint32_BigNumCmp_sec(d1, nWordLen, curve->eccp_n, nWordLen) >= 0);

    //make r1 < n
    do {
        ret = get_rand((u8 *)r1, nWordLen<<2);
        if(TRNG_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}

        if(tmp_step)
        {
            r1[nWordLen-1] &= (1<<tmp_step)-1;
        }
        else
        {;}
    }while(uint32_BigNumCmp_sec(r1, nWordLen, curve->eccp_n, nWordLen) >= 0);

    (void)get_rand_fast((u8 *)out1, 8);

    //sleep random number
    uint32_sleep(out1[0]&0x1FF, (u8)(out1[0] >> 24));

    /////////// d2 = (dA - d1) mod n
    ret = pke_modsub(curve->eccp_n, dA, d1, d2, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

#ifndef PKE_RAM_GUARD
    //copy back curve->eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //sleep random number
    uint32_sleep(out1[1]&0x1FF, (u8)(out1[1] >> 24));

    /////////// r2 = (r - r1) mod n
    ret = pke_modsub(curve->eccp_n, r, r1, r2, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
#else
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_bitLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //out1 =  r1*d1 mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(r1, d1, out1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    // out1 = (e + r1 * d1) mod n
    ret = pke_modadd(curve->eccp_n, e, out1, out1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //tmp =  r2*d2 mod n
    ret = pke_modmul_internal(r2, d2, tmp, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    /////////// out1 = (e + r1 * d1 + r2 * d2) mod n
    ret = pke_modadd(curve->eccp_n, tmp, out1, out1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //out1 =  out1 * K^-1 mod n
    ret = pke_modmul_internal(s, out1, out1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //out2 =  r1*d2 mod n
    ret = pke_modmul_internal(r1, d2, out2, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //tmp =  r2 * d1 mod n
    ret = pke_modmul_internal(r2, d1, tmp, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    // out2 = (r1 * d2 + r2 * d1) mod n
    ret = pke_modadd(curve->eccp_n, tmp, out2, out2, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //out2 =  out2 * K^-1 mod n
    ret = pke_modmul_internal(s, out2, out2, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    /////////// s = (out1 + out2) mod n
    ret = pke_modadd(curve->eccp_n, out1, out2, s, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    ret = ECDSA_SUCCESS_S;

END:

    if(ECDSA_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)s, nWordLen<<2);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)r1, nWordLen<<2);
    (void)get_rand_fast((u8 *)r2, nWordLen<<2);
    (void)get_rand_fast((u8 *)d1, nWordLen<<2);
    (void)get_rand_fast((u8 *)d2, nWordLen<<2);
    (void)get_rand_fast((u8 *)out1, nWordLen<<2);
    (void)get_rand_fast((u8 *)out2, nWordLen<<2);
    (void)get_rand_fast((u8 *)tmp, nWordLen<<2);

    return ret;
}


/* function: Generate ECDSA Signature in U32 little-endian big integer style
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     e -------------------------- input, derived from hash value
 *     k -------------------------- input, internal random integer k
 *     dA ------------------------- input, private key
 *     r -------------------------- output, signature r
 *     s -------------------------- output, signature s
 * return:
 *     ECDSA_SUCCESS_S(success); other(error)
 * caution:
 *     1. please make sure e is in [0,n-1], dA is in [1,n-1]
 */
u32 ecdsa_sign_uint32_s(eccp_curve_t *curve, u32 *e, u32 *k, u32 *dA, u32 *r, u32 *s)
{
    u32 tmp1[ECCP_MAX_WORD_LEN];
    u32 tmp2[ECCP_MAX_WORD_LEN];
    u32 k_bak[ECCP_MAX_WORD_LEN];
    u32 pWordLen, nWordLen, maxWordLen;
    u32 tmp_step;
    u32 ret = ECDSA_ERROR_S;

    if(NULL == curve || NULL == e || NULL == k || NULL == dA || NULL == r || NULL == s)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    maxWordLen = GET_MAX_LEN(nWordLen,pWordLen);

    //make sure k in [1, n-1]
    ret = uint32_integer_check_sec(k, curve->eccp_n, nWordLen, ECDSA_ZERO_ALL, ECDSA_INTEGER_TOO_BIG,
            ECDSA_SUCCESS_S);
    if(ECDSA_SUCCESS_S != ret)
    {
        return ret;
    }
    else
    {;}

    //backup k
    uint32_copy(k_bak, k, nWordLen);

    uint32_clear(tmp1, maxWordLen);  //for comparing with n later(suppose nWordLen >= pWordLen)

    //get x1
//    if(curve->eccp_half_Gx && curve->eccp_half_Gy)
//    {
//        ret = eccp_pointMul_base(curve, k, tmp1, tmp2);
//    }
//    else
    {
        ret = eccp_pointMul_sec(curve, k, curve->eccp_Gx, curve->eccp_Gy, tmp1, tmp2);  //y coordinate is not needed
    }

    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

#ifndef PKE_RAM_GUARD
    //copy back curve paras that not covered by output
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
    uint32_copy(curve->eccp_n,   (u32 *)(PKE_B(5,tmp_step)), nWordLen);
#endif

    //check the point [k]G
    if(PKE_SUCCESS != eccp_pointVerify(curve, tmp1, tmp2))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve paras that not covered by output
    uint32_copy(curve->eccp_b,   (u32 *)(PKE_A(4,tmp_step)), pWordLen);
    uint32_copy(curve->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
    uint32_copy(curve->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
    uint32_copy(curve->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
#endif

    //r = x1 mod n
    uint32_set(tmp2, 0xFFFFFFFF, nWordLen);
    uint32_copy(r, tmp1, nWordLen);
    if(uint32_BigNumCmp_sec(tmp1, nWordLen, curve->eccp_n, nWordLen) >= 0)
    {
        ret = pke_modsub(tmp2, tmp1, curve->eccp_n, r, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(tmp2, e, curve->eccp_n, tmp2, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    //make sure r is not zero
    if(uint32_BigNum_Check_Zero_sec(r, nWordLen))
    {
        return ECDSA_ZERO_ALL;
    }
    else
    {;}

    //get tmp1 = s
    ret = ecdsa_sign_get_sec_s(curve, e, k, dA, r, tmp1);
    if(ECDSA_SUCCESS_S != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //make sure s is not zero
    if(uint32_BigNum_Check_Zero_sec(tmp1, nWordLen))
    {
        return ECDSA_ZERO_ALL;
    }
    else
    {;}

    //get s
    ret = ecdsa_sign_get_sec_s(curve, e, k, dA, r, s);
    if(ECDSA_SUCCESS_S != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    (void)get_rand_fast((u8 *)tmp2, 3<<2);

    //sleep random number & securely cmp
    uint32_sleep(tmp2[0] & 0x1F, (u8)(tmp2[0]>>16));
    if(uint32_cmp_sec(tmp1, s, nWordLen, (u8)(tmp2[0]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(tmp2[1] & 0x1F, (u8)(tmp2[1]>>16));
    if(uint32_cmp_sec(tmp1, s, nWordLen, (u8)(tmp2[1]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(tmp2[2] & 0x1F, (u8)(tmp2[2]>>16));
    if(uint32_cmp_sec(tmp1, s, nWordLen, (u8)(tmp2[2]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //make sure s is not zero
    if(uint32_BigNum_Check_Zero_sec(tmp1, nWordLen))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //securely cmp k and backup
    if(uint32_cmp_sec(k_bak, k, nWordLen, (u8)(tmp2[0]>>8)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = ECDSA_SUCCESS_S;

END:

    if(ECDSA_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)r, nWordLen<<2);
        (void)get_rand_fast((u8 *)s, nWordLen<<2);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)k_bak, nWordLen<<2);
    (void)get_rand_fast((u8 *)tmp1, maxWordLen<<2);
    (void)get_rand_fast((u8 *)tmp2, nWordLen<<2);

    return ret;
}


/* function: Generate ECDSA Signature in byte string style
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     E -------------------------- input, hash value, big-endian
 *     EByteLen ------------------- input, byte length of E
 *     rand_k --------------------- input, random big integer k in signing, big-endian
 *     priKey --------------------- input, private key, big-endian
 *     signature ------------------ output, signature r and s, big-endian
 * return:
 *     ECDSA_SUCCESS_S(success); other(error)
 * caution:
 *     1. the method of getting big integer e from hash value E is based on SEC1 V2.
 */
u32 ecdsa_sign_s(eccp_curve_t *curve, u8 *E, u32 EByteLen, u8 *rand_k, u8 *priKey,
        u8 *signature)
{
    u32 tmpLen;
    u32 nByteLen;
    u32 nWordLen;
    u32 e[ECCP_MAX_WORD_LEN], k[ECCP_MAX_WORD_LEN], dA[ECCP_MAX_WORD_LEN];
    u32 r[ECCP_MAX_WORD_LEN], s[ECCP_MAX_WORD_LEN];
    u32 ret = ECDSA_ERROR_S;
    u16 eccp_curve_crc16;
    eccp_curve_t * curve1;
    eccp_sec_ctx_t ctx[1];

    if(NULL == curve || NULL == priKey || NULL == signature)
    {
        return ECDSA_ERROR_S;
    }
    else if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    //E could be zero
    if(NULL == E)
    {
        EByteLen = 0;
    }
    else
    {;}

    //init curve
    curve1 = eccp_curve_init(ctx, (const eccp_curve_t *)curve);
    if(NULL == curve1)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    nByteLen = GET_BYTE_LEN(curve1->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve1->eccp_n_bitLen);

    //check crc16 of curve paras
    eccp_curve_crc16 = calc_eccp_curve_crc16(curve);
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //get integer e from hash value E(according to SEC1-V2 2009)
    uint32_clear(e, nWordLen);
    if(curve1->eccp_n_bitLen >= (EByteLen<<3)) //in this case, make E as e directly
    {
        reverse_byte_array((u8 *)E, (u8 *)e, EByteLen);
    }
    else                                       //in this case, make left eccp_n_bitLen bits of E as e
    {
        reverse_byte_array((u8 *)E, (u8 *)e, nByteLen);
        tmpLen = (curve1->eccp_n_bitLen)&7;
        if(tmpLen)
        {
            Big_Div2n(e, nWordLen, 8-tmpLen);
        }
        else
        {;}
    }

    //get e = e mod n, i.e., make sure e in [0, n-1]
    uint32_set(k, 0xFFFFFFFF, nWordLen);
    if(uint32_BigNumCmp(e, nWordLen, curve1->eccp_n, nWordLen) >= 0)
    {
        ret = pke_modsub(k, e, curve1->eccp_n, e, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(k, e, curve1->eccp_n, dA, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    //make sure priKey in [1, n-1]
    dA[nWordLen - 1] = 0;
    reverse_byte_array((u8 *)priKey, (u8 *)dA, nByteLen);
    ret = uint32_integer_check_sec(dA, curve1->eccp_n, nWordLen, ECDSA_ERROR_S, ECDSA_ERROR_S,
            ECDSA_SUCCESS_S);
    if(ECDSA_SUCCESS_S != ret)
    {
        //ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //get k
    if(rand_k)
    {
        k[nWordLen - 1] = 0;
        reverse_byte_array(rand_k, (u8 *)k, nByteLen);
    }
    else
    {
ECDSA_SIGN_LOOP:
        ret = get_rand((u8 *)k, nByteLen);
        if(TRNG_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}

        //make sure k has the same bit length as n
        tmpLen = (curve1->eccp_n_bitLen)&0x1F;
        if(tmpLen)
        {
            k[nWordLen-1] &= (1<<(tmpLen))-1;
        }
        else
        {;}
    }

    //sign
    ret = ecdsa_sign_uint32_s(curve1, e, k, dA, r, s);
    if((ECDSA_ZERO_ALL == ret || ECDSA_INTEGER_TOO_BIG == ret) && (NULL == rand_k))
    {
        goto ECDSA_SIGN_LOOP;
    }
    else
    {;}

    if(ECDSA_SUCCESS_S != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //check crc16 of curve paras
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    reverse_byte_array((u8 *)r, signature, nByteLen);
    reverse_byte_array((u8 *)s, signature+nByteLen, nByteLen);
    ret = ECDSA_SUCCESS_S;

END:

    eccp_curve_uninit(ctx);
    if(ECDSA_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)signature, nByteLen<<1);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)e, nWordLen<<2);
    (void)get_rand_fast((u8 *)k, nWordLen<<2);
    (void)get_rand_fast((u8 *)dA, nWordLen<<2);
    (void)get_rand_fast((u8 *)r, nWordLen<<2);
    (void)get_rand_fast((u8 *)s, nWordLen<<2);

    return ret;
}


/* function: Verify ECDSA Signature in byte string style
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     E -------------------------- input, hash value, big-endian
 *     EByteLen ------------------- input, byte length of E
 *     pubKey --------------------- input, public key, big-endian
 *     signature ------------------ input, signature r and s, big-endian
 * return:
 *     ECDSA_SUCCESS_S(success); other(error)
 * caution:
 *     1. the method of getting big integer e from hash value E is based on SEC1 V2.
 */
u32 ecdsa_verify_s(eccp_curve_t *curve, u8 *E, u32 EByteLen, u8 *pubKey, u8 *signature)
{
    u32 tmpLen = 0;
    u32 nByteLen = 0;
    u32 nWordLen = 0;
    u32 pByteLen = 0;
    u32 pWordLen = 0;
    u32 maxWordLen = 0;
    u32 e[ECCP_MAX_WORD_LEN], r[ECCP_MAX_WORD_LEN], s[ECCP_MAX_WORD_LEN];
    u32 tmp[ECCP_MAX_WORD_LEN], x[ECCP_MAX_WORD_LEN];
    u32 tmp_step;
    u32 ret = ECDSA_ERROR_S;
    u16 eccp_curve_crc16;
    eccp_curve_t * curve1;
    eccp_sec_ctx_t ctx[1];

    if(NULL == curve || NULL == pubKey || NULL == signature)
    {
        return ECDSA_ERROR_S;
    }
    else if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    //E could be zero
    if(NULL == E)
    {
        EByteLen = 0;
    }
    else
    {;}

    //init curve
    curve1 = eccp_curve_init(ctx, (const eccp_curve_t *)curve);
    if(NULL == curve1)
    {
        return ECDSA_ERROR_S;
    }
    else
    {;}

    //check crc16 of curve paras
    eccp_curve_crc16 = calc_eccp_curve_crc16(curve);
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    nByteLen = GET_BYTE_LEN(curve1->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve1->eccp_n_bitLen);
    pByteLen = GET_BYTE_LEN(curve1->eccp_p_bitLen);
    pWordLen = GET_WORD_LEN(curve1->eccp_p_bitLen);
    maxWordLen = GET_MAX_LEN(nWordLen,pWordLen);

    //make sure r in [1, n-1]
    r[nWordLen - 1] = 0;
    reverse_byte_array(signature, (u8 *)r, nByteLen);
    ret = uint32_integer_check(r, curve1->eccp_n, nWordLen, ECDSA_ERROR_S, ECDSA_ERROR_S,
            ECDSA_SUCCESS_S);
    if(ECDSA_SUCCESS_S != ret)
    {
        //ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //make sure s in [1, n-1]
    s[nWordLen - 1] = 0;
    reverse_byte_array(signature+nByteLen, (u8 *)s, nByteLen);
    ret = uint32_integer_check(s, curve1->eccp_n, nWordLen, ECDSA_ERROR_S, ECDSA_ERROR_S,
            ECDSA_SUCCESS_S);
    if(ECDSA_SUCCESS_S != ret)
    {
        //ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp = s^(-1) mod n
    ret = pke_modinv(curve1->eccp_n, s, tmp, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve1->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //get integer e from hash value E(according to SEC1-V2 2009)
    uint32_clear(e, nWordLen);
    if(curve1->eccp_n_bitLen >= (EByteLen<<3)) //in this case, make E as e directly
    {
        reverse_byte_array((u8 *)E, (u8 *)e, EByteLen);
    }
    else                                       //in this case, make left eccp_n_bitLen bits of E as e
    {
        memcpy_((u8 *)e, E, nByteLen);
        reverse_byte_array((u8 *)E, (u8 *)e, nByteLen);
        tmpLen = (curve1->eccp_n_bitLen)&7;
        if(tmpLen)
        {
            Big_Div2n(e, nWordLen, 8-tmpLen);
        }
        else
        {;}
    }

    //get e = e mod n, i.e., make sure e in [0, n-1]
    uint32_set(x, 0xFFFFFFFF, nWordLen);
    if(uint32_BigNumCmp(e, nWordLen, curve1->eccp_n, nWordLen) >= 0)
    {
        ret = pke_modsub(x, e, curve1->eccp_n, e, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(x, x, curve1->eccp_n, x, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
#else
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_bitLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //x =  e*(s^(-1)) mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(e, tmp, x, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back eccp_n
    uint32_copy(curve1->eccp_n, (u32 *)(PKE_B(3,tmp_step)), nWordLen);
#endif

    //tmp =  r*(s^(-1)) mod n
    ret = pke_modmul_internal(r, tmp, tmp, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //check public key
    uint32_clear(e, maxWordLen);  //for comparing with n later(suppose nWordLen >= pWordLen)
    s[pWordLen - 1] = 0;
    reverse_byte_array(pubKey, (u8 *)e, pByteLen);
    reverse_byte_array(pubKey+pByteLen, (u8 *)s, pByteLen);

    ret = eccp_pointVerify(curve1, e, s);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve paras that not covered by output
    uint32_copy(curve1->eccp_b,   (u32 *)(PKE_A(4,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
#endif

#if !(defined(PKE_LP) || defined(AIC_PKE_SECURE))
    if(curve1->eccp_half_Gx && curve1->eccp_half_Gy)
    {
        ret = eccp_pointMul_Shamir(curve1, tmp, e, s, x, curve1->eccp_Gx, curve1->eccp_Gy, e, s);

        //copy back curve paras that not covered by output
        uint32_copy(curve1->eccp_p,   (u32 *)(PKE_A(0,tmp_step)), pWordLen);
        uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_B(0,tmp_step)), pWordLen);
    }
    else
#endif
    {
        ret = ~(PKE_SUCCESS);
    }

    if(PKE_SUCCESS != ret)
    {
        ret = eccp_pointMul_sec(curve1, tmp, e, s, e, s);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}

#ifndef PKE_RAM_GUARD
        //copy back curve paras that not covered by output
        uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
        uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
        uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
        uint32_copy(curve1->eccp_n,   (u32 *)(PKE_B(5,tmp_step)), nWordLen);
#endif

        if(!uint32_BigNum_Check_Zero(x, nWordLen))
        {
            if(PKE_SUCCESS !=eccp_pointMul_sec(curve1, x, curve1->eccp_Gx, curve1->eccp_Gy, x, tmp))
            {
                ret = ECDSA_ERROR_S;
                goto END;
            }
            else
            {;}

#ifndef PKE_RAM_GUARD
            //copy back curve paras that not covered by output
            uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
            uint32_copy(curve1->eccp_n,   (u32 *)(PKE_B(5,tmp_step)), nWordLen);
#endif

            if(PKE_SUCCESS != eccp_pointAdd(curve1, e, s, x, tmp, e, s))
            {
                ret = ECDSA_ERROR_S;
                goto END;
            }
            else
            {;}

#ifndef PKE_RAM_GUARD
            //copy back curve paras that not covered by output
            uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
            uint32_copy(curve1->eccp_n,   (u32 *)(PKE_B(5,tmp_step)), nWordLen);
#endif

            if(PKE_SUCCESS != eccp_pointVerify(curve1, e, s))
            {
                ret = ECDSA_ERROR_S;
                goto END;
            }
            else
            {;}

#ifndef PKE_RAM_GUARD
            //copy back curve paras that not covered by output
            uint32_copy(curve1->eccp_b,   (u32 *)(PKE_A(4,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
            uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
#endif
        }
        else
        {;}
    }
    else
    {;}

    //e = x1 mod n
    uint32_set(x, 0xFFFFFFFF, nWordLen);
    if(uint32_BigNumCmp_sec(e, nWordLen, curve1->eccp_n, nWordLen) >= 0)
    {
        ret = pke_modsub(x, e, curve1->eccp_n, e, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }
    else //fake operation
    {
        ret = pke_modsub(x, tmp, curve1->eccp_n, x, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            ret = ECDSA_ERROR_S;
            goto END;
        }
        else
        {;}
    }

    (void)get_rand_fast((u8 *)tmp, 3<<2);

    //sleep random number & securely cmp
    uint32_sleep(tmp[0] & 0x1F, (u8)(tmp[0]>>16));
    if(uint32_cmp_sec(e, r, nWordLen, (u8)(tmp[0]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(tmp[1] & 0x1F, (u8)(tmp[1]>>16));
    if(uint32_cmp_sec(e, r, nWordLen, (u8)(tmp[1]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //sleep random number & securely cmp
    uint32_sleep(tmp[2] & 0x1F, (u8)(tmp[2]>>16));
    if(uint32_cmp_sec(e, r, nWordLen, (u8)(tmp[2]>>24)))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //check crc16 of curve paras
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = ECDSA_SUCCESS_S;

END:

    (void)get_rand_fast((u8 *)e, maxWordLen<<2);
    (void)get_rand_fast((u8 *)r, maxWordLen<<2);
    (void)get_rand_fast((u8 *)s, maxWordLen<<2);
    (void)get_rand_fast((u8 *)tmp, maxWordLen<<2);
    (void)get_rand_fast((u8 *)x, maxWordLen<<2);

    eccp_curve_uninit(ctx);
    return ret;
}

#endif

