#ifdef AIC_PKE_ECDSA_SUPPORT

#include <hal_pke_ecdsa.h>
#include <utility.h>
#include <trng.h>

/* function: Generate ECDSA Signature in U32 little-endian big integer style
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     e -------------------------- input, derived from hash value
 *     k -------------------------- input, internal random integer k
 *     dA ------------------------- input, private key
 *     r -------------------------- output, signature r
 *     s -------------------------- output, signature s
 * return:
 *     ECDSA_SUCCESS(success); other(error)
 * caution:
 *     1. please make sure e is in [0,n-1], dA is in [1,n-1]
 */
u32 ecdsa_sign_uint32(eccp_curve_t *curve, u32 *e, u32 *k, u32 *dA, u32 *r, u32 *s)
{
    u32 nWordLen;
    u32 pWordLen;
    u32 tmp1[ECCP_MAX_WORD_LEN];
    u32 ret;

    if(NULL == curve || NULL == e || NULL == k || NULL == dA || NULL == r || NULL == s)
    {
        return ECDSA_POINTOR_NULL;
    }
    else if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_INVALID_INPUT;
    }
    else
    {;}

    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);

    //make sure k in [1, n-1]
    ret = uint32_integer_check(k, curve->eccp_n, nWordLen, ECDSA_ZERO_ALL, ECDSA_INTEGER_TOO_BIG,
            ECDSA_SUCCESS);
    if(ECDSA_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get x1
#if !(defined(PKE_LP) || defined(AIC_PKE_SECURE))
    if(curve->eccp_half_Gx && curve->eccp_half_Gy)
    {
        ret = eccp_pointMul_base(curve, k, tmp1, NULL);
    }
    else
    {
#endif
        ret = eccp_pointMul(curve, k, curve->eccp_Gx, curve->eccp_Gy, tmp1, NULL);  //y coordinate is not needed
#if !(defined(PKE_LP) || defined(AIC_PKE_SECURE))
    }
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //r = x1 mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_mod(tmp1, pWordLen, curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, nWordLen, r);
#else
    ret = pke_mod(tmp1, pWordLen, curve->eccp_n, curve->eccp_n_h, nWordLen, r);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else if(uint32_BigNum_Check_Zero(r, nWordLen))//make sure r is not zero
    {
        return ECDSA_ZERO_ALL;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
#else
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_bitLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp1 =  r*dA mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(r, dA, tmp1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp1 = e + r*dA mod n
    ret = pke_modadd(curve->eccp_n, e, tmp1, tmp1, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //s = k^(-1) mod n
    ret = pke_modinv(curve->eccp_n, k, s, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //s = (k^(-1))*(e + r*dA) mod n
    ret = pke_modmul_internal(s, tmp1, s, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //make sure s is not zero
    if(uint32_BigNum_Check_Zero(s, nWordLen))
    {
        return ECDSA_ZERO_ALL;
    }
    else
    {
        return ECDSA_SUCCESS;
    }
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
 *     ECDSA_SUCCESS(success); other(error)
 * caution:
 *     1. the method of getting big integer e from hash value E is based on SEC1 V2.
 */
u32 ecdsa_sign(eccp_curve_t *curve, u8 *E, u32 EByteLen, u8 *rand_k, u8 *priKey,
        u8 *signature)
{
    u32 tmpLen;
    u32 nByteLen;
    u32 nWordLen;
    u32 e[ECCP_MAX_WORD_LEN], k[ECCP_MAX_WORD_LEN], dA[ECCP_MAX_WORD_LEN];
    u32 r[ECCP_MAX_WORD_LEN], s[ECCP_MAX_WORD_LEN];
    u32 ret;

    if(NULL == curve || NULL == priKey || NULL == signature)
    {
        return ECDSA_POINTOR_NULL;
    }
    else if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_INVALID_INPUT;
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

    nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);

    //get integer e from hash value E(according to SEC1-V2 2009)
    uint32_clear(e, nWordLen);
    if(curve->eccp_n_bitLen >= (EByteLen<<3)) //in this case, make E as e directly
    {
        reverse_byte_array((u8 *)E, (u8 *)e, EByteLen);
    }
    else                                      //in this case, make left eccp_n_bitLen bits of E as e
    {
        reverse_byte_array((u8 *)E, (u8 *)e, nByteLen);
        tmpLen = (curve->eccp_n_bitLen)&7;
        if(tmpLen)
        {
            Big_Div2n(e, nWordLen, 8-tmpLen);
        }
        else
        {;}
    }

    //get e = e mod n, i.e., make sure e in [0, n-1]
    if(uint32_BigNumCmp(e, nWordLen, curve->eccp_n, nWordLen) >= 0)
    {
        ret = pke_sub(e, curve->eccp_n, e, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

    //make sure priKey in [1, n-1]
    dA[nWordLen - 1] = 0;
    reverse_byte_array((u8 *)priKey, (u8 *)dA, nByteLen);
    ret = uint32_integer_check(dA, curve->eccp_n, nWordLen, ECDSA_ZERO_ALL, ECDSA_INTEGER_TOO_BIG,
            ECDSA_SUCCESS);
    if(ECDSA_SUCCESS != ret)
    {
        return ret;
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
            return ret;
        }
        else
        {
            //make sure k has the same bit length as n
            tmpLen = (curve->eccp_n_bitLen)&0x1F;
            if(tmpLen)
            {
                k[nWordLen-1] &= (1<<(tmpLen))-1;
            }
            else
            {;}
        }
    }

    //sign
    ret = ecdsa_sign_uint32(curve, e, k, dA, r, s);
    if((ECDSA_ZERO_ALL == ret || ECDSA_INTEGER_TOO_BIG == ret) && (NULL == rand_k))
    {
        goto ECDSA_SIGN_LOOP;
    }
    else
    {;}

    if(ECDSA_SUCCESS != ret)
    {
        return ret;
    }
    else
    {
        reverse_byte_array((u8 *)r, signature, nByteLen);
        reverse_byte_array((u8 *)s, signature+nByteLen, nByteLen);

        return ECDSA_SUCCESS;
    }
}


/* function: Verify ECDSA Signature in byte string style
 * parameters:
 *     curve ---------------------- input, ecc curve struct pointer, please make sure it is valid
 *     E -------------------------- input, hash value, big-endian
 *     EByteLen ------------------- input, byte length of E
 *     pubKey --------------------- input, public key, big-endian
 *     signature ------------------ input, signature r and s, big-endian
 * return:
 *     ECDSA_SUCCESS(success); other(error)
 * caution:
 *     1. the method of getting big integer e from hash value E is based on SEC1 V2.
 */
u32 ecdsa_verify(eccp_curve_t *curve, u8 *E, u32 EByteLen, u8 *pubKey, u8 *signature)
{
    u32 tmpLen;
    u32 nByteLen;
    u32 nWordLen;
    u32 pByteLen;
    u32 pWordLen;
    u32 e[ECCP_MAX_WORD_LEN], r[ECCP_MAX_WORD_LEN], s[ECCP_MAX_WORD_LEN];
    u32 tmp[ECCP_MAX_WORD_LEN], x[ECCP_MAX_WORD_LEN];
    u32 ret;

    if(NULL == curve || NULL == pubKey || NULL == signature)
    {
        return ECDSA_POINTOR_NULL;
    }
    else if(curve->eccp_p_bitLen > AIC_PKE_ECCP_MAX_BIT_LEN)
    {
        return ECDSA_INVALID_INPUT;
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

    nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    pByteLen = GET_BYTE_LEN(curve->eccp_p_bitLen);
    pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);

    //make sure r in [1, n-1]
    r[nWordLen - 1] = 0;
    reverse_byte_array(signature, (u8 *)r, nByteLen);
    ret = uint32_integer_check(r, curve->eccp_n, nWordLen, ECDSA_ZERO_ALL, ECDSA_INTEGER_TOO_BIG,
            ECDSA_SUCCESS);
    if(ECDSA_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //make sure s in [1, n-1]
    s[nWordLen - 1] = 0;
    reverse_byte_array(signature+nByteLen, (u8 *)s, nByteLen);
    ret = uint32_integer_check(s, curve->eccp_n, nWordLen, ECDSA_ZERO_ALL, ECDSA_INTEGER_TOO_BIG,
            ECDSA_SUCCESS);
    if(ECDSA_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp = s^(-1) mod n
    ret = pke_modinv(curve->eccp_n, s, tmp, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get integer e from hash value E(according to SEC1-V2 2009)
    uint32_clear(e, nWordLen);
    if(curve->eccp_n_bitLen >= (EByteLen<<3)) //in this case, make E as e directly
    {
        reverse_byte_array((u8 *)E, (u8 *)e, EByteLen);
    }
    else                                      //in this case, make left eccp_n_bitLen bits of E as e
    {
        memcpy_((u8 *)e, E, nByteLen);
        reverse_byte_array((u8 *)E, (u8 *)e, nByteLen);
        tmpLen = (curve->eccp_n_bitLen)&7;
        if(tmpLen)
        {
            Big_Div2n(e, nWordLen, 8-tmpLen);
        }
        else
        {;}
    }

    //get e = e mod n, i.e., make sure e in [0, n-1]
    if(uint32_BigNumCmp(e, nWordLen, curve->eccp_n, nWordLen) >= 0)
    {
        ret = pke_sub(e, curve->eccp_n, e, nWordLen);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, curve->eccp_n_bitLen);
#else
    ret = pke_set_modulus_and_pre_monts(curve->eccp_n, curve->eccp_n_h, curve->eccp_n_bitLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
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
        return ret;
    }
    else
    {;}

    //tmp =  r*(s^(-1)) mod n
    ret = pke_modmul_internal(r, tmp, tmp, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //check public key
    e[pWordLen - 1] = 0;
    s[pWordLen - 1] = 0;
    reverse_byte_array(pubKey, (u8 *)e, pByteLen);
    reverse_byte_array(pubKey+pByteLen, (u8 *)s, pByteLen);
    ret = eccp_pointVerify(curve, e, s);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    ret = eccp_pointMul(curve, tmp, e, s, e, s);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(!uint32_BigNum_Check_Zero(x, nWordLen))
    {
        ret = eccp_pointMul(curve, x, curve->eccp_Gx, curve->eccp_Gy, x, tmp);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}

        ret = eccp_pointAdd(curve, e, s, x, tmp, e, s);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

    //x = x1 mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_mod(e, pWordLen, curve->eccp_n, curve->eccp_n_h, curve->eccp_n_n0, nWordLen, tmp);
#else
    ret = pke_mod(e, pWordLen, curve->eccp_n, curve->eccp_n_h, nWordLen, tmp);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(uint32_BigNumCmp(tmp, nWordLen, r, nWordLen))
    {
        return ECDSA_VERIFY_FAILED;
    }
    else
    {
        return ECDSA_SUCCESS;
    }
}

#endif

