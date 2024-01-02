//#include <stdio.h>

#include <aic_core.h>

#ifdef AIC_PKE_SM2_SUPPORT

#include <sm2.h>
#include <trng.h>
#include <utility.h>
#ifdef AIC_PKE_SEC
#include <utility_sec.h>
#endif

#define SM2_DEFAULT_ID_BYTE_LEN         (16)
static char * const g_sm2_default_id = "1234567812345678";

//SM2 algorithm parameters
const u32 sm2p256v1_p[8]    = {0xFFFFFFFF,0xFFFFFFFF,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
const u32 sm2p256v1_p_h[8]  = {0x00000003,0x00000002,0xFFFFFFFF,0x00000002,0x00000001,0x00000001,0x00000002,0x00000004};
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
const u32 sm2p256v1_p_n0[1] = {1,};
#endif
const u32 sm2p256v1_a[8]    = {0xFFFFFFFC,0xFFFFFFFF,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
const u32 sm2p256v1_b[8]    = {0x4D940E93,0xDDBCBD41,0x15AB8F92,0xF39789F5,0xCF6509A7,0x4D5A9E4B,0x9D9F5E34,0x28E9FA9E};
const u32 sm2p256v1_Gx[8]   = {0x334C74C7,0x715A4589,0xF2660BE1,0x8FE30BBF,0x6A39C994,0x5F990446,0x1F198119,0x32C4AE2C};
const u32 sm2p256v1_Gy[8]   = {0x2139F0A0,0x02DF32E5,0xC62A4740,0xD0A9877C,0x6B692153,0x59BDCEE3,0xF4F6779C,0xBC3736A2};
const u32 sm2p256v1_n[8]    = {0x39D54123,0x53BBF409,0x21C6052B,0x7203DF6B,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
const u32 sm2p256v1_n_h[8]  = {0x7C114F20,0x901192AF,0xDE6FA2FA,0x3464504A,0x3AFFE0D4,0x620FC84C,0xA22B3D3B,0x1EB5E412};
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
const u32 sm2p256v1_n_n0[1] = {0x72350975,};
#endif

//SM2 para (n-1), for private key checking
const u32 sm2p256v1_n_1[8]  = {0x39D54122,0x53BBF409,0x21C6052B,0x7203DF6B,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};

//[2^128]G, for [k]G of high speed
#if !(defined(PKE_LP) || defined(AIC_PKE_SECURE))
const u32 sm2p256v1_2_128_G_x[8] = {0xD13A42ED,0xEAE3D9A9,0x484E1B38,0x2B2308F6,0x88C21F3A,0x3DB7B248,0x74D55DA9,0xB692E5B5};
const u32 sm2p256v1_2_128_G_y[8] = {0xE295E5AB,0xD186469D,0x73438E6D,0xDB61AC17,0x544926F9,0x5A924F85,0x0F3FB613,0xA175051B};
#endif

const eccp_curve_t sm2_curve[1] = {
    {
        256,
        256,
        (u32 *)sm2p256v1_p,
        (u32 *)sm2p256v1_p_h,
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
        (u32 *)sm2p256v1_p_n0,
#endif
        (u32 *)sm2p256v1_a,
        (u32 *)sm2p256v1_b,
        (u32 *)sm2p256v1_Gx,
        (u32 *)sm2p256v1_Gy,
        (u32 *)sm2p256v1_n,
        (u32 *)sm2p256v1_n_h,
#if (defined(PKE_LP) || defined(AIC_PKE_SECURE))
        (u32 *)sm2p256v1_n_n0,
#else
        (u32 *)sm2p256v1_2_128_G_x,
        (u32 *)sm2p256v1_2_128_G_y,
#endif
    }
};

/* function: get SM2 Z value = SM3(bitLenofID||ID||a||b||Gx||Gy||Px||Py)
 * parameters:
 *     ID ------------------------- input, User ID
 *     byteLenofID ---------------- input, byte length of ID, must be less than 2^13
 *     pubKey --------------------- input, public key(0x04 + x + y), 65 bytes, big-endian
 *     Z -------------------------- output, Z value, SM3 digest, 32 bytes
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. bit length of ID must be less than 2^16, thus byte length must be less than 2^13
 *     2. if ID is NULL, then replace it with sm2 default ID
 *     3. please make sure the pubKey is valid
 */
u32 sm2_getZ(u8 *ID, u32 byteLenofID, u8 pubKey[65], u8 Z[32])
{
    u32 tmp[SM2_WORD_LEN];
    u32 tmp2[SM2_WORD_LEN];
    HASH_CTX ctx[1];
    u32 ret;
    u8 tmp_u8;

    if(NULL == pubKey || NULL == Z)
    {
        return SM2_BUFFER_NULL;
    }
    else if(POINT_UNCOMPRESSED != pubKey[0])
    {
        return SM2_INPUT_INVALID;
    }
    else if(byteLenofID >= SM2_MAX_ID_BYTE_LEN)
    {
        return SM2_INPUT_INVALID;
    }
    else if((NULL == ID) || (0 == byteLenofID))
    {
        ID = (u8 *)g_sm2_default_id;
        byteLenofID = SM2_DEFAULT_ID_BYTE_LEN;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array(pubKey+1, tmp, SM2_WORD_LEN);
    reverse_word_array(pubKey+1+SM2_BYTE_LEN, tmp2, SM2_WORD_LEN);
#else
    reverse_byte_array(pubKey+1, (u8 *)(tmp), SM2_BYTE_LEN);
    reverse_byte_array(pubKey+1+SM2_BYTE_LEN, (u8 *)(tmp2), SM2_BYTE_LEN);
#endif

    ret = eccp_pointVerify((eccp_curve_t *)sm2_curve, tmp, tmp2);
    if(PKE_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_init(ctx, HASH_SM3);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    byteLenofID <<= 3;
    tmp_u8 = (byteLenofID>>8) & 0xFF;
    ret = hash_update(ctx, (u8 *)&tmp_u8, 1);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    tmp_u8 = byteLenofID & 0xFF;
    ret = hash_update(ctx, (u8 *)&tmp_u8, 1);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    byteLenofID >>= 3;
    ret = hash_update(ctx, ID, byteLenofID);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)sm2p256v1_a, tmp, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)sm2p256v1_a, (u8 *)tmp, SM2_BYTE_LEN);
#endif

    ret = hash_update(ctx, (u8 *)tmp, SM2_BYTE_LEN);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)sm2p256v1_b, tmp, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)sm2p256v1_b, (u8 *)tmp, SM2_BYTE_LEN);
#endif

    ret = hash_update(ctx, (u8 *)tmp, SM2_BYTE_LEN);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)sm2p256v1_Gx, tmp, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)sm2p256v1_Gx, (u8 *)tmp, SM2_BYTE_LEN);
#endif

    ret = hash_update(ctx, (u8 *)tmp, SM2_BYTE_LEN);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)sm2p256v1_Gy, tmp, SM2_WORD_LEN);
#else
    reverse_byte_array((u8 *)sm2p256v1_Gy, (u8 *)tmp, SM2_BYTE_LEN);
#endif

    ret = hash_update(ctx, (u8 *)tmp, SM2_BYTE_LEN);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx, pubKey+1, SM2_BYTE_LEN<<1);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_final(ctx, Z);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}


/* function: get SM2 E value = SM3(Z||M) (one-off style)
 * parameters:
 *     M      --------------------- input, Message
 *     byteLen -------------------- input, byte length of M
 *     Z      --------------------- input, Z value, 32 bytes
 *     E      --------------------- output, E value, 32 bytes
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u32 sm2_getE(u8 *M, u32 byteLen, u8 Z[32], u8 E[32])
{
    HASH_CTX ctx[1];
    u32 ret;

    if(NULL == M || NULL == Z || NULL == E)
    {
        return SM2_BUFFER_NULL;
    }
    else if(0 == byteLen)
    {
        return SM2_INPUT_INVALID;
    }
    else
    {;}

    ret = hash_init(ctx, HASH_SM3);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx, Z, 32);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx, M, byteLen);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_final(ctx, E);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}


#ifdef SM2_GETE_BY_STEPS
/* function: step 1 of getting SM2 E value(stepwise style), init
 * parameters:
 *     ctx ------------------------ input, HASH_CTX context pointer
 *     Z -------------------------- input, Z value, 32 bytes
 * return: SM2_SUCCESS(success), other(error)
 * caution:
 *     1.
 */
u32 sm2_getE_init(HASH_CTX *ctx, u8 Z[32])
{
    u32 ret;

    ret = hash_init(ctx, HASH_SM3);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx, Z, 32);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}


/* function: step 2 of getting SM2 E value(stepwise style), update message
 * parameters:
 *     ctx ------------------------ input, HASH_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 * return: SM2_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
u32 sm2_getE_update(HASH_CTX *ctx, u8 *msg, u32 msg_bytes)
{
    u32 ret;

    ret = hash_update(ctx, msg, msg_bytes);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}


/* function: step 3 of getting SM2 E value(stepwise style), message update done, get the digest(SM2 E value)
 * parameters:
 *     ctx ------------------------ input, HASH_CTX context pointer
 *     E -------------------------- output, hash digest, SM2 E value
 * return: SM2_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the ctx is valid and initialized
 *     2. please make sure the digest buffer E is sufficient
 */
u32 sm2_getE_final(HASH_CTX *ctx, u8 E[32])
{
    u32 ret;

    ret = hash_final(ctx, E);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = SM2_SUCCESS;

END:

    return ret;
}
#endif


/* function: Generate SM2 public key from private key
 * parameters:
 *     priKey --------------------- input, private key, 32 bytes, big-endian
 *     pubKey --------------------- output, public key(0x04 + x + y), 65 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u32 sm2_get_pubkey_from_prikey(u8 priKey[32], u8 pubKey[65])
{
    u32 ret;

    if(NULL == priKey || NULL == pubKey)
    {
        return SM2_BUFFER_NULL;
    }
    else
    {;}

    ret = eccp_get_pubkey_from_prikey((eccp_curve_t *)sm2_curve, priKey, pubKey+1);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {
        pubKey[0] = POINT_UNCOMPRESSED;

        return SM2_SUCCESS;
    }
}


/* function: Generate SM2 random Key pair
 * parameters:
 *     priKey --------------------- output, private key, 32 bytes, big-endian
 *     pubKey --------------------- output, public key(0x04 + x + y), 65 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u32 sm2_getkey(u8 priKey[32], u8 pubKey[65])
{
    u32 ret;

#if 1
    if(NULL == priKey || NULL == pubKey)
    {
        return SM2_BUFFER_NULL;
    }
    else
    {;}

    ret = eccp_getkey((eccp_curve_t *)sm2_curve, priKey, pubKey+1);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {
        pubKey[0] = POINT_UNCOMPRESSED;

        return SM2_SUCCESS;
    }
#else

    u32 k[SM2_WORD_LEN], tmp[SM2_WORD_LEN<<1];

    if(NULL == priKey || NULL == pubKey)
    {
        return SM2_BUFFER_NULL;
    }
    else
    {;}

SM2_GETKEY_LOOP:

    ret = get_rand((u8 *)k, SM2_BYTE_LEN);
    if(TRNG_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //make sure priKey in [1, n-2]
    if(uint32_BigNum_Check_Zero(k, SM2_WORD_LEN))
    {
        goto SM2_GETKEY_LOOP;
    }
    else if(uint32_BigNumCmp(k, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
    {
        goto SM2_GETKEY_LOOP;
    }
    else
    {;}

#ifdef SM2_HIGH_SPEED
    ret = eccp_pointMul_base((eccp_curve_t *)sm2_curve, k, tmp, tmp+SM2_WORD_LEN);
#else
    ret = eccp_pointMul((eccp_curve_t *)sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp, tmp+SM2_WORD_LEN);
#endif
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    pubKey[0] = POINT_UNCOMPRESSED;
#ifdef PKE_BIG_ENDIAN
    reverse_word_array((u8 *)k, k, SM2_WORD_LEN);
    memcpy_(priKey, k, SM2_BYTE_LEN);
    reverse_word_array((u8 *)tmp, k, SM2_WORD_LEN);
    memcpy_(pubKey+1, k, SM2_BYTE_LEN);
    reverse_word_array((u8 *)(tmp+SM2_WORD_LEN), k, SM2_WORD_LEN);
    memcpy_(pubKey+1+SM2_BYTE_LEN, k, SM2_BYTE_LEN);
#else
    reverse_byte_array((u8 *)k, priKey, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)tmp, pubKey+1, SM2_BYTE_LEN);
    reverse_byte_array((u8 *)(tmp+SM2_WORD_LEN), pubKey+1+SM2_BYTE_LEN, SM2_BYTE_LEN);
#endif
    return SM2_SUCCESS;
#endif
}


#endif

