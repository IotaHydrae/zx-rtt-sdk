//#include <stdio.h>

#include <aic_core.h>

#ifdef AIC_PKE_RSA_SUPPORT

#include <rsa.h>
#include <pke_prime.h>
#include <trng.h>
#include <utility.h>

/* function: out = a^e mod n
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n
 *     e -------------------------- input, u32 big integer e, exeponent, make sure e < n
 *     n -------------------------- input, u32 big integer n, modulus, make sure n is odd
 *     out ------------------------ output, out = a^e mod n
 *     eBitLen  ------------------- input, real bit length of u32 big integer e
 *     nBitLen  ------------------- input, real bit length of u32 big integer n
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. a, n, and out have the same word length:((nBitLen+31)>>5); and e word length is (eBitLen+31)>>5
 */
u32 RSA_ModExp(u32 *a, u32 *e, u32 *n, u32 *out, u32 eBitLen, u32 nBitLen)
{
    u32 eWordLen = GET_WORD_LEN(eBitLen);
    u32 nWordLen = GET_WORD_LEN(nBitLen);
    u32 ret;

    if(NULL == a || NULL == e || NULL == n || NULL == out)
    {
        return RSA_BUFFER_NULL;
    }
    else if((nBitLen > RSA_MAX_BIT_LEN) || (eBitLen > nBitLen))
    {
        return RSA_INPUT_TOO_LONG;
    }
    else if((nBitLen == 0) || (!(n[0] & 1)))
    {
        return RSA_INPUT_INVALID;
    }
    else
    {;}

    ret = pke_modexp_check_input((const u32 *)n, (const u32 *)e, (const u32 *)a,
            out, nWordLen, eWordLen);
    if(PKE_FINISHED == ret)
    {
        return RSA_SUCCESS;
    }
    else if(PKE_SUCCESS != ret)
    {
        return ret;
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
        return ret;
    }
    else
    {;}

    return pke_modexp(n, e, a, out, nWordLen, eWordLen);
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
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. a and out have the same word length:((nBitLen+31)>>5); and p,p_h,q,q_h,dp,dq,u
 *        have the same word length:((nBitLen/2+31)>>5)
 */
u32 RSA_CRTModExp(u32 *a, u32 *p, u32 *q, u32 *dp, u32*dq, u32 *u,
        u32 *out, u32 nBitLen)
{
    u32 buf[RSA_MAX_WORD_LEN];
    u32 *m1 = buf;
    u32 *m2 = buf+(RSA_MAX_WORD_LEN/2);
    u32 *tmp_out;
    u32 tmp_step;
    u32 nWordLen = GET_WORD_LEN(nBitLen);
    u32 pBitLen = nBitLen/2;
    u32 pWordLen = GET_WORD_LEN(pBitLen);
    s32 flag;
    u32 ret;

    if(NULL == a || NULL == p || NULL == q || NULL == dp || NULL == dq || NULL == u || NULL == out)
    {
        return RSA_BUFFER_NULL;
    }
    else if(nBitLen > RSA_MAX_BIT_LEN)
    {
        return RSA_INPUT_TOO_LONG;
    }
    else if((nBitLen == 0) || (nBitLen&1))
    {
        return RSA_INPUT_INVALID;
    }
    else
    {;}

    //get n = p*q
    ret = pke_mul(p, q, buf, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //a should be in [0,n]
    flag = uint32_BigNumCmp(a, nWordLen, buf, nWordLen);
    if(flag > 0)
    {
        return RSA_INPUT_INVALID;
    }
    else if((0 == flag) || (1 == uint32_BigNum_Check_Zero(a, nWordLen)))  //if a is 0 or n, the output is 0
    {
        uint32_clear(out, nWordLen);
        return RSA_SUCCESS;
    }
    else
    {;}

    //do pke_pre_calc_mont() first, because a may be less than p or q, then pke_mod() will not
    //call pke_pre_calc_mont() inside, but pke_modexp() needs the output of pke_pre_calc_mont().

    //m2 = (a) mod q
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(q, pBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(q, pBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get the pBitLen step
    tmp_step = pke_get_operand_bytes();

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_mod(a, nWordLen, q, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), pWordLen, m2);
#else
    ret = pke_mod(a, nWordLen, q, (u32 *)(PKE_B(0,tmp_step)), pWordLen, m2);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //m2 = (a)^dq mod q
    ret = pke_modexp(q, dq, m2, m2, pWordLen, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //m1 = (a) mod p
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(p, pBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(p, pBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_mod(a, nWordLen, p, (u32 *)(PKE_A(3,tmp_step)), (u32 *)(PKE_B(4,tmp_step)), pWordLen, m1);
#else
    ret = pke_mod(a, nWordLen, p, (u32 *)(PKE_B(0,tmp_step)), pWordLen, m1);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //m1 = (a)^dp mod p
    ret = pke_modexp(p, dp, m1, m1, pWordLen, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
     tmp_out = (u32 *)(PKE_B(0,tmp_step));
#else
     tmp_out = (u32 *)(PKE_B(1,tmp_step));
#endif

    //m1 = (m1-m2) mod p
    if(uint32_BigNumCmp(m2, pWordLen, p, pWordLen) >= 0)
    {
        //if m2 >= p, get tmp_out = m2 mod p
        ret = pke_sub(m2, p, tmp_out, pWordLen);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}

        ret = pke_modsub(p, m1, tmp_out, m1, pWordLen);
    }
    else
    {
        ret = pke_modsub(p, m1, m2, m1, pWordLen);
    }

    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //m1 = h = u*(m1-m2) mod p
#if 1
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    ret = pke_modmul_internal(m1, u, m1, pWordLen);
#else
    ret = pke_modmul(p, m1, u, m1, pWordLen);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //store the nBitLen step
    tmp_step = pke_set_operand_width(nBitLen);

    //A1 = hq
    ret = pke_mul(m1, q, (u32 *)(PKE_A(1,tmp_step)), pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //out = m2+hq
    uint32_copy((u32 *)(PKE_B(1,tmp_step)), m2, pWordLen);
    uint32_clear((u32 *)(PKE_B(1,tmp_step))+pWordLen, nWordLen-pWordLen);
    return pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), out, nWordLen);
}


/* function: get big odd integer e of eBitLen
 * parameters:
 *     e -------------------------- input, u32 big odd integer e
 *     eBitLen  ------------------- input, bit length of u32 big odd integer e
 * return: 0(success), 1(error: eBitLen<2)
 * caution:
 *     1. eBitLen must be big than 1
 */
u32 RSA_Get_E1(u32 e[], u32 eBitLen)
{
    u32 eWordLen = (eBitLen+0x1F)>>5;
    u32 ret;

    if(eBitLen<2)
    {
        return RSA_INPUT_INVALID;
    }
    else
    {;}

    ret = get_rand((u8 *)e, eWordLen<<2);
    if(TRNG_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    eBitLen &= 31;

    if(eBitLen)
    {
#if 0
        e[eWordLen - 1] <<= (32 - eBitLen);
        e[eWordLen - 1] |= 0x80000000;
        e[eWordLen - 1] >>= (32 - eBitLen);
#else
        e[eWordLen - 1] &= (1<<(eBitLen))-1;
        e[eWordLen - 1] |= 1<<(eBitLen - 1);
#endif
    }
    else
    {
        e[eWordLen - 1] |= 0x80000000;
    }

    e[0] |= 0x01;          //make e odd

    return 0;
}


/* function: get big odd integer e of eBitLen, satisfies e < fai_n of bitLen
 * parameters:
 *     e -------------------------- input, u32 big odd integer e
 *     fai_n ---------------------- input, u32 big even integer fai_n
 *     bitLen   ------------------- input, bit length of u32 big odd integer e and n
 * return: 0(success), 1(error: bitLen<66), 2(error, n is 1000000000...000000)
 * caution:
 *     1. eBitLen must be big than 65
 *     2. n can not be 1000000000...000000
 */
u32 RSA_Get_E2(u32 e[], u32 fai_n[], u32 bitLen)
{
    u32 wordLen;
    s32 i;
    u8 j;

    if(bitLen < 66)
        return 1;

    RSA_Get_E1(e, bitLen);
    wordLen = (bitLen+0x1F)>>5;
    j = bitLen&31;                   // namely j = eBitLen%32;
    if(j==0)
    {
        j = 32;
    }                                //此时j表示e或phi_n的最高字的比特长度
    j--;                             //此时j表示e或phi_n的次高比特在最高字的位置，若为0则要到次高字了
    i = wordLen - 1;                 //i表示最高字位置

    if(j==0)                         //j为0，则要到次高字
    {
        i--;
        j=32;
    }

    while(i>=0)
    {

        e[i] &= (~(1<<(j-1)));
        if(uint32_BigNumCmp(e, i+1, fai_n, i+1) < 0)       //if e < n
        {
            return 0;
        }

        j--;
        if(0 == j)                     //j为0，则要到下一字
        {
            i--;
            j=32;
        }
    }

    return 2;          //fail, because n is 1000000000...000000
}


/* function: judge whether big integer a is equal to 0x5a5a5a5a5a...5a or not
 * parameters:
 *     a -------------------------- input, u32 big integer a
 *     aBitLen -------------------- input, real bit length of a
 * return: 0(a==0x5a5a5a5a5a...5a), 1(a!=0x5a5a5a5a5a...5a)
 * caution:
 *     1. aBitLen can not be 0
 *     2. if aBitLen%32 != 0, then the highest word of a should be 0
 */
u32 CheckValue_0x5a5a5a5a(u32 a[], u32 aBitLen)
{
    u32 i, wordLen = aBitLen>>5;

    if(aBitLen & 0x1F)
    {
        if(a[wordLen] != 0)
        {
            return 1;
        }
        else
        {;}
    }
    else
    {;}

    for(i=0; i<wordLen; i++)
    {
        if(a[i] != 0x5a5a5a5a)
        {
            return 1;
        }
        else
        {;}
    }

    return 0;
}


/* function: generate RSA key (e,d,n)
 * parameters:
 *     e -------------------------- output, u32 big integer, RSA public key e
 *     d -------------------------- output, u32 big integer, RSA private key d
 *     n -------------------------- output, u32 big integer, RSA public module n
 *     eBitLen  ------------------- input, real bit length of e
 *     nBitLen  ------------------- input, real bit length of n
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. nBitLen can not be even
 *     2. eBitLen must be larger than 1, and less than or equal to nBitLen
 */
u32 RSA_GetKey(u32 *e, u32 *d, u32 *n, u32 eBitLen, u32 nBitLen)
{
    u32 buf[RSA_MAX_WORD_LEN];
    u32 *p, *q, *in, *out;
    u32 pBitLen, pWordLen, eWordLen, nWordLen, tmp_step;
    u32 count, ret;

    if(NULL == e || NULL == d || NULL == n)
    {
        return RSA_BUFFER_NULL;
    }
    else if(nBitLen&1 || nBitLen < RSA_MIN_BIT_LEN || nBitLen > RSA_MAX_BIT_LEN)  //nBitLen can not be odd
    {
        return RSA_INPUT_INVALID;
    }
    else if(eBitLen<2 || eBitLen>nBitLen)
    {
        return RSA_INPUT_INVALID;
    }
    else
    {;}

    p = buf;
    q = buf+RSA_MAX_WORD_LEN/2;

    tmp_step = pke_set_operand_width(nBitLen);

    in = (u32 *)(PKE_B(1,tmp_step));
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    out = (u32 *)(PKE_A(2,tmp_step));
#else
    out = (u32 *)(PKE_A(1,tmp_step));
#endif

    eWordLen = GET_WORD_LEN(eBitLen);
    nWordLen = GET_WORD_LEN(nBitLen);
    pBitLen = nBitLen>>1;
    pWordLen = GET_WORD_LEN(pBitLen);

GET_PQ:

    ret = get_prime(p, pBitLen);
    if(ret)
    {
        return ret;
    }
    else
    {;}

    ret = get_prime(q, pBitLen);
    if(ret)
    {
        return ret;
    }
    else
    {;}

    p[0]--;                                            // p=p-1
    q[0]--;                                            // q=q-1
    ret = pke_mul(p, q, n, pWordLen);                  // get fai(n)=(p-1)(q-1)
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    count = 0;
GET_E:
    count++;
    if(count == 7)
    {
        goto GET_PQ;
    }
    else
    {;}

    switch(eBitLen)
    {
        case 2 :  {e[0] = 3; break;}
        case 5 :  {e[0] = 17; break;}
        case 17:  {e[0] = 65537; break;}
        default:
        {
            if(eBitLen == nBitLen)
            {
                ret = RSA_Get_E2(e, n, eBitLen);
                if(ret)
                {
                    return ret;
                }
                else
                {;}
            }
            else
            {
                ret = RSA_Get_E1(e, eBitLen);
                if(ret)
                {
                    return ret;
                }
                else
                {;}
            }
            break;
        }
    }

    //get d = e^(-1) mod n
    ret = pke_modinv(n, e, d, nWordLen, eWordLen);
    if(PKE_NO_MODINV == ret)                           //if d doesn't exist
    {
        if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
        {
            goto GET_PQ;
        }
        else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
        {                                              //e, fai(n) have common divisor.
            goto GET_E;
        }
    }
    else if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get n = pq
    p[0]++;
    q[0]++;
    ret = pke_mul(p, q, n, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
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
        return ret;
    }
    else
    {;}

    //Encryption test
    if(nBitLen & 0x1F)
    {
        in[nWordLen-1]=0;
    }
    else
    {;}

    uint32_set(in, 0x5a5a5a5a, nBitLen>>5);

    ret = pke_modexp(n, e, in, out, nWordLen, eWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    ret = pke_modexp(n, d, out, out, nWordLen, nWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(CheckValue_0x5a5a5a5a(out, nBitLen))
    {
        goto GET_PQ;
    }
    else
    {
        return RSA_SUCCESS;
    }
}


/* Function: generate RSA-CRT key (e,p,q,dp,dq,u,n)
 * Parameters:
 *     e -------------------------- output, u32 big integer, RSA public key e
 *     p -------------------------- output, u32 big integer, RSA private key p
 *     q -------------------------- output, u32 big integer, RSA private key q
 *     dp-------------------------- output, u32 big integer, RSA private key dp
 *     dq-------------------------- output, u32 big integer, RSA private key dq
 *     u -------------------------- output, u32 big integer, RSA private key u = q^(-1) mod p
 *     n -------------------------- output, u32 big integer, RSA public module n
 *     eBitLen  ------------------- input, real bit length of e
 *     nBitLen  ------------------- input, real bit length of n
 * Return: RSA_SUCCESS(success), other(error)
 * Caution:
 *     1. nBitLen can not be even
 *     2. eBitLen must be larger than 1, and less than or equal to nBitLen
 */
u32 RSA_GetCRTKey(u32 *e, u32 *p, u32 *q, u32 *dp, u32 *dq, u32 *u,
        u32 *n, u32 eBitLen, u32 nBitLen)
{
    u32 buf[RSA_MAX_WORD_LEN];
    u32 pBitLen, pWordLen, eWordLen, nWordLen, i, wordLen;
    s32 count;
    u32 ret;

    if(NULL == e || NULL == p || NULL == q || NULL == dp || NULL == dq || NULL == u || NULL == n)
    {
        return RSA_BUFFER_NULL;
    }
    else if((nBitLen&1) || (nBitLen<RSA_MIN_BIT_LEN) || (nBitLen > RSA_MAX_BIT_LEN))  //nBitLen can not be odd
    {
        return RSA_INPUT_INVALID;
    }
    else if(eBitLen<2 || eBitLen>nBitLen)
    {
        return RSA_INPUT_INVALID;
    }
    else
    {;}

    eWordLen = GET_WORD_LEN(eBitLen);
    nWordLen = GET_WORD_LEN(nBitLen);
    pBitLen = nBitLen>>1;
    pWordLen = GET_WORD_LEN(pBitLen);

GET_PQ:

    ret = get_prime(p, pBitLen);
    if(ret)
    {
        return ret;
    }
    else
    {;}

    ret = get_prime(q, pBitLen);
    if(ret)
    {
        return ret;
    }
    else
    {;}

    count = uint32_BigNumCmp(p, pWordLen, q, pWordLen);         // make p > q, for get u = q^(-1) mod p convenient
    if(count == -1)
    {
        for(i=0; i<pWordLen; i++)
        {
            wordLen = p[i];
            p[i] = q[i];
            q[i] = wordLen;
        }
    }
    else if(count == 0)
    {
        goto GET_PQ;
    }
    else
    {;}

    p[0]--;                                                // p=p-1
    q[0]--;                                                // q=q-1
    if(eBitLen == nBitLen)
    {
        ret = pke_mul(p, q, n, pWordLen); // get fai(n)=(p-1)(q-1)
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }
    else
    {;}

    count = 0;
GET_E:
    count++;
    if(count == 7)
    {
        goto GET_PQ;
    }
    else
    {;}

    switch(eBitLen)
    {
        case 2 :  {e[0] = 3; break;}
        case 5 :  {e[0] = 17; break;}
        case 17:  {e[0] = 65537; break;}
        default:
        {
            if(eBitLen == nBitLen)
            {
                ret = RSA_Get_E2(e, n, eBitLen);
                if(ret)
                {
                    return ret;
                }
                else
                {;}
            }
            else
            {
                ret = RSA_Get_E1(e, eBitLen);
                if(ret)
                {
                    return ret;
                }
                else
                {;}
            }
            break;
        }
    }

    // dp = e^(-1) mod (p-1)
    if(uint32_BigNumCmp(e, eWordLen, p, pWordLen) > 0)
    {
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
        ret = pke_mod(e, eWordLen, p, NULL, NULL, pWordLen, u);
#else
        ret = pke_mod(e, eWordLen, p, NULL, pWordLen, u);
#endif
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}

        wordLen = pWordLen;
    }
    else
    {
        uint32_copy(u, e, eWordLen);
        wordLen = eWordLen;
    }

    ret = pke_modinv(p, u, dp, pWordLen, wordLen);
    if(PKE_NO_MODINV == ret)
    {
        if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
        {
            goto GET_PQ;
        }
        else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
        {                                              //e, fai(n) have common divisor.
            goto GET_E;
        }
    }
    else if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    // dq = e^(-1) mod (q-1)
    if(uint32_BigNumCmp(e, eWordLen, q, pWordLen) > 0)
    {
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
        ret = pke_mod(e, eWordLen, q, NULL, NULL, pWordLen, u);
#else
        ret = pke_mod(e, eWordLen, q, NULL, pWordLen, u);
#endif
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}

        wordLen = pWordLen;
    }
    else
    {
        uint32_copy(u, e, eWordLen);
        wordLen = eWordLen;
    }

    ret = pke_modinv(q, u, dq, pWordLen, wordLen);
    if(PKE_NO_MODINV == ret)
    {
        if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
        {
            goto GET_PQ;
        }
        else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
        {                                              //e, fai(n) have common divisor.
            goto GET_E;
        }
    }
    else if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    p[0]++;
    q[0]++;

    // u = q^(-1) mod p
    ret = pke_modinv(p, q, u, pWordLen, pWordLen);
    if(PKE_NO_MODINV == ret)
    {
        goto GET_PQ;
    }
    else if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    // get n
    ret = pke_mul(p, q, n, pWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //Encryption test
    if(nBitLen & 0x1F)
    {
        buf[nWordLen-1]=0;
    }
    else
    {;}

    wordLen = nBitLen>>5;
    uint32_set(buf, 0x5a5a5a5a, wordLen);

#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
    ret = pke_pre_calc_mont(n, nBitLen, NULL, NULL);
#else
    ret = pke_pre_calc_mont(n, nBitLen, NULL);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    ret = pke_modexp(n, e, buf, buf, nWordLen, eWordLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(!CheckValue_0x5a5a5a5a(buf, nBitLen))
    {
        goto GET_PQ;
    }
    else
    {;}

    ret = RSA_CRTModExp(buf, p, q, dp, dq, u, buf, nBitLen);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    if(CheckValue_0x5a5a5a5a(buf, nBitLen))
    {
        goto GET_PQ;
    }
    else
    {
        return RSA_SUCCESS;
    }
}

#endif

