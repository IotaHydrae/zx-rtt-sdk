//#include <stdio.h>

#include <aic_core.h>

#ifdef AIC_PKE_SM2_SUPPORT

#include <sm2.h>
#include <hash_kdf.h>
#include <trng.h>
#include <utility.h>

extern const u32 sm2p256v1_Gx[8];
extern const u32 sm2p256v1_Gy[8];
extern const u32 sm2p256v1_n[8];
extern const u32 sm2p256v1_n_h[8];
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
extern const u32 sm2p256v1_n_n0[1];
#endif
extern const u32 sm2p256v1_n_1[8];

extern const eccp_curve_t sm2_curve[1];

/* function: Generate SM2 Signature r and s with rand k
 * parameters:
 *     e[8]   --------------------- input, e value, 8 words, little-endian
 *     k[8]   --------------------- input, random number k, 8 words, little-endian
 *     dA[8]  --------------------- input, private key, 8 words, little-endian
 *     r[8]   --------------------- output, Signature r, 8 words, little-endian
 *     s[8]   --------------------- output, Signature s, 8 words, little-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. e and dA can not be modified
 *     2. e must be less than n(order of the SM2 curve)
 *     3. dA must be in [1, n-2]
 */
u32 sm2_sign_with_k(u32 e[8], u32 k[8], u32 dA[8], u32 r[8], u32 s[8])
{
    u32 tmp1[SM2_WORD_LEN], tmp2[SM2_WORD_LEN];
    u32 ret;

    if(NULL == e || NULL == k || NULL == dA || NULL == r || NULL == s)
    {
        return SM2_BUFFER_NULL;
    }
    else
    {;}

    //make sure k in [1, n-1]
    ret = uint32_integer_check(k, (u32 *)sm2p256v1_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#ifdef SM2_HIGH_SPEED
    ret = eccp_pointMul_base((eccp_curve_t *)sm2_curve, k, tmp1, NULL);
#else
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp1, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp1 = x1 mod n
    if(uint32_BigNumCmp(tmp1, SM2_WORD_LEN, (u32 *)sm2p256v1_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_sub(tmp1, (u32 *)sm2p256v1_n, tmp1, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

    //r = e + x1 mod n
    ret = pke_modadd((u32 *)sm2p256v1_n, e, tmp1, r, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //make sure r is not zero
    if(uint32_BigNum_Check_Zero(r, SM2_WORD_LEN))
    {
        return SM2_ZERO_ALL;
    }
    else
    {;}

    //tmp1 = r + k mod n
    ret = pke_modadd((u32 *)sm2p256v1_n, r, k, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else if(uint32_BigNum_Check_Zero(tmp1, SM2_WORD_LEN))   //make sure r+k is not n
    {
        return SM2_ZERO_ALL;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, (u32 *)sm2p256v1_n_n0, SM2_BIT_LEN);
#else
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, SM2_BIT_LEN);
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
    ret = pke_modmul_internal(r, dA, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp1 =  (k - r*dA) mod n
    ret = pke_modsub((u32 *)sm2p256v1_n, k, tmp1, tmp1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp2 = 1+dA
    uint32_copy(tmp2, dA, SM2_WORD_LEN);
    ret = uint32_big_num_little_endian_add_little(tmp2, SM2_WORD_LEN, 1, 1);
    if(ret)
    {
        return PKE_INTEGER_TOO_BIG;
    }
    else
    {;}

    //tmp2 = (1+dA)^(-1) mod n
    ret = pke_modinv((u32 *)sm2p256v1_n, tmp2, tmp2, SM2_WORD_LEN, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#if 0
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, (u32 *)sm2p256v1_n_n0, SM2_BIT_LEN);
#else
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, SM2_BIT_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}
#endif

    //s = ((1+dA)^(-1))*(k - r*dA) mod n
    ret = pke_modmul_internal(tmp1, tmp2, s, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //make sure s is not zero
    if(uint32_BigNum_Check_Zero(s, SM2_WORD_LEN))
    {
        return SM2_ZERO_ALL;
    }
    else
    {
        return SM2_SUCCESS;
    }
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
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 */
u32 sm2_sign(u8 E[32], u8 rand_k[32], u8 priKey[32], u8 signature[64])
{
    u32 e[SM2_WORD_LEN], k[SM2_WORD_LEN], dA[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN];
    u32 ret;

    if(NULL == E || NULL == priKey || NULL == signature)
    {
        return SM2_BUFFER_NULL;
    }
    else
    {;}

    //e = e mod n
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(E, e, SM2_WORD_LEN);
#else
    reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);
#endif
    if(uint32_BigNumCmp(e, SM2_WORD_LEN, (u32 *)sm2p256v1_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_sub(e, (u32 *)sm2p256v1_n, e, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

    //make sure priKey in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(priKey, dA, SM2_WORD_LEN);
#else
    reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check(dA, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(NULL == rand_k)
    {
        do {
            ret = get_rand((u8 *)k, SM2_BYTE_LEN);
            if(TRNG_SUCCESS != ret)
            {
                break;
            }
            else
            {;}

            ret = sm2_sign_with_k(e, k, dA, r, s);
        } while((SM2_ZERO_ALL == ret) || (SM2_INTEGER_TOO_BIG == ret));
    }
    else
    {
        reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);

        ret = sm2_sign_with_k(e, k, dA, r, s);
    }

    if(SM2_SUCCESS != ret)
    {
        return ret;
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

        return SM2_SUCCESS;
    }
}


/* function: Verify SM2 Signature
 * parameters:
 *     E[32] ---------------------- input, E value, 32 bytes, big-endian
 *     pubKey[65] ----------------- input, public key(0x04 + x + y), 65 bytes, big-endian
 *     signature[64] -------------- input, Signature r and s, 64 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success, the signature is valid); other(error or the signature is invalid)
 * caution:
 */
u32 sm2_verify(u8 E[32], u8 pubKey[65], u8 signature[64])
{
    u32 e[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN], tmp[SM2_WORD_LEN*4];
    u32 *t = e;
    u32 ret;

    if(NULL == E || NULL == pubKey || NULL == signature)
    {
        return SM2_BUFFER_NULL;
    }
    else if(POINT_UNCOMPRESSED != pubKey[0])    //make sure pubKey[0] is POINT_UNCOMPRESSED
    {
        return SM2_INPUT_INVALID;
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
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, (u32 *)(tmp+2*SM2_WORD_LEN), (u32 *)(tmp+3*SM2_WORD_LEN));
    if(PKE_SUCCESS != ret)
    {
        return SM2_NOT_ON_CURVE;
    }
    else
    {;}

    //make sure r in [1, n-1]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(signature, r, SM2_WORD_LEN);
#else
    reverse_byte_array(signature, (u8 *)r, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check(r, (u32 *)sm2p256v1_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG, 
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
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
    ret = uint32_integer_check(s, (u32 *)sm2p256v1_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG, 
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //t = (r+s) mod n
    ret = pke_modadd((u32 *)sm2p256v1_n, r, s, t, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //if t is 0, refuse the signature
    if(uint32_BigNum_Check_Zero(t, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

#ifdef SM2_HIGH_SPEED
    ret = eccp_pointMul_Shamir_safe((eccp_curve_t *)sm2_curve,
                                    s, (u32 *)sm2p256v1_Gx, (u32 *)sm2p256v1_Gy,
                                    t, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                                    tmp, NULL);
#else
    //[s]G
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, s, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //[t]PA
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, t, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN, tmp+2*SM2_WORD_LEN,
                        tmp+3*SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //[s]G + [t]PA
    ret = eccp_pointAdd((eccp_curve_t *)sm2_curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                        tmp, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //e = e mod n
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(E, e, SM2_WORD_LEN);
#else
    reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);
#endif
    if(uint32_BigNumCmp(e, SM2_WORD_LEN, (u32 *)sm2p256v1_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_sub(e, (u32 *)sm2p256v1_n, e, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            goto END;
        }
        else
        {;}
    }
    else
    {;}

    //tmp = x1 mod n
    if(uint32_BigNumCmp(tmp, SM2_WORD_LEN, (u32 *)sm2p256v1_n, SM2_WORD_LEN) >= 0)
    {
        ret = pke_sub(tmp, (u32 *)sm2p256v1_n, tmp, SM2_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            goto END;
        }
        else
        {;}
    }
    else
    {;}

    //tmp = e + x1 mod n
    ret = pke_modadd((u32 *)sm2p256v1_n, e, tmp, tmp, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //cmp
    if(uint32_BigNumCmp(tmp, SM2_WORD_LEN, r, SM2_WORD_LEN))
    {
        ret = SM2_VERIFY_FAILED;
        goto END;
    }
    else
    {;}

    //success
    ret = SM2_SUCCESS;

END:

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
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 *     2. please make sure pubkey_x and pubkey_y are valid
 */
u32 sm2_encrypt_with_k(u8 *M, u32 MByteLen, u32 *k,
                            u32 *pubkey_x, u32 *pubkey_y,
                            sm2_cipher_order_e order,
                            u8 *C, u32 *CByteLen)
{
    u8 counter[4] = {0,0,0,1};
    u32 xy[SM2_WORD_LEN<<1];
    u8 *C2, *C3;
    int32_t i;
    u32 ret;

    HASH_NODE hash_node[3];  //since M and C may point the same address, please do not initialize hash_node here.

    if(NULL == M || NULL == k || NULL == pubkey_x || NULL == pubkey_y || NULL == C || NULL == CByteLen)
    {
        return SM2_BUFFER_NULL;
    }
    else if(MByteLen == 0)
    {
        return SM2_INPUT_INVALID;
    }
    else if(order > SM2_C1C2C3)
    {
        return SM2_INPUT_INVALID;
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
            return SM2_INPUT_INVALID;
        }
        else
        {;}
    }
    else if(M < C)
    {
        if(M + MByteLen > C)
        {
            return SM2_INPUT_INVALID;
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
    ret = uint32_integer_check(k, (u32 *)sm2p256v1_n, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get [k]G
#ifdef SM2_HIGH_SPEED
    ret = eccp_pointMul_base((eccp_curve_t *)sm2_curve, k, xy, xy+SM2_WORD_LEN);
#else
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, xy, xy+SM2_WORD_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

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
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, k, pubkey_x, pubkey_y, xy, xy+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

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
        return ret;
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
        return ret;
    }
    else
    {
        *CByteLen = MByteLen+1+3*SM2_BYTE_LEN;

        return SM2_SUCCESS;
    }
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
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 *     2. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 *     3. please make sure pubKey is valid
 */
u32 sm2_encrypt(u8 *M, u32 MByteLen, u8 rand_k[32], u8 pubKey[65],
        sm2_cipher_order_e order, u8 *C, u32 *CByteLen)
{
    u32 k[SM2_WORD_LEN];
    u32 pubkey_x[SM2_WORD_LEN],pubkey_y[SM2_WORD_LEN];
    u32 ret;

    if(NULL == pubKey)
    {
        return SM2_BUFFER_NULL;
    }
    else if(POINT_UNCOMPRESSED != pubKey[0])
    {
        return SM2_INPUT_INVALID;
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
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, pubkey_x, pubkey_y);
    if(PKE_SUCCESS != ret)
    {
        return SM2_NOT_ON_CURVE;
    }
    else
    {;}

    if(NULL == rand_k)
    {
        do {
            ret = get_rand((u8 *)k, SM2_BYTE_LEN);
            if(TRNG_SUCCESS != ret)
            {
                break;
            }
            else
            {;}

            ret = sm2_encrypt_with_k(M, MByteLen, k, pubkey_x, pubkey_y, order, C, CByteLen);
        } while((SM2_ZERO_ALL == ret) || (SM2_INTEGER_TOO_BIG == ret));
    }
    else
    {
        reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);

        ret = sm2_encrypt_with_k(M, MByteLen, k, pubkey_x, pubkey_y, order, C, CByteLen);
    }

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
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can be the same buffer
 */
u32 sm2_decrypt(u8 *C, u32 CByteLen, u8 priKey[32],
        sm2_cipher_order_e order, u8 *M, u32 *MByteLen)
{
    u8 counter[4] = {0,0,0,1};
    u32 i, temLen;
    u32 dA[SM2_WORD_LEN], xy[SM2_WORD_LEN<<1];
    u8 digest[SM2_BYTE_LEN];
    u8 C3_buf[SM2_BYTE_LEN];
    u8 *C2, *C3;
    u32 ret;

    HASH_NODE hash_node[3] = {
        {(u8 *)xy, SM2_BYTE_LEN<<1},
        {counter, 4},
        {(u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN},
    };

    if(NULL == C || NULL == priKey || NULL == M || NULL == MByteLen)
    {
        return SM2_BUFFER_NULL;
    }
    else if(CByteLen <= 1+3*SM2_BYTE_LEN)                                        //97 = 1+3*ECCP_BYTELEN
    {
        return SM2_INPUT_INVALID;
    }
    else if(order > SM2_C1C2C3)
    {
        return SM2_INPUT_INVALID;
    }
    else
    {;}

    temLen = CByteLen-1-(3*SM2_BYTE_LEN);

    C2 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?0:SM2_BYTE_LEN);
    C3 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?temLen:0);

    //not support M and C crossing, but support M = C
    if(M > C)
    {
        if(C + CByteLen > M)
        {
            return SM2_INPUT_INVALID;
        }
        else
        {;}
    }
    else if(M < C)
    {
        if(M + temLen > C)
        {
            return SM2_INPUT_INVALID;
        }
        else
        {;}
    }
    else  //M = C
    {;}

    //make sure C1 is on the SM2 curve
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(C+1, xy, SM2_WORD_LEN);
    reverse_word_array(C+1+SM2_BYTE_LEN, xy+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(C+1, (u8 *)xy, SM2_BYTE_LEN);
    reverse_byte_array(C+1+SM2_BYTE_LEN, (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, xy, xy+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return SM2_NOT_ON_CURVE;
    }
    else
    {;}

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

    //make sure priKey in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(priKey, dA, SM2_WORD_LEN);
#else
    reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check(dA, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //[dA]C1
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, dA, xy, xy+SM2_WORD_LEN, xy, xy+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)xy, xy, SM2_WORD_LEN);
    reverse_word_array((u8 *)(xy+SM2_WORD_LEN), xy+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)xy, (u8 *)xy, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif

    ret = ansi_x9_63_kdf_node_with_xor_in(HASH_SM3, hash_node, 2, 1, C2, M, temLen, 1);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //hash_node[0].msg_addr  = (u8 *)xy;
    hash_node[0].msg_bytes = SM2_BYTE_LEN;
    hash_node[1].msg_addr  = (u8 *)M;
    hash_node[1].msg_bytes = temLen;
    //hash_node[2].msg_addr  = (u8 *)(xy+SM2_WORD_LEN);
    //hash_node[2].msg_bytes = SM2_BYTE_LEN;
    ret = hash_node_steps(HASH_SM3, hash_node, 3, digest);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(memcmp_(C3, digest, SM2_BYTE_LEN))
    {
        return SM2_DECRYPT_VERIFY_FAILED;
    }
    else
    {
        *MByteLen = temLen;
        return SM2_SUCCESS;
    }
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
 *     SM2_SUCCESS(success); other(error)
 * caution: 
 *     1. please make sure the inputs are valid
 *     2. S1 and SA are optional, if you don't need, please set S1 and SA as NULL
 *     3. in case that S1(S2) and SA(SB) exist, if S1=SB,S2=SA, then exchange success.
 */
u32 sm2_exchangekey(sm2_exchange_role_e role,
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
    u32 ret;

    if(NULL == dA || NULL == PB || NULL == rA || NULL == RA || NULL == RB)
    {
        return SM2_BUFFER_NULL;
    }
    else if(NULL == ZA || NULL == ZB || NULL == KA)
    {
        return SM2_BUFFER_NULL;
    }
    else if(role > SM2_Role_Responsor)
    {
        return SM2_EXCHANGE_ROLE_INVALID;
    }
    else if(0 == kByteLen)
    {
        return SM2_INPUT_INVALID;
    }
    else if((POINT_UNCOMPRESSED != PB[0]) || (POINT_UNCOMPRESSED != RA[0]) || (POINT_UNCOMPRESSED != RB[0]))
    {
        return SM2_INPUT_INVALID;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(RA+1, x1, SM2_WORD_LEN);
    reverse_word_array(RA+1+SM2_BYTE_LEN, t1, SM2_WORD_LEN);
#else
    reverse_byte_array(RA+1, (u8 *)x1, SM2_BYTE_LEN);
    reverse_byte_array(RA+1+SM2_BYTE_LEN, (u8 *)t1, SM2_BYTE_LEN);
#endif
    if(PKE_SUCCESS != eccp_pointVerify((eccp_curve_t *)sm2_curve, x1, t1))
    {
        ret = SM2_NOT_ON_CURVE;
        goto END;
    }
    else
    {;}

    //get x1
    uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);
    x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

    //make sure rA in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(rA, t1, SM2_WORD_LEN);
#else
    reverse_byte_array(rA, (u8 *)t1, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check(t1, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, (u32 *)sm2p256v1_n_n0, SM2_BIT_LEN);
#else
    ret = pke_load_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, SM2_BIT_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //t1 = x1*rA mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(x1, t1, t1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //make sure dA in [1, n-2]
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(dA, x1, SM2_WORD_LEN);
#else
    reverse_byte_array(dA, (u8 *)x1, SM2_BYTE_LEN);
#endif
    ret = uint32_integer_check(x1, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN, SM2_ZERO_ALL, SM2_INTEGER_TOO_BIG,
            SM2_SUCCESS);
    if(SM2_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //t1 = (dA + x1*rA) mod n, and it must not be 0
    ret = pke_modadd((u32 *)sm2p256v1_n, t1, x1, t1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else if(uint32_BigNum_Check_Zero(t1, SM2_WORD_LEN))
    {
        ret = SM2_ZERO_ALL;
        goto END;
    }
    else
    {;}

    //make sure RB on the SM2 curve
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(RB+1, tmp, SM2_WORD_LEN);
    reverse_word_array(RB+1+SM2_BYTE_LEN, tmp+SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(RB+1, (u8 *)tmp, SM2_BYTE_LEN);
    reverse_byte_array(RB+1+SM2_BYTE_LEN, (u8 *)(tmp+SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return SM2_NOT_ON_CURVE;
    }
    else
    {;}

    uint32_copy(x1, tmp, SM2_WORD_LEN/2);
    uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);
    x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

#ifdef SM2_HIGH_SPEED
    ret = pke_set_modulus_and_pre_monts((u32 *)sm2p256v1_n, (u32 *)sm2p256v1_n_h, SM2_BIT_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //x1 = tA*x2 mod n
    ret = pke_modmul_internal(t1, x1, x1, SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //get PB point and verify
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(PB+1, tmp+2*SM2_WORD_LEN, SM2_WORD_LEN);
    reverse_word_array(PB+1+SM2_BYTE_LEN, tmp+3*SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(PB+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
    reverse_byte_array(PB+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //[tA]PB +[tA*x2 mod n]RB
    ret = eccp_pointMul_Shamir_safe((eccp_curve_t *)sm2_curve,
                                    t1, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                                    x1, tmp, tmp+SM2_WORD_LEN,
                                    tmp, tmp+SM2_WORD_LEN);
#else
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, x1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //get PB point(caution: do not delete this)
#ifdef PKE_BIG_ENDIAN
    reverse_word_array(PB+1, tmp+2*SM2_WORD_LEN, SM2_WORD_LEN);
    reverse_word_array(PB+1+SM2_BYTE_LEN, tmp+3*SM2_WORD_LEN, SM2_WORD_LEN);
#else
    reverse_byte_array(PB+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
    reverse_byte_array(PB+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
#endif
    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = eccp_pointAdd((eccp_curve_t *)sm2_curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
                        tmp, tmp+SM2_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, t1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

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
        goto END;
    }
    else
    {;}

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
            goto END;
        }
        else
        {;}
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}

#endif

