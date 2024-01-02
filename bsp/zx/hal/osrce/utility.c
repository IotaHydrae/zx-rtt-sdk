//#include <stdio.h>

#include <utility.h>

void memcpy_(u8 *dst, u8 *src, u32 size)
{
#if 0
    while(size--)
    {
        *dst++ = *src++;
    }
#else
    u32 *aa = (u32 *)dst;
    u32 *bb = (u32 *)src;
    u32 i, count, tmp;

    if((0U != (CAST2UINT32(dst) & 3U)) || (0U != (CAST2UINT32(src) & 3U)))
    {
        while(0U != (size--))
        {
            *dst++ = *src++;
        }
    }
    else
    {
        count = size/4U;
        for(i=0; i<count; i++)
        {
            *aa++ = *bb++;
        }

        tmp = size&3U;
        if(0U != tmp)
        {
            dst = &(dst[size&(~0x03U)]);
            src = &(src[size&(~0x03U)]);
            while(0U != (tmp--))
            {
                *dst++ = *src++;
            }
        }
        else
        {;}
    }
#endif
}

void memset_(u8 *dst, u8 value, u32 size)
{
#if 0
    while(size--)
    {
        *dst++ = value;
    }
#else
    u32 i, count, tmp;

    tmp = ((u32)dst) & 3U;
    if(0U != tmp)
    {
        if(size > 4U-tmp)
        {
            for(i=0U; i<4U-tmp; i++)
            {
                *dst++ = value;
            }
            size -= (4U-tmp);
        }
        else
        {
            for(i=0U; i<size; i++)
            {
                *dst++ = value;
            }
            return;
        }
    }
    else
    {;}

    count = size/4U;
    if(0U != count)
    {
        tmp = value;
        tmp = (tmp<<8)|value;
        tmp = (tmp<<8)|value;
        tmp = (tmp<<8)|value;
        for(i=0; i<count; i++)
        {
            *((u32 *)dst) = tmp;
            dst=&(dst[4]);
        }
    }
    else
    {;}

    tmp = size&3U;
    if(0U != tmp)
    {
        for(i=0; i<tmp; i++)
        {
            *dst++ = value;
        }
    }
    else
    {;}
#endif
}

u8 memcmp_(u8 *m1, u8 *m2, u32 size)
{
    u8 c;

    while(0U != (size--))
    {
        c = ((*m1++) - (*m2++));
        if((u8)0 != c)
        {
            return c;
        }
        else
        {;}
    }

    return 0;
}

/* function: set uint32 buffer
 * parameters:
 *     a -------------------------- output, output word buffer
 *     value ---------------------- input, input word value
 *     wordLen -------------------- input, word length of buffer a
 * return: none
 * caution:
 */
void uint32_set(u32 *a, u32 value, u32 wordLen)
{
    while(0U != wordLen)
    {
        a[--wordLen] = value;
    }
}

/* function: copy uint32 buffer
 * parameters:
 *     dst ------------------------ output, output word buffer 
 *     src ------------------------ input, input word buffer
 *     wordLen -------------------- input, word length of buffer dst or src
 * return: none
 * caution:  
 */
void uint32_copy(u32 *dst, u32 *src, u32 wordLen)
{
    u32 i;

    if(dst != src)
    {
        for(i=0; i<wordLen; i++)
        {
            dst[i] = src[i];
        }
    }
    else
    {;}
}


/* function: clear uint32 buffer
 * parameters:
 *     a -------------------------- input&output, word buffer a
 *     aWordLen ------------------- input, word length of buffer a
 * return: none
 * caution:  
 */
void uint32_clear(u32 *a, u32 wordLen)
{
#if 1
    volatile u32 i = wordLen;

    while(0U != i)
    {
        a[--i] = 0;
    }
#else
    volatile u32 i = 0;
    for(i=0;i<wordLen;i++)
    {
        a[i] = 0;
    }
#endif
}


/* function: sleep for a while
 * parameters:
 *     count ---------------------- input, counter for sleeping
 * return: a u32 value, actually no use, ignore this
 * caution:  
 */
static u32 uint32_sleep1(u32 count)
{
    u32 a=0, b=0;
    u32 i;
    volatile u32 result=0;

    for (i=0; i<count; i++)
    {
        result |= ((a++) - (b+i));
    }

    return result;
}


/* function: sleep for a while
 * parameters:
 *     count ---------------------- input, counter for sleeping
 * return: a u32 value, actually no use, ignore this
 * caution:  
 */
static u32 uint32_sleep2(u32 count)
{
    u32 a=0, b=0;
    u32 i;
    volatile u32 result=0;

    for (i=0; i<count; i++)
    {
        result |= ((a+i) ^ (b++));
    }

    return result;
}


/* function: sleep for a while
 * parameters:
 *     count ---------------------- input, count  
 * return: none
 * caution:  
 */
void uint32_sleep(u32 count, u8 rand)
{
    u8 rand1 = rand & 0x01U;

    if(0U == rand1)
    {
        (void)uint32_sleep1(count);
    }
    else
    {
        (void)uint32_sleep2(count);
    }
}


/* function: convert 0x1122334455667788 to 0x4433221188776655
 * parameters:
 *     in ------------------------- source address
 *     out ------------------------ destination address
 *     wordLen -------------------- word length of in/out
 * return: none
 * caution:
 */
void uint32_endian_reverse(u8 *in, u8 *out, u32 wordLen)
{
    u8 tmp;

    if(in == out)
    {
        while(wordLen>0U)
        {
            tmp=*in;
            in[0]=in[3];
            in[3]=tmp;
            in=&(in[1]);
            tmp=*in;
            in[0]=in[1];
            in[1]=tmp;
            wordLen--;
            in=&(in[3]);
        }
    }
    else
    {
        while(wordLen>0U)
        {
            out[0] = in[3];
            out[1] = in[2];
            out[2] = in[1];
            out[3] = in[0];
            wordLen--;
            in = &(in[4]);
            out = &(out[4]);
        }
    }
}


/* function: reverse word array
 * parameters:
 *     in ------------------------- input, input buffer
 *     out ------------------------ output, output buffer
 *     wordLen -------------------- input, word length of in or out
 * return: none
 * caution:
 *    1. in and out could point the same buffer

void reverse_word_array(u8 *in, u32 *out, u32 wordLen)
{
    u32 idx, round = wordLen >> 1;
    u32 tmp;
    u32 *p_in;

    if(((u32)(in))&3)
    {
        memcpy_(out, in, wordLen<<2);
        p_in = out;
    }
    else
    {
        p_in = (u32 *)in;
    }

    for (idx = 0; idx < round; idx++)
    {
        tmp = p_in[idx];
        out[idx] = p_in[wordLen - 1 - idx];
        out[wordLen - 1 - idx] = tmp;
    }

    if ((wordLen & 0x1) && (p_in != out))
    {
        out[round] = p_in[round];
    }
    else
    {;}
} */


/* function: reverse byte array
 * parameters:
 *     in ------------------------- input, input buffer
 *     out ------------------------ output, output buffer
 *     byteLen -------------------- input, byte length of in or out
 * return: none
 * caution:
 *    1. in and out could point the same buffer
 */
void reverse_byte_array(u8 *in, u8 *out, u32 byteLen)
{
    u32 idx, round = byteLen >> 1;
    u8 tmp;

    for (idx = 0; idx < round; idx++)
    {
        tmp = in[idx];
        out[idx] = in[byteLen - 1U - idx];
        out[byteLen - 1U - idx] = tmp;
    }

    if ((0U != (byteLen & 0x1U)) && (in != out))
    {
        out[round] = in[round];
    }
    else
    {;}
}


/* function: reverse byte order in every u32 word
 * parameters:
 *     in ------------------------- input, input byte buffer
 *     out ------------------------ output, output word buffer
 *     bytelen -------------------- input, byte length of buffer in or out
 * return: none
 * caution:  1. byteLen must be a multiple of 4

void reverse_word(u8 *in, u8 *out, u32 bytelen)
{
    u32 i, len;
    u8 tmp;
    u8 *p = in;

    if(in == out)
    {
        while(bytelen>0)
        {
            tmp=*p;
            *p=*(p+3);
            *(p+3)=tmp;
            p+=1;
            tmp=*p;
            *p=*(p+1);
            *(p+1)=tmp;
            bytelen-=4;
            p+=3;
        }
    }
    else
    {
        for (i = 0; i < bytelen; i++)
        {
            len = i >> 2;
            len = len << 3;
            out[i] = p[len + 3 - i];
        }
    }
}*/


/* function: reverse word order
 * parameters:
 *     in ------------------------- input, input word buffer
 *     out ------------------------ output, output word buffer
 *     wordLen -------------------- input, word length of buffer in or out
 *     reverse_word --------------- input, whether to reverse byte order in every word, 0:no, other:yes
 * return: none
 * caution:
 *     1. in DAM mode, the memory may be accessed by words, not by bytes, this function is designed
 *        for the case

void dma_reverse_word_array(u32 *in, u32 *out, u32 wordLen, u32 reverse_word)
{
    u32 i, j;
    u32 tmp;
    u32 *p=out;

    if(in == out)
    {
        for(i=0; i<wordLen; i+=4)
        {
            for (j = 0; j < 2; j++)
            {
                tmp = p[j];
                p[j] = p[4 - 1 - j];
                p[4 - 1 - j] = tmp;
            }
            p+=4;
        }
    }
    else
    {
        for(i=0; i<wordLen; i+=4)
        {
            p[0] = in[3];
            p[1] = in[2];
            p[2] = in[1];
            p[3] = in[0];
            p+=4;
            in+=4;
        }
    }

    if(reverse_word)
    {
        for (i = 0; i < wordLen; i++)
        {
            tmp = *out;
            *out = tmp&0xFF;
            *out <<= 8;
            *out |= (tmp>>8)&0xFF;
            *out <<= 8;
            *out |= (tmp>>16)&0xFF;
            *out <<= 8;
            *out |= (tmp>>24)&0xFF;

            out++;
        }
    }
    else
    {;}
} */


/* function: C = A XOR B
 * parameters:
 *     A -------------------------- input, byte buffer a
 *     B -------------------------- input, byte buffer b
 *     C -------------------------- output, C = A XOR B
 *     byteLen -------------------- input, byte length of A,B,C
 * return: none
 * caution:
 */
void uint8_XOR(u8 *A, u8 *B, u8 *C, u32 byteLen)
{
    u32 i;

    for(i=0; i<byteLen; i++)
    {
        C[i] = A[i] ^ B[i];
    }
}


/* function: C = A XOR B
 * parameters:
 *     A -------------------------- input, word buffer a
 *     B -------------------------- input, word buffer b
 *     C -------------------------- output, C = A XOR B
 *     byteLen -------------------- input, word length of A,B,C
 * return: none
 * caution:
 */
void uint32_XOR(u32 *A, u32 *B, u32 *C, u32 wordLen)
{
    u32 i;

    for(i=0; i<wordLen; i++)
    {
        C[i] = A[i] ^ B[i];
    }
}


/* Function: get aimed bit value of big integer a
 * Parameters:
 *     a -------------------------- big integer a
 *     bit_index ------------------ aimed bit location
 * Return:
 *     bit value of aimed bit
 * Caution:
 *     1. for the LSB, bit index is 0.
 */
u32 get_bit_value_by_index(const u32 *a, u32 bit_index)
{
    if(0u != (a[(bit_index) >> 5u] & ((u32)1u << (bit_index & 31u))))
    {
        return 1u;
    }
    else
    {
        return 0u;
    }
}


/* function: get real bit length of big number a of wordLen words
 */
u32 get_valid_bits(const u32 *a, u32 wordLen)
{
    u32 i;
    u32 j;

    if(0U == wordLen)
    {
        return 0;
    }
    else
    {;}

    for (i = wordLen; i > 0U; i--)
    {
        if (0U != a[i - 1U])
        {
            break;
        }
        else
        {;}
    }

    if(0U == i)
    {
        return 0U;
    }
    else
    {;}

    for (j = 32U; j > 0U; j--)
    {
        if (0U != (a[i - 1U] & (((u32)0x1) << (j - 1U))))
        {
            break;
        }
        else
        {;}
    }

    return ((i - 1U) << 5U) + j;
}


/* function: get real word lenth of big number a of max_words words
 * parameters:
 *     a -------------------------- input, big integer a
 *     max_words ------------------ input, max word length of a
 * return: real word length of big number a
 * caution:
 */
u32 get_valid_words(u32 *a, u32 max_words)
{
    u32 i;

    for (i = max_words; i > 0U; i--)
    {
        if (0U != a[i - 1U])
        {
            return i;
        }
        else
        {;}
    }

    return 0;
}


/* function: check whether big number or u8 buffer a is all zero or not
 * parameters:
 *     a -------------------------- input, byte buffer a
 *     aByteLen ------------------- input, byte length of a
 * return: 0(a is not zero),1(a is all zero)
 * caution:
 */
u8 uint8_BigNum_Check_Zero(u8 *a, u32 aByteLen)
{
    u32 i;

    for(i=0; i<aByteLen; i++)
    {
        if(0U != a[i])
        {
            return 0;
        }
        else
        {;}
    }

    return 1;
}


/* function: check whether big number or u32 buffer a is all zero or not
 * parameters:
 *     a -------------------------- input, big integer or word buffer a
 *     aWordLen ------------------- input, word length of a
 * return: 0(a is not zero), 1(a is all zero)
 * caution:
 */
u32 uint32_BigNum_Check_Zero(u32 *a, u32 aWordLen)
{
    u32 i;

    for(i=0; i<aWordLen; i++)
    {
        if(0U != a[i])
        {
            return 0;
        }
        else
        {;}
    }

    return 1;
}


/* function: a = a + b
 * parameters:
 *     a -------------------------- input, big number a, u8 big-endian
 *     a_bytes -------------------- input, byte length of a
 *     b -------------------------- input, u8 integer b
 *     is_secure ------------------ input, is secure implementation, 0(not), other(yes)
 * return: 0(not overflow),1(overflow)
 * caution:
 *     1. this is mainly used for counter++ in SKE, KDF, etc.
 */
u32 uint8_big_num_big_endian_add_little(u8 *a, u32 a_bytes, u8 b, 
        u8 is_secure)
{
    u32 i;
    u32 ret;

    if((u8)0 != is_secure)
    {
        i = a_bytes;
        while(0U != (i--))
        {
            a[i] += b;
            if(a[i] < b)
            {
                b = 1U;
            }
            else
            {
                b = 0U;
            }
        }

        ret = b;
    }
    else
    {
        i = a_bytes;
        while(0U != (i--))
        {
            a[i] += b;
            if(a[i] < b)
            {
                b = 1U;
            }
            else
            {
                break;
            }
        }

        if(i != (u32)(0U - 1U))
        {
            ret = 0U;
        }
        else
        {
            ret = 1U;
        }
    }

    return ret;
}


/* function: a = a + b
 * parameters:
 *     a -------------------------- input, big number a, u32 little-endian
 *     a_words -------------------- input, word length of a
 *     b -------------------------- input, u32 integer b
 *     is_secure ------------------ input, is secure implementation, 0(not), other(yes)
 * return: 0(not overflow),1(overflow)
 * caution:
 *     1. this is mainly used for public key algorithm implementation
 */
u32 uint32_big_num_little_endian_add_little(u32 *a, u32 a_words, u32 b, 
        u8 is_secure)
{
    u32 i;
    u32 ret;

    if((u8)0 != is_secure)
    {
        for(i = 0U; i < a_words; i++)
        {
            a[i] += b;
            if(a[i] < b)
            {
                b = 1U;
            }
            else
            {
                b = 0U;
            }
        }

        ret = b;
    }
    else
    {
        for(i = 0U; i < a_words; i++)
        {
            a[i] += b;
            if(a[i] < b)
            {
                b = 1U;
            }
            else
            {
                break;
            }
        }

        if(i < a_words)
        {
            ret = 0U;
        }
        else
        {
            ret = 1U;
        }
    }

    return ret;
}


/* function: compare big integer a and b
 * parameters:
 *     a -------------------------- input, big integer a
 *     aWordLen ------------------- input, word length of a
 *     b -------------------------- input, big integer b
 *     bWordLen ------------------- input, word length of b
 * return:
 *     0:a=b,   1:a>b,   -1: a<b
 * caution:
 */
s32 uint32_BigNumCmp(u32 *a, u32 aWordLen, u32 *b, u32 bWordLen)
{
    u32 i;

    aWordLen = get_valid_words(a, aWordLen);
    bWordLen = get_valid_words(b, bWordLen);

    if(aWordLen > bWordLen)
    {
        return 1;
    }
    else if(aWordLen < bWordLen)
    {
        return -1;
    }
    else
    {
        //handle other;
    }

    i = aWordLen;
    while(0U != (i--))
    {
        if(a[i] > b[i])
        {
            return 1;
        }
        else if(a[i] < b[i])
        {
            return -1;
        }
        else
        {
            //handle other;
        }
    }

    return 0;
}


/* function: for a = b*2^t, b is odd, get t
 * parameters:
 *     a -------------------------- big integer a
 * return:
 *     number of multiple by 2, for a
 * caution:
 *     1. make sure a != 0
 */
u32 Get_Multiple2_Number(u32 *a)
{
    u32 t, i=0, j=0;

    while(0U == (a[i]))
    {
        i++;
    }

    t = a[i];
    while(0U == (t&1U))
    {
        j++;
        t>>=1;
    }

    return (i<<5)+j;
}


/* function: a = a/(2^n)
 * parameters:
 *     a -------------------------- big integer a
 *     aWordLen ------------------- word length of a
 *     n -------------------------- exponent of 2^n
 * return:
 *     word length of a = a/(2^n)
 * caution:
 *     1. make sure aWordLen is real word length of a
 *     2. please make sure aWordLen*32 is not less than n
 */
u32 Big_Div2n(u32 *a, u32 aWordLen, u32 n)
{
    u32 i;
    u32 j;

    aWordLen = get_valid_words(a, aWordLen);

    if(0U == n)
    {
        return aWordLen;
    }
    else if(0U == aWordLen)
    {
        return 0U;
    }
    else
    {
        //handle other;
    }

    //now a is not zero(aWordLen is not zero), and n is not zero either.

    if(n<32U)
    {
        for(i=0U; i<aWordLen-1U; i++)
        {
            a[i] >>= n;
            a[i] |= (a[i+1U]<<(32U-n));
        }
        a[i] >>= n;

        if(0U == a[i])
        {
            return i;
        }
        else
        {
            return aWordLen;
        }
    }
    else
    {;}

    j = n>>5U; //j=n/32;
    n &= 31U;  //n=n%32;

    if(j < aWordLen)
    {
        if(0U != n)   //n is in [1, 31]
        {
            for(i=0; i<aWordLen-j-1U; i++)
            {
                a[i] = a[i+j]>>n;
                a[i] |= (a[i+j+1U]<<(32U-n));
            }
            a[i] = a[i+j]>>n;
            uint32_clear(&a[aWordLen-j], j);

            if(0U == a[i])
            {
                return i;
            }
            else
            {
                return aWordLen-j;
            }
        }
        else    //n is 0
        {
            for(i=0; i<aWordLen-j; i++)
            {
                a[i] = a[i+j];
            }
            uint32_clear(&a[aWordLen-j], j);

            return aWordLen-j;
        }
    }
    else
    {
        uint32_clear(a, aWordLen);
        return 0U;
    }
}


/* Function: check whether a is equal to 1 or not
 * Parameters:
 *     a ---------------- pointer to u32 big integer a
 *     aWordLen --------- word length of big integer a
 * Return: 1(a is 1), 0(a is not 1)
 * Caution:
 */
u8 Bigint_Check_1(u32 *a, u32 aWordLen)
{
    u32 i;

    if(0U == aWordLen)
    {
        return 0;
    }
    else if(a[0] != 1U)
    {
        return 0;
    }
    else
    {
        //handle other;
    }

    for(i=1; i<aWordLen; i++)
    {
        if(0U != a[i])
        {
            return 0;
        }
        else
        {;}
    }

    return 1;
}


/* function: check whether a is equal to p-1 or not
 * parameters:
 *     a ---------------- pointer to u32 big integer a
 *     p ---------------- pointer to u32 big integer p, p must be odd
 *     wordLen ---------- word length of a and p
 * return: 1(a is p-1), 0(a is not p-1)
 * caution:
 *     1. make sure p is odd
 */
u8 Bigint_Check_p_1(u32 *a, u32 *p, u32 wordLen)
{
    u32 i;

    if(0U == wordLen)
    {
        return 0;
    }
    else if(a[0] != p[0] - 1U)
    {
        return 0;
    }
    else
    {
        //handle other;
    }

    for(i=1; i<wordLen; i++)
    {
        if(a[i] != p[i])
        {
            return 0;
        }
        else
        {;}
    }

    return 1;
}


/* function: check whether integer k is in [1, n-1]
 * parameters:
 *     k -------------------------- input, big number k
 *     n -------------------------- input, big number n
 *     wordLen -------------------- input, word length of k and n
 * return:
 *     ret_zero ------------------- k is zero
 *     ret_big -------------------- k is greater/bigger than or equal to n
 *     ret_success ---------------- k is in [1, n-1]
 * caution:
 *     1.
 */
u32 uint32_integer_check(u32 *k, u32 *n, u32 wordLen, u32 ret_zero, u32 ret_big,
        u32 ret_success)
{
    if(0U != uint32_BigNum_Check_Zero(k, wordLen))
    {
        return ret_zero;
    }
    else if(uint32_BigNumCmp(k, wordLen, n, wordLen) >= 0)
    {
        return ret_big;
    }
    else
    {
        //handle other;
    }

    return ret_success;
}

