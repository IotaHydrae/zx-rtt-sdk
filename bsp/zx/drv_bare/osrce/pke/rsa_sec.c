//#include <stdio.h>

#include "rsa.h"

#if defined(AIC_PKE_SEC) && defined(RSA_SEC)

#include <utility_sec.h>
#include <trng.h>

#ifdef AIC_PKE_SEC
#define RSA_SEC_API_OPTIMIZATION
#endif

/* function: out = a^d mod n
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n
 *     e -------------------------- input, u32 big integer e, public key, it is less than 2^64.
 *     d -------------------------- input, u32 big integer d, private key
 *     n -------------------------- input, u32 big integer n, modulus, make sure n is odd
 *     out ------------------------ output, out = a^d mod n
 *     eBitLen  ------------------- input, real bit length of u32 big integer e, please make sure eBitLen <= 64.
 *     nBitLen  ------------------- input, real bit length of u32 big integer n
 * return: RSA_SUCCESS_S(success), other(error)
 * caution:
 *     1. a, n, and out have the same word length:((nBitLen+31)>>5); and e word length is (eBitLen+31)>>5
 *     2. please make sure 2 <= eBitLen <= 64.
 *     3. a and out can not point to the same buffer.
 */
u32 RSA_ModExp_with_pub(u32 *a, u32 *e, u32 *d, u32 *n, u32 *out, u32 eBitLen, u32 nBitLen)
{
    u32 r[RSA_MAX_WORD_LEN];
    u32 cx[RSA_MAX_WORD_LEN];
    u32 eWordLen = GET_WORD_LEN(eBitLen);
    u32 nWordLen = GET_WORD_LEN(nBitLen);
    u32 dBitLen = get_valid_bits(d, nWordLen);
    u32 dWordLen = GET_WORD_LEN(dBitLen);
    u32 ret;

    if(NULL == a || NULL == e|| NULL == d || NULL == n || NULL == out)
    {
        return RSA_ERROR_S;
    }
    else if(nBitLen > RSA_MAX_BIT_LEN || nBitLen < RSA_MIN_BIT_LEN || (nBitLen&1) || dBitLen < 2 || dBitLen > nBitLen
            || eBitLen < 2 || eBitLen > 64 || (eBitLen > nBitLen))
    {
        return RSA_ERROR_S;
    }
    else if(!(n[0] & 1))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    if(a == out)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    ret = pke_modexp_check_input((const u32 *)n, (const u32 *)d, (const u32 *)a,
            out, nWordLen, nWordLen);
    if(PKE_FINISHED == ret)
    {
        return RSA_SUCCESS_S;
    }
    else if(PKE_SUCCESS != ret)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(n, nBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(n, nBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    //get r and r^(-1) mod n
GET_RAND_R:
    ret = uint32_get_rand_big_number_msb_0(r, nBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_modinv(n, r, cx, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        goto GET_RAND_R;
    }
    else
    {;}

    //cx = (r^(-1))^e mod n
    ret = pke_modexp(n, e, cx, cx, nWordLen, eWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = a*(r^(-1))^e mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(a, cx, cx, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_modexp_with_pub(n, d, e, cx, cx, nWordLen, dWordLen, eWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //get output
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(r, cx, out, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //check output three or more times
    ret = pke_modexp(n, e, out, cx, nWordLen, eWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    (void)get_rand_fast((u8 *)r, 8);

    if(0 != uint32_cmp_sec(cx, a, nWordLen, r[0]&0xFF))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (r[0]>>8)&0xFF);

    if(0 != uint32_cmp_sec(cx, a, nWordLen, (r[0]>>16)&0xFF))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (r[0]>>24)&0xFF);

    if(0 != uint32_cmp_sec(cx, a, nWordLen, r[1]&0xFF))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (r[1]>>8)&0xFF);

    ret = RSA_SUCCESS_S;

END:

    if(RSA_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)out, nWordLen << 2);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)r,  nWordLen << 2);
    (void)get_rand_fast((u8 *)cx, nWordLen << 2);

    return ret;
}


/* function: out = a^d mod n
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n
 *     d -------------------------- input, u32 big integer d, private key
 *     n -------------------------- input, u32 big integer n, modulus, make sure n is odd
 *     out ------------------------ output, out = a^d mod n
 *     nBitLen  ------------------- input, real bit length of u32 big integer n
 * return: RSA_SUCCESS_S(success), other(error)
 * caution:
 *     1. a, n, and out have the same word length:((nBitLen+31)>>5)
 *     2. a and out can point to the same buffer.
 */
u32 RSA_ModExp_without_pub(u32 *a, u32 *d, u32 *n, u32 *out, u32 nBitLen)
{
    u32 nWordLen = GET_WORD_LEN(nBitLen);
    u32 dBitLen = get_valid_bits(d, nWordLen);
    u32 dWordLen = GET_WORD_LEN(dBitLen);
    volatile u16 crc1, crc2;
    u16 tmp_step, crc3;
    s32 flag;
    u32 ret;

    if(NULL == a || NULL == d || NULL == n || NULL == out)
    {
        return RSA_ERROR_S;
    }
    else if(nBitLen > RSA_MAX_BIT_LEN || nBitLen < RSA_MIN_BIT_LEN || (nBitLen&1) || dBitLen < 2 || dBitLen > nBitLen)
    {
        return RSA_ERROR_S;
    }
    else if(!(n[0] & 1))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    //a should be in [0,n]
    flag = uint32_BigNumCmp(a, nWordLen, n, nWordLen);
    if(flag > 0)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    if((0 == flag) || uint32_BigNum_Check_Zero(a, nWordLen))      //if a is 0 or n
    {
        if(uint32_BigNum_Check_Zero_sec(d, dWordLen))  //0^0 mod n
        {
            return RSA_ERROR_S;
        }
        else                                           //if a is 0, d is not 0, the output is 0
        {
            uint32_clear(out, nWordLen);
            return RSA_SUCCESS_S;
        }
    }
    else if(uint32_BigNum_Check_Zero_sec(d, dWordLen))            //a is in [1,n-1], d is 0, the output is 1
    {
        uint32_clear(out, nWordLen);
        *out = 1;
        return RSA_SUCCESS_S;
    }
    else
    {;}

    ret = pke_pre_calc_mont(n, nBitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    //crc1 = crc(d||n) = crc((d XOR rand)||n) XOR crc(rand||00...00)
    (void)get_rand_fast((u8 *)(PKE_A(4,tmp_step)), dWordLen << 2);
    uint32_XOR((u32 *)(PKE_A(4,tmp_step)), d, (u32 *)(PKE_A(0,tmp_step)), dWordLen);
    crc1 = crc16_calc((u8 *)(PKE_A(0,tmp_step)), dWordLen<<2, 0);
    crc1 = crc16_calc((u8 *)n, nWordLen<<2, crc1);

    crc3 = crc16_calc((u8 *)(PKE_A(4,tmp_step)), dWordLen<<2, 0);
    uint32_clear((u32 *)(PKE_A(4,tmp_step)), nWordLen);
    crc3 = crc16_calc((u8 *)(PKE_A(4,tmp_step)), nWordLen<<2, crc3);
    crc1 ^= crc3;

    ret = pke_modexp_without_pub(n, d, a, out, nWordLen, dWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //now A1 holds d1, B1 holds d2, here recover A1 = d1+d2 = d
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), (u32 *)(PKE_A(1,tmp_step)), dWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //crc2 = crc(d||n) = crc((d XOR rand)||n) XOR crc(rand||00...00)
    (void)get_rand_fast((u8 *)(PKE_A(4,tmp_step)), dWordLen << 2);
    uint32_XOR((u32 *)(PKE_A(4,tmp_step)), (u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), dWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,tmp_step)), dWordLen<<2, 0);
    crc2 = crc16_calc((u8 *)n, nWordLen<<2, crc2);

    crc3 = crc16_calc((u8 *)(PKE_A(4,tmp_step)), dWordLen<<2, 0);
    uint32_clear((u32 *)(PKE_A(4,tmp_step)), nWordLen);
    crc3 = crc16_calc((u8 *)(PKE_A(4,tmp_step)), nWordLen<<2, crc3);
    crc2 ^= crc3;

    //check crc three or more times
    (void)get_rand_fast((u8 *)&ret, 4);

    if(0 != (crc1 - crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret)&0xFF);

    if(0 != (crc1 ^ crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret>>8)&0xFF);

    if((crc1 & crc2) != (crc1 | crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret>>16)&0xFF);

    ret = RSA_SUCCESS_S;

END:

    if(RSA_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)out, nWordLen << 2);
    }
    else
    {;}

    return ret;
}


/* function: out = a^d mod n, here d represents RSA CRT private key (p,q,dp,dq,u)
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n=pq
 *     p -------------------------- input, u32 big integer p, prime number, one part of private key (p,q,dp,dq,u)
 *     q -------------------------- input, u32 big integer q, prime number, one part of private key (p,q,dp,dq,u)
 *     dp ------------------------- input, u32 big integer dp = e^(-1) mod (p-1), one part of private key (p,q,dp,dq,u)
 *     dq ------------------------- input, u32 big integer dq = e^(-1) mod (q-1), one part of private key (p,q,dp,dq,u)
 *     u -------------------------- input, u32 big integer u = q^(-1) mod p, one part of private key (p,q,dp,dq,u)
 *     e -------------------------- input, u32 big integer e, public key, it is less than 2^64.
 *     out ------------------------ output, out = a^d mod n
 *     eBitLen  ------------------- input, real bit length of u32 big integer e, please make sure 2 <= eBitLen <= 64.
 *     nBitLen  ------------------- input, real bit length of u32 big integer n=pq
 * return: RSA_SUCCESS_S(success), other(error)
 * caution:
 *     1. a and out have the same word length:((nBitLen+31)>>5); and p,p_h,q,q_h,dp,dq,u
 *        have the same word length:((nBitLen/2+31)>>5)
 *     2. please make sure 2 <= eBitLen <= 64.
 *     3. a and out can not point to the same buffer
 */
u32 RSA_CRTModExp_with_pub(u32 *a, u32 *p, u32 *q, u32 *dp, u32*dq, u32 *u, u32 *e,
        u32 *out, u32 eBitLen, u32 nBitLen)
{
    //caution: y and k both occupy RSA_MAX_WORD_LEN/2 words, but tmp_buf is also used for mod tmpp operations,
    //it need RSA_MAX_WORD_LEN/2 + 2 words, to keep k, so tmp_buf needs RSA_MAX_WORD_LEN + 2 words.
    u32 tmp_buf[RSA_MAX_WORD_LEN + 2];
    u32 *tmp_n = out;                               //u32 tmp_n[RSA_MAX_WORD_LEN+4];
    u32 *y = tmp_buf;                               //u32 k[RSA_MAX_WORD_LEN/2];
    u32 *k = tmp_buf + RSA_MAX_WORD_LEN/2 + 2;      //u32 y[RSA_MAX_WORD_LEN/2];
    u32 h1[RSA_MAX_WORD_LEN/2];
//    u32 h2[RSA_MAX_WORD_LEN/2];
//    u32 p1[RSA_MAX_WORD_LEN/2]={0};
    u32 dp1[RSA_MAX_WORD_LEN/2 + 2];
    u32 tmpp[RSA_MAX_WORD_LEN/2 + 2];
    u32 tmpq[RSA_MAX_WORD_LEN/2 + 2];
    u32 r[RSA_MAX_WORD_LEN/2+RSA_MAX_WORD_LEN];
    u32 *cx = r+RSA_MAX_WORD_LEN/2;         //cx[RSA_MAX_WORD_LEN]={0};

    u32 *m1 = tmpq;                         //m1[RSA_MAX_WORD_LEN/2 + 2];
    u32 m2[RSA_MAX_WORD_LEN/2 + 2];

    u32 eWordLen;
    u32 nWordLen;
    u32 pWordLen;
    u32 uWordLen;

    u32 tempp_WordLen;
    u32 tempq_WordLen;
    u32 temp_WordLen;

    u32 pBitLen;
    u32 temp_bitLen;

    u32 p_step, n_step, tmp_step;
    s32 flag;
    u32 ret;

    if(NULL == a || NULL == p || NULL == q || NULL == dp || NULL == dq || NULL == u || NULL == e || NULL == out)
    {
        return RSA_ERROR_S;
    }
    else if(nBitLen > RSA_MAX_BIT_LEN || nBitLen < RSA_MIN_BIT_LEN || (nBitLen&1) || eBitLen < 2 || eBitLen > 64
            || (eBitLen > nBitLen))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    if(a == out)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    eWordLen = GET_WORD_LEN(eBitLen);
    nWordLen = GET_WORD_LEN(nBitLen);
    pBitLen = nBitLen/2;
    pWordLen = GET_WORD_LEN(pBitLen);
    temp_WordLen = pWordLen + 2;

    //get tmp_n = p*q
    ret = pke_mul_internal(p, q, tmp_n, pWordLen, pWordLen, nWordLen);
    if((PKE_SUCCESS != ret) || (0 == (tmp_n[0] & 1)))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    //a should be in [0,n]
    flag = uint32_BigNumCmp(a, nWordLen, tmp_n, nWordLen);
    if(flag > 0)
    {
        return RSA_ERROR_S;
    }
    else if((0 == flag) || (1 == uint32_BigNum_Check_Zero(a, nWordLen)))
    {
        //if a is 0 or n, the output is 0
        uint32_clear(out, nWordLen);
        return RSA_SUCCESS_S;
    }
    else
    {;}

    //get hardware step
    n_step = pke_set_operand_width(nBitLen);
    p_step = pke_set_operand_width(pBitLen);

    /************* get 64bit random k, y, and make sure k < y, and y is odd    *************/
GET_KY_P:
    ret = get_rand((u8 *)(k+2), 8);
    ret |= get_rand((u8 *)(y+2), 8);
    if(TRNG_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    flag = uint32_BigNumCmp_sec(k+2, 2, y+2, 2);
    if(0 == flag)
    {
        goto GET_KY_P;
    }
    else if(1 == flag)
    {
        uint32_copy(y, k+2, 2);
        uint32_copy(k, y+2, 2);
    }
    else
    {
        uint32_copy(k, k+2, 2);
        uint32_copy(y, y+2, 2);
    }

    y[0] |= 0x01;

    uint32_clear(k+2, pWordLen-2);
    uint32_clear(y+2, pWordLen-2);

    /************************* get r and cx = a*(r^(-1))^e mod n *************************/
    //get r and r^(-1) mod n
GET_RAND_R:
    ret = get_rand((u8 *)r, pWordLen << 2);
    if(TRNG_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modinv(tmp_n, r, cx, nWordLen, pWordLen);
#else
    ret = pke_modinv(tmp_n, r, (u32 *)(PKE_B(0,n_step)), nWordLen, pWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        goto GET_RAND_R;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_pre_calc_mont(tmp_n, nBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont((u32 *)(PKE_B(3,n_step)), nBitLen, NULL, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = (r^(-1))^e mod n
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp(tmp_n, e, cx, cx, nWordLen, eWordLen);
#else
    ret = pke_modexp((u32 *)(PKE_B(3,n_step)), e, (u32 *)(PKE_B(0,n_step)), (u32 *)(PKE_A(0,n_step)), nWordLen, eWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = a*(r^(-1))^e mod n
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul_internal(cx, a, cx, nWordLen);
#else
    ret = pke_modmul_internal((u32 *)(PKE_A(0,n_step)), a, cx, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* dp1 = dp + k*h1 + k*h2 = dp + k(p-1) *************************/
    //get random big number h1 < p (and h1 < p-1)
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, k, dp1, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    //h1 = (p-1) - h1
#if 0
    p[0] -= 1;
    (void)pke_sub(p, h1, h1, pWordLen);
    p[0] |= 1;
#else
    uint32_copy((u32 *)(PKE_A(1,p_step)), p, pWordLen);
    *(u32 *)(PKE_A(1,p_step)) -= 1;
    ret = pke_sub((u32 *)(PKE_A(1,p_step)), h1, h1, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
#endif

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(h1, k, tmpp, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(h1, k, (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmpp, dp1, tmpp, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), dp1, (u32 *)(PKE_A(1,tmp_step)), temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    uint32_copy(dp1, dp, pWordLen);
    uint32_clear(dp1+pWordLen, 2);
    ret = pke_add(tmpp, dp1, dp1, temp_WordLen);
#else
    uint32_copy((u32 *)(PKE_B(1,tmp_step)), dp, pWordLen);
    uint32_clear((u32 *)(PKE_B(1,tmp_step))+pWordLen, 2);
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), dp1, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* tmpp = p1*y + p2*y = py ***********************/
    //dp + k(p-1) < (k+1)(p-1) <= y(p-1) < py
    //get random big number h1 < p
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, y, tmpp, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_sub(p, h1, h1, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(h1, y, tmp_buf, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(h1, y, (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmp_buf, tmpp, tmpp, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), tmpp, tmpp, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get m2 ***********************/
    temp_bitLen = get_valid_bits(tmpp, temp_WordLen);
    tempp_WordLen = GET_WORD_LEN(temp_bitLen);
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(tmpp, temp_bitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(tmpp, temp_bitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = cx mod tmpp
    tmp_step = pke_get_operand_bytes();
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(cx, nWordLen, tmpp, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempp_WordLen, tmp_buf);
#else
    ret = pke_mod(cx, nWordLen, tmpp, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempp_WordLen, (u32 *)(PKE_B(0,tmp_step)));
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf^dp1 mod tmpp
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp_ladder(tmpp, dp1, tmp_buf, dp1, tempp_WordLen, tempp_WordLen);  //dp1 < tmpp
#else
    ret = pke_modexp_ladder((u32 *)(PKE_B(3,tmp_step)), dp1, (u32 *)(PKE_B(0,tmp_step)), dp1, tempp_WordLen, tempp_WordLen);  //dp1 < tmpp
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(p, pBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(p, pBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //m2 = dp1 mod p
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, m2);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************* get 64bit random k, y, and make sure k < y, and y is odd *************/
GET_KY_Q:
    ret = get_rand((u8 *)(k+2), 8);
    ret |= get_rand((u8 *)(y+2), 8);
    if(TRNG_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    flag = uint32_BigNumCmp_sec(k+2, 2, y+2, 2);
    if(0 == flag)
    {
        goto GET_KY_Q;
    }
    else if(1 == flag)
    {
        uint32_copy(y, k+2, 2);
        uint32_copy(k, y+2, 2);
    }
    else
    {
        uint32_copy(k, k+2, 2);
        uint32_copy(y, y+2, 2);
    }

    y[0] |= 0x01;

    uint32_clear(k+2, pWordLen-2);
    uint32_clear(y+2, pWordLen-2);

    /************************* dq1 = dq + k*h1 + k*h2 = dq + k(q-1) *************************/
    //get random big number h1 < q (and h1 < q-1)
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, k, dp1, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    //h1 = (q-1) - h1
#if 0
    q[0] -= 1;
    (void)pke_sub(q, h1, h1, pWordLen);
    q[0] |= 1;
#else
    uint32_copy((u32 *)(PKE_A(1,p_step)), q, pWordLen);
    *(u32 *)(PKE_A(1,p_step)) -= 1;
    ret = pke_sub((u32 *)(PKE_A(1,p_step)), h1, h1, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
#endif

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(h1, k, tmpq, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(h1, k, (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmpq, dp1, tmpq, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), dp1, (u32 *)(PKE_A(1,tmp_step)), temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    uint32_copy(dp1, dq, pWordLen);
    uint32_clear(dp1+pWordLen, 2);
    ret = pke_add(tmpq, dp1, dp1, temp_WordLen);
#else
    uint32_copy((u32 *)(PKE_B(1,tmp_step)), dq, pWordLen);
    uint32_clear((u32 *)(PKE_B(1,tmp_step))+pWordLen, 2);
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), dp1, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* tmpq = q1*y + q2*y = qy ***********************/
    //get random big number h1 < q
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_sub(q, h1, k, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, y, tmpq, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(k, y, tmp_buf, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(k, (u32 *)(PKE_B(0,tmp_step)), (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmp_buf, tmpq, tmpq, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), tmpq, tmpq, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get m1 ***********************/
    temp_bitLen = get_valid_bits(tmpq, temp_WordLen);
    tempq_WordLen = GET_WORD_LEN(temp_bitLen);
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(tmpq, temp_bitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(tmpq, temp_bitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = cx mod tmpq
    tmp_step = pke_get_operand_bytes();
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(cx, nWordLen, tmpq, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempq_WordLen, tmp_buf);
#else
    ret = pke_mod(cx, nWordLen, tmpq, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempq_WordLen, (u32 *)(PKE_B(0,tmp_step)));
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf^dp1 mod tmpq
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp_ladder(tmpq, dp1, tmp_buf, dp1, tempq_WordLen, tempq_WordLen); //dp1 < tempq
#else
    ret = pke_modexp_ladder((u32 *)(PKE_B(3,tmp_step)), dp1, (u32 *)(PKE_B(0,tmp_step)), dp1, tempq_WordLen, tempq_WordLen); //dp1 < tempq
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(q, pBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(q, pBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //m1 = dp1 mod q
    ret = pke_mod(dp1, tempq_WordLen, q, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, m1);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get mx ***********************/
    //get random big number dp1 < u
    temp_bitLen = get_valid_bits(u, pWordLen);
    uWordLen = GET_WORD_LEN(temp_bitLen);
    ret = uint32_get_rand_big_number_msb_0(dp1, temp_bitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //u = dp1 + cx
    uint32_clear(dp1 + uWordLen, tempp_WordLen - uWordLen);
    uint32_clear(cx + uWordLen, tempp_WordLen - uWordLen);
    ret = pke_sub(u, dp1, cx, uWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = (m2 - m1) mod tmpp
    uint32_clear(m1 + pWordLen, tempp_WordLen - pWordLen);
    uint32_clear(m2 + pWordLen, tempp_WordLen - pWordLen);
    ret = pke_modsub(tmpp, m2, m1, tmp_buf, tempp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    //dp1 = tmp_buf*u1 mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul(tmpp, dp1, tmp_buf, dp1, tempp_WordLen);
#else
    ret = pke_modmul((u32 *)(PKE_B(3,tmp_step)), dp1, tmp_buf, dp1, tempp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = tmp_buf*u2 mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul_internal(cx, tmp_buf, cx, tempp_WordLen);
#else
    ret = pke_modmul_internal(cx, tmp_buf, (u32 *)(PKE_A(0,tmp_step)), tempp_WordLen);//B0 corrupted after modmul
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf*u mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modadd(tmpp, cx, dp1, dp1, tempp_WordLen);
#else
    ret = pke_modadd((u32 *)(PKE_B(3,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), dp1, dp1, tempp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(p, pBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(p, pBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = dp1 mod p
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, cx);
#else
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, (u32 *)(PKE_B(0,n_step)));
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(k, cx, tmp_buf, pWordLen, pWordLen, nWordLen);
#else
    ret = pke_mul_internal(k, (u32 *)(PKE_B(0,n_step)), tmp_buf, pWordLen, pWordLen, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(h1, cx, cx, pWordLen, pWordLen, nWordLen);
#else
    ret = pke_mul_internal(h1, (u32 *)(PKE_B(0,n_step)), (u32 *)(PKE_A(1,n_step)), pWordLen, pWordLen, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(cx, tmp_buf, tmp_buf, nWordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,n_step)), tmp_buf, (u32 *)(PKE_A(1,n_step)), nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    uint32_copy(cx, m1, pWordLen);
    uint32_clear(cx + pWordLen, nWordLen - pWordLen);
    ret = pke_add(tmp_buf, cx, tmp_buf, nWordLen);
#else
    uint32_copy((u32 *)(PKE_B(1,n_step)), m1, pWordLen);
    uint32_clear((u32 *)(PKE_B(1,n_step))+pWordLen, nWordLen - pWordLen);
    ret = pke_add((u32 *)(PKE_A(1,n_step)), (u32 *)(PKE_B(1,n_step)), (u32 *)(PKE_B(0,n_step)), nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get output tmp_buf ***********************/
    uint32_clear(r + pWordLen, nWordLen - pWordLen);
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul(tmp_n, r, tmp_buf, tmp_buf, nWordLen);
#else
    ret = pke_modmul(tmp_n, r, (u32 *)(PKE_B(0,n_step)), tmp_buf, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* check output three or more times ***********************/
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp(tmp_n, e, tmp_buf, cx, nWordLen, eWordLen);
#else
    ret = pke_modexp((u32 *)(PKE_B(3,n_step)), e, tmp_buf, cx, nWordLen, eWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    (void)get_rand_fast((u8 *)r, 8);

    if(0 != uint32_cmp_sec(cx, a, nWordLen, (u8)r[0]))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (u8)(r[0]>>8));

    if(0 != uint32_cmp_sec(cx, a, nWordLen, (u8)(r[0]>>16)))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (u8)(r[0]>>24));

    if(0 != uint32_cmp_sec(cx, a, nWordLen, (u8)r[1]))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (u8)(r[1]>>8));

    /************************* output ***********************/
    uint32_copy(out, tmp_buf, nWordLen);

    ret = RSA_SUCCESS_S;

END:

    (void)get_rand_fast((u8 *)y,    (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)k,    (pWordLen)<<2);
    (void)get_rand_fast((u8 *)h1,   (pWordLen)<<2);
    (void)get_rand_fast((u8 *)dp1,  (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)tmpp, (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)tmpq, (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)r,    (pWordLen)<<2);
    (void)get_rand_fast((u8 *)cx,   (nWordLen)<<2);
    (void)get_rand_fast((u8 *)m2,   (pWordLen+2)<<2);

    return ret;
}



/* function: out = a^d mod n, here d represents RSA CRT private key (p,q,dp,dq,u)
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n=pq
 *     p -------------------------- input, u32 big integer p, prime number, one part of private key (p,q,dp,dq,u)
 *     q -------------------------- input, u32 big integer q, prime number, one part of private key (p,q,dp,dq,u)
 *     dp ------------------------- input, u32 big integer dp = e^(-1) mod (p-1), one part of private key (p,q,dp,dq,u)
 *     dq ------------------------- input, u32 big integer dq = e^(-1) mod (q-1), one part of private key (p,q,dp,dq,u)
 *     u -------------------------- input, u32 big integer u = q^(-1) mod p, one part of private key (p,q,dp,dq,u)
 *     out ------------------------ output, out = a^d mod n
 *     nBitLen  ------------------- input, real bit length of u32 big integer n=pq
 * return: RSA_SUCCESS_S(success), other(error)
 * caution:
 *     1. a and out have the same word length:((nBitLen+31)>>5); and p,p_h,q,q_h,dp,dq,u
 *        have the same word length:((nBitLen/2+31)>>5)
 *     2. a and out can not point to the same buffer
 */
u32 RSA_CRTModExp_without_pub(u32 *a, u32 *p, u32 *q, u32 *dp, u32*dq, u32 *u,
        u32 *out, u32 nBitLen)
{
    //caution: y and k both occupy RSA_MAX_WORD_LEN/2 words, but tmp_buf is also used for mod tmpp operations,
    //it need RSA_MAX_WORD_LEN/2 + 2 words, to keep k, so tmp_buf needs RSA_MAX_WORD_LEN + 2 words.
    u32 tmp_buf[RSA_MAX_WORD_LEN + 2];
    u32 *tmp_n = out;                               //u32 tmp_n[RSA_MAX_WORD_LEN+4];
    u32 *y = tmp_buf;                               //u32 k[RSA_MAX_WORD_LEN/2];
    u32 *k = tmp_buf + RSA_MAX_WORD_LEN/2 + 2;      //u32 y[RSA_MAX_WORD_LEN/2];
    u32 h1[RSA_MAX_WORD_LEN/2];
//    u32 h2[RSA_MAX_WORD_LEN/2];
//    u32 p1[RSA_MAX_WORD_LEN/2]={0};
    u32 dp1[RSA_MAX_WORD_LEN/2 + 2];
    u32 tmpp[RSA_MAX_WORD_LEN/2 + 2];
    u32 tmpq[RSA_MAX_WORD_LEN/2 + 2];
//    u32 r[RSA_MAX_WORD_LEN/2+MAX_RSA_WORD_LEN];
    u32 cx[RSA_MAX_WORD_LEN]={0};  //u32 *cx = r+RSA_MAX_WORD_LEN/2;         //

    u32 *m1 = tmpq;                         //m1[RSA_MAX_WORD_LEN/2 + 2];
    u32 m2[RSA_MAX_WORD_LEN/2 + 2];

    u32 nWordLen;
    u32 pWordLen;
    u32 uWordLen;
    u32 temp_WordLen;

    u32 pBitLen;
    u32 temp_bitLen;

    u32 tempp_WordLen;
    u32 tempq_WordLen;
    u32 p_step, n_step, tmp_step;

    volatile u16 crc1, crc2;
    u16 crc3;

    s32 flag;
    u32 ret;

    if(NULL == a || NULL == p || NULL == q || NULL == dp || NULL == dq || NULL == u || NULL == out)
    {
        return RSA_ERROR_S;
    }
    else if(nBitLen > RSA_MAX_BIT_LEN || nBitLen < RSA_MIN_BIT_LEN || (nBitLen&1))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    if(a == out)
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    nWordLen = GET_WORD_LEN(nBitLen);
    pBitLen = nBitLen/2;
    pWordLen = GET_WORD_LEN(pBitLen);
    temp_WordLen = pWordLen + 2;

    //get hardware step
    pke_set_operand_width(nBitLen);
    n_step = pke_get_operand_bytes();
    pke_set_operand_width(pBitLen);
    p_step = pke_get_operand_bytes();

    //get tmp_n = p*q
    ret = pke_mul_internal(p, q, tmp_n, pWordLen, pWordLen, nWordLen);
    if((PKE_SUCCESS != ret) || (0 == (tmp_n[0] & 1)))
    {
        return RSA_ERROR_S;
    }
    else
    {;}

    //a should be in [0,n]
    flag = uint32_BigNumCmp(a, nWordLen, tmp_n, nWordLen);
    if(flag > 0)
    {
        return RSA_ERROR_S;
    }
    else if((0 == flag) || (1 == uint32_BigNum_Check_Zero(a, nWordLen)))
    {
        //if a is 0 or n, the output is 0
        uint32_clear(out, nWordLen);
        return RSA_SUCCESS_S;
    }
    else
    {;}

    //crc1 = crc(d||n) = crc((d XOR rand)||n) XOR crc(rand||00...00), here d represents dp||p||dq||u||q
    (void)get_rand_fast((u8 *)(PKE_B(0,n_step)), (pWordLen*5) << 2);
    uint32_XOR((u32 *)(PKE_B(0,n_step)),              dp, (u32 *)(PKE_A(0,n_step)),              pWordLen);
    uint32_XOR((u32 *)(PKE_B(0,n_step)) + pWordLen,   p,  (u32 *)(PKE_A(0,n_step)) + pWordLen,   pWordLen);
    uint32_XOR((u32 *)(PKE_B(0,n_step)) + pWordLen*2, dq, (u32 *)(PKE_A(0,n_step)) + pWordLen*2, pWordLen);
    uint32_XOR((u32 *)(PKE_B(0,n_step)) + pWordLen*3, u,  (u32 *)(PKE_A(0,n_step)) + pWordLen*3, pWordLen);
    uint32_XOR((u32 *)(PKE_B(0,n_step)) + pWordLen*4, q,  (u32 *)(PKE_A(0,n_step)) + pWordLen*4, pWordLen);
    crc1 = crc16_calc((u8 *)(PKE_A(0,n_step)), (pWordLen*5) << 2, 0);
    crc1 = crc16_calc((u8 *)tmp_n, nWordLen<<2, crc1);

    crc3 = crc16_calc((u8 *)(PKE_B(0,n_step)), (pWordLen*5) << 2, 0);
    uint32_clear((u32 *)(PKE_B(0,n_step)), nWordLen);
    crc3 = crc16_calc((u8 *)(PKE_B(0,n_step)), nWordLen<<2, crc3);
    crc1 ^= crc3;

    /************* get 64bit random y, and y is odd    *************/
    ret = get_rand((u8 *)y, 8);
    if(TRNG_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    y[0] |= 0x01;
    uint32_clear(y+2, pWordLen-2);

    /************************* tmpp = p1*y + p2*y = py ***********************/
    //get random big number h1 < p
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, y, tmpp, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    ret = pke_sub(p, h1, k, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(k, y, tmp_buf, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(k, y, (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmp_buf, tmpp, tmpp, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), tmpp, tmpp, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get m2 ***********************/
    temp_bitLen = get_valid_bits(tmpp, temp_WordLen);
    tempp_WordLen = GET_WORD_LEN(temp_bitLen);
    ret = pke_pre_calc_mont(tmpp, temp_bitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = a mod tmpp
    tmp_step = pke_get_operand_bytes();
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(a, nWordLen, tmpp, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempp_WordLen, tmp_buf);
#else
    ret = pke_mod(a, nWordLen, tmpp, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempp_WordLen, tmp_buf);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf^dp mod tmpp
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp_without_pub(tmpp, dp, tmp_buf, dp1, tempp_WordLen, pWordLen);
#else
    ret = pke_modexp_without_pub((u32 *)(PKE_B(3,tmp_step)), dp, tmp_buf, dp1, tempp_WordLen, pWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //////////////// CRC --- dp
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    (void)get_rand_fast((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2);
    uint32_XOR((u32 *)(PKE_A(0,tmp_step)), (u32 *)(PKE_B(0,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,tmp_step)), (pWordLen) << 2, 0);
    crc3 = crc16_calc((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2, 0);

    ret = pke_pre_calc_mont(p, pBitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //m2 = dp1 mod p
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, m2);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //////////////// CRC --- p
    ret = pke_add(h1, k, (u32 *)(PKE_A(0,p_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    (void)get_rand_fast((u8 *)(PKE_B(0,p_step)), (pWordLen) << 2);
    uint32_XOR((u32 *)(PKE_A(0,p_step)), (u32 *)(PKE_B(0,p_step)), (u32 *)(PKE_A(0,p_step)), pWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,p_step)), (pWordLen) << 2, crc2);
    crc3 = crc16_calc((u8 *)(PKE_B(0,p_step)), (pWordLen) << 2, crc3);

    /************* get 64bit random y, and y is odd    *************/
    ret = get_rand((u8 *)y, 8);
    if(TRNG_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    y[0] |= 0x01;
    uint32_clear(y+2, pWordLen-2);

    /************************* tmpq = q1*y + q2*y = qy ***********************/
    //get random big number h1 < q
    ret = uint32_get_rand_big_number_msb_0(h1, pBitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_sub(q, h1, k, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = pke_mul_internal(h1, y, tmpq, pWordLen, pWordLen, temp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(k, y, tmp_buf, pWordLen, pWordLen, temp_WordLen);
#else
    ret = pke_mul_internal(k, (u32 *)(PKE_B(0,tmp_step)), (u32 *)(PKE_A(1,tmp_step)), pWordLen, pWordLen, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(tmp_buf, tmpq, tmpq, temp_WordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), tmpq, tmpq, temp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get m1 ***********************/
    temp_bitLen = get_valid_bits(tmpq, temp_WordLen);
    tempq_WordLen = GET_WORD_LEN(temp_bitLen);
    ret = pke_pre_calc_mont(tmpq, temp_bitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = a mod tmpq
    tmp_step = pke_get_operand_bytes();
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(a, nWordLen, tmpq, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempq_WordLen, tmp_buf);
#else
    ret = pke_mod(a, nWordLen, tmpq, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), tempq_WordLen, tmp_buf);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf^dq mod tmpq
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modexp_without_pub(tmpq, dq, tmp_buf, dp1, tempq_WordLen, pWordLen);
#else
    ret = pke_modexp_without_pub((u32 *)(PKE_B(3,tmp_step)), dq, tmp_buf, dp1, tempq_WordLen, pWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //////////////// CRC --- dq
    ret = pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    (void)get_rand_fast((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2);
    uint32_XOR((u32 *)(PKE_A(0,tmp_step)), (u32 *)(PKE_B(0,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,tmp_step)), (pWordLen) << 2, crc2);
    crc3 = crc16_calc((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2, crc3);

    ret = pke_pre_calc_mont(q, pBitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //m1 = dp1 mod q
    ret = pke_mod(dp1, tempq_WordLen, q, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, m1);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    /************************* get mx ***********************/
    //get random big number dp1 < u
    temp_bitLen = get_valid_bits(u, pWordLen);
    uWordLen = GET_WORD_LEN(temp_bitLen);
    ret = uint32_get_rand_big_number_msb_0(dp1, temp_bitLen);
    if(ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //u = dp1 + cx
    uint32_clear(dp1 + uWordLen, tempp_WordLen - uWordLen);
    uint32_clear(cx + uWordLen, tempp_WordLen - uWordLen);
    ret = pke_sub(u, dp1, cx, uWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //tmp_buf = (m2 - m1) mod tmpp
    uint32_clear(m1 + pWordLen, tempp_WordLen - pWordLen);
    uint32_clear(m2 + pWordLen, tempp_WordLen - pWordLen);
    ret = pke_modsub(tmpp, m2, m1, tmp_buf, tempp_WordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

    //back up u1
    uint32_copy((u32 *)(PKE_B(1,tmp_step)), dp1, pWordLen);

    //dp1 = tmp_buf*u1 mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul(tmpp, dp1, tmp_buf, dp1, tempp_WordLen);
#else
    ret = pke_modmul((u32 *)(PKE_B(3,tmp_step)), dp1, tmp_buf, dp1, tempp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //back up u2
    uint32_copy((u32 *)(PKE_A(2,tmp_step)), cx, pWordLen);

    //cx = tmp_buf*u2 mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modmul_internal(cx, tmp_buf, cx, tempp_WordLen);
#else
    ret = pke_modmul_internal(cx, tmp_buf, (u32 *)(PKE_A(0,tmp_step)), tempp_WordLen);//B0 corrupted after modmul
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //dp1 = tmp_buf*u mod p'
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_modadd(tmpp, cx, dp1, dp1, tempp_WordLen);
#else
    ret = pke_modadd((u32 *)(PKE_B(3,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), dp1, dp1, tempp_WordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //////////////// CRC --- u
    ret = pke_add((u32 *)(PKE_A(2,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    (void)get_rand_fast((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2);
    uint32_XOR((u32 *)(PKE_A(0,tmp_step)), (u32 *)(PKE_B(0,tmp_step)), (u32 *)(PKE_A(0,tmp_step)), pWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,tmp_step)), (pWordLen) << 2, crc2);
    crc3 = crc16_calc((u8 *)(PKE_B(0,tmp_step)), (pWordLen) << 2, crc3);

    ret = pke_pre_calc_mont(p, pBitLen, NULL, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //cx = dp1 mod p
#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, cx);
#else
    ret = pke_mod(dp1, tempp_WordLen, p, (u32 *)(PKE_A(3,p_step)), (u32 *)(PKE_B(4,p_step)), pWordLen, (u32 *)(PKE_B(0,n_step)));
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(k, cx, tmp_buf, pWordLen, pWordLen, nWordLen);
#else
    ret = pke_mul_internal(k, (u32 *)(PKE_B(0,n_step)), tmp_buf, pWordLen, pWordLen, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //back up k
    uint32_copy((u32 *)(PKE_A(4,n_step)), (u32 *)(PKE_A(0,n_step)), pWordLen);

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_mul_internal(h1, cx, cx, pWordLen, pWordLen, nWordLen);
#else
    ret = pke_mul_internal(h1, (u32 *)(PKE_B(0,n_step)), (u32 *)(PKE_A(1,n_step)), pWordLen, pWordLen, nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    ret = pke_add(cx, tmp_buf, tmp_buf, nWordLen);
#else
    ret = pke_add((u32 *)(PKE_A(1,n_step)), tmp_buf, (u32 *)(PKE_A(1,n_step)), nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef RSA_SEC_API_OPTIMIZATION
    uint32_copy(cx, m1, pWordLen);
    uint32_clear(cx+pWordLen, nWordLen - pWordLen);
    ret = pke_add(tmp_buf, cx, (u32 *)(PKE_B(2,n_step)), nWordLen);
#else
    uint32_copy((u32 *)(PKE_B(1,n_step)), m1, pWordLen);
    uint32_clear((u32 *)(PKE_B(1,n_step))+pWordLen, nWordLen - pWordLen);
    ret = pke_add((u32 *)(PKE_A(1,n_step)), (u32 *)(PKE_B(1,n_step)), (u32 *)(PKE_B(2,n_step)), nWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}

    //////////////// CRC --- q
    ret = pke_add(h1, (u32 *)(PKE_A(4,n_step)), (u32 *)(PKE_A(0,n_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    (void)get_rand_fast((u8 *)(PKE_B(0,n_step)), (pWordLen) << 2);
    uint32_XOR((u32 *)(PKE_A(0,n_step)), (u32 *)(PKE_B(0,n_step)), (u32 *)(PKE_A(0,n_step)), pWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,n_step)), (pWordLen) << 2, crc2);
    crc3 = crc16_calc((u8 *)(PKE_B(0,n_step)), (pWordLen) << 2, crc3);

    //////////////// CRC --- n
    (void)get_rand_fast((u8 *)(PKE_B(0,n_step)), (nWordLen) << 2);
    uint32_XOR(tmp_n, (u32 *)(PKE_B(0,n_step)), (u32 *)(PKE_A(0,n_step)), nWordLen);
    crc2 = crc16_calc((u8 *)(PKE_A(0,n_step)), (nWordLen) << 2, crc2);
    crc3 = crc16_calc((u8 *)(PKE_B(0,n_step)), (nWordLen) << 2, crc3);
    crc2 ^= crc3;

    //check crc three or more times
    (void)get_rand_fast((u8 *)&ret, 4);

    if(0 != (crc1 - crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret)&0xFF);

    if(0 != (crc1 ^ crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret>>8)&0xFF);

    if((crc1 & crc2) != (crc1 | crc2))
    {
        ret = RSA_ERROR_S;
        goto END;
    }
    else
    {;}
    uint32_sleep(nWordLen, (ret>>16)&0xFF);

    //output
    uint32_copy(out, (u32 *)(PKE_B(2,n_step)), nWordLen);

    ret = RSA_SUCCESS_S;

END:

    (void)get_rand_fast((u8 *)y,    (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)k,    (pWordLen)<<2);
    (void)get_rand_fast((u8 *)h1,   (pWordLen)<<2);
    (void)get_rand_fast((u8 *)dp1,  (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)tmpp, (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)tmpq, (pWordLen+2)<<2);
    (void)get_rand_fast((u8 *)cx,   (nWordLen)<<2);
    (void)get_rand_fast((u8 *)m2,   (pWordLen+2)<<2);

    return ret;
}

#endif

