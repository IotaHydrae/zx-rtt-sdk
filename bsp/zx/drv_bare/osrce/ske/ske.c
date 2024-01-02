#include <stdio.h>
#include <string.h>
#include <utility.h>
#include <ske.h>
#include <ske_secure_port.h>

/* function: compare buf1 and buf2
 * parameters:
 *     buf1 ----------------------- input, word buffer 1
 *     buf2 ----------------------- input, word buffer 2
 *     wordLen -------------------- input, word length of buffer 1 and 2
 * return: 
 *     0:buf1 == buf2,   other:buf1 != buf2
 * caution: 
 */
#ifdef CONFIG_SKE_SUPPORT_RESIST_FAULT_INJECTION
static u32 ske_sec_uint32_memcmp(volatile u32 *buf1, volatile u32 *buf2, u32 wordLen)
{
    volatile int32_t i;
    u32 tmp;
    u32 a[4]={0};

    for(i=wordLen-1; i>=0; i--)
    {
        tmp = (buf1[i] & buf2[i]);
        if(tmp < buf1[i] || tmp < buf2[i])
        {
            a[0] |= 0x5acb793d;
        }
        else
        {;}

        a[1] |= (buf1[i] ^ buf2[i]);

        a[2] |= (buf1[i] - buf2[i]);

        tmp = (buf1[i] | buf2[i]);
        if(tmp > buf1[i] || tmp > buf2[i])
        {
            a[3] |= 0x5acb793d;
        }
        else
        {;}
    }

    if(i != -1)
    {
        return 0x1234abcd;
    }
    else
    {;}

    return (a[0] | a[1] | a[2] | a[3]);
}
#endif

#if 1
/* function: a=a+1
 * parameters:
 *     a -------------------------- input, big integer a in bytes, big-endian
 *     bytes ---------------------- input, byte length of a
 * return: none
 * caution:
 *     1. for CTR/CCM counter addition(big-endian)
 *     2. if a overflow, the high carry will not be kept,
 *        actually, for ccm, the counter never overflows
 */
void ske_big_endian_add_uint8(u8 *a, u32 a_bytes, u8 b)
{
    int32_t i;

    for(i=a_bytes; i>0; )
    {
        a[--i] += b;
        if(a[i] < b)
        {
            b = 1;
        }
        else
        {
#if 1
            b = 0;   //for security
#else
            break;
#endif
        }
    }
}
#else
/* function: a=a+1
 * parameters:
 *     a -------------------------- input, big integer a in bytes, big-endian
 *     bytes ---------------------- input, byte length of a
 * return: none
 * caution:
 *     1. for CTR/CCM counter addition(little-endian)
 *     2. if a overflow, the high carry will not be kept, actually, for ccm, 
 *        the counter never overflows
 */
void ske_little_endian_add_uint32(u32 *a, u32 a_words, u32 b)
{
    int32_t i;

    for(i=a_words; i>0; )
    {
        a[--i] += b;
        if(a[i] < b)
        {
            b = 1;
        }
        else
        {
#if 0
            b = 0;   //for security
#else
            break;
#endif
        }
    }
}
#endif

/* function: check whether the ske algorithm is valid or not
 * parameters:
 *     ske_alg -------------------- input, specific ske algorithm
 * return: SKE_SUCCESS(valid), other(invalid)
 * caution:
 *     1.
 */
u8 ske_check_alg(SKE_ALG ske_alg)
{
    u8 ret;

    switch(ske_alg)
    {
#ifdef AIC_SKE_DES_SUPPORT
    case SKE_ALG_DES:
#endif

#ifdef AIC_SKE_TDES_128_SUPPORT
    case SKE_ALG_TDES_128:
#endif

#ifdef AIC_SKE_TDES_192_SUPPORT
    case SKE_ALG_TDES_192:
#endif

#ifdef SUPPORT_SKE_TDES_EEE_128
    case SKE_ALG_TDES_EEE_128:
#endif

#ifdef SUPPORT_SKE_TDES_EEE_192
    case SKE_ALG_TDES_EEE_192:
#endif

#ifdef AIC_SKE_AES_128_SUPPORT
    case SKE_ALG_AES_128:
#endif

#ifdef AIC_SKE_AES_192_SUPPORT
    case SKE_ALG_AES_192:
#endif

#ifdef AIC_SKE_AES_256_SUPPORT
    case SKE_ALG_AES_256:
#endif

#ifdef AIC_SKE_SM4_SUPPORT
    case SKE_ALG_SM4:
#endif
        ret = SKE_SUCCESS;
        break;

    default:
        ret = SKE_INPUT_INVALID;
        break;
    }

    return ret;
}


/* function: check whether the ske algorithm mode is valid or not
 * parameters:
 *     ske_alg -------------------- input, specific ske algorithm
 *     ske_mode ------------------- input, specific ske algorithm mode
 * return: SKE_SUCCESS(valid), other(invalid)
 * caution:
 *     1.
 */
u8 ske_check_mode(SKE_ALG ske_alg, SKE_MODE ske_mode)
{
    u8 ret;

    switch(ske_mode)
    {
#ifdef AIC_SKE_MODE_ECB_SUPPORT
    case SKE_MODE_ECB:
#endif

#ifdef AIC_SKE_MODE_CBC_SUPPORT
    case SKE_MODE_CBC:
#endif

#ifdef AIC_SKE_MODE_CFB_SUPPORT
    case SKE_MODE_CFB:
#endif

#ifdef AIC_SKE_MODE_OFB_SUPPORT
    case SKE_MODE_OFB:
#endif

#ifdef AIC_SKE_MODE_CTR_SUPPORT
    case SKE_MODE_CTR:
#endif

#ifdef AIC_SKE_MODE_CBC_SUPPORT_MAC
    case SKE_MODE_CBC_MAC:
#endif
        ret = SKE_SUCCESS;
        break;

    //for DES/3DES, CAMC is not supported at present
#ifdef SUPPORT_SKE_MODE_CMAC
    case SKE_MODE_CMAC:
        switch(ske_alg)
        {
#ifdef AIC_SKE_AES_128_SUPPORT
        case SKE_ALG_AES_128 :
#endif

#ifdef AIC_SKE_AES_192_SUPPORT
        case SKE_ALG_AES_192 :
#endif

#ifdef AIC_SKE_AES_256_SUPPORT
        case SKE_ALG_AES_256 :
#endif

#ifdef AIC_SKE_SM4_SUPPORT
        case SKE_ALG_SM4 :
#endif

#if (defined(AIC_SKE_AES_128_SUPPORT) || defined(AIC_SKE_AES_192_SUPPORT) || defined(AIC_SKE_AES_256_SUPPORT) || defined(AIC_SKE_SM4_SUPPORT))
            ret = SKE_SUCCESS;
            break;
#endif

        default:
            ret = SKE_INPUT_INVALID;
        }
        break;
#endif

    //for DES/3DES, XTS, CCM and GCM mode are not supported due to the definition or standard
#ifdef SUPPORT_SKE_MODE_XTS
    case SKE_MODE_XTS:
#endif

#ifdef SUPPORT_SKE_MODE_CCM
    case SKE_MODE_CCM:
#endif

#ifdef SUPPORT_SKE_MODE_GCM
    case SKE_MODE_GCM:
#endif

#if (defined(SUPPORT_SKE_MODE_XTS) || defined(SUPPORT_SKE_MODE_CCM) || defined(SUPPORT_SKE_MODE_GCM))
        switch(ske_alg)
        {
#ifdef AIC_SKE_AES_128_SUPPORT
        case SKE_ALG_AES_128 :
#endif

#ifdef AIC_SKE_AES_192_SUPPORT
        case SKE_ALG_AES_192 :
#endif

#ifdef AIC_SKE_AES_256_SUPPORT
        case SKE_ALG_AES_256 :
#endif

#ifdef AIC_SKE_SM4_SUPPORT
        case SKE_ALG_SM4 :
#endif

#if (defined(AIC_SKE_AES_128_SUPPORT) || defined(AIC_SKE_AES_192_SUPPORT) || defined(AIC_SKE_AES_256_SUPPORT) || defined(AIC_SKE_SM4_SUPPORT))
            ret = SKE_SUCCESS;
            break;
#endif

        default:
            ret = SKE_INPUT_INVALID;
            break;
        }

        break;
#endif

    default:
        ret = SKE_INPUT_INVALID;
        break;
    }

    return ret;
}


/* function: get block byte length for spcific ske alg
 * parameters:
 *     ske_alg -------------------- input, ske algorithm
 * return: block byte length for ske alg
 * caution: 
 *     1. please make sure ske_alg is valid
 */
u8 ske_sec_get_block_byte_len(SKE_ALG ske_alg)
{
    u8 byteLen;

    switch(ske_alg)
    {
#ifdef AIC_SKE_DES_SUPPORT
    case SKE_ALG_DES :
#endif

#ifdef AIC_SKE_TDES_128_SUPPORT
    case SKE_ALG_TDES_128 :
#endif

#ifdef AIC_SKE_TDES_192_SUPPORT
    case SKE_ALG_TDES_192 :
#endif

#ifdef SUPPORT_SKE_TDES_EEE_128
    case SKE_ALG_TDES_EEE_128 :
#endif

#ifdef SUPPORT_SKE_TDES_EEE_192
    case SKE_ALG_TDES_EEE_192 :
#endif

#if (defined(AIC_SKE_DES_SUPPORT) ||defined(AIC_SKE_TDES_128_SUPPORT) ||defined(AIC_SKE_TDES_192_SUPPORT)   \
    ||defined(SUPPORT_SKE_TDES_EEE_128) ||defined(SUPPORT_SKE_TDES_EEE_192))
        byteLen = 8;
        break;
#endif

#ifdef AIC_SKE_AES_128_SUPPORT
    case SKE_ALG_AES_128 :
#endif

#ifdef AIC_SKE_AES_192_SUPPORT
    case SKE_ALG_AES_192 :
#endif

#ifdef AIC_SKE_AES_256_SUPPORT
    case SKE_ALG_AES_256 :
#endif

#ifdef AIC_SKE_SM4_SUPPORT
    case SKE_ALG_SM4 :
#endif

#if (defined(AIC_SKE_AES_128_SUPPORT) ||defined(AIC_SKE_AES_192_SUPPORT) ||defined(AIC_SKE_AES_256_SUPPORT) ||defined(AIC_SKE_SM4_SUPPORT))
        byteLen = 16;
        break;
#endif

    default:
        byteLen = 16;   //default alg SM4
    }

    return byteLen;
}


/* function: get key byte length for spcific ske alg
 * parameters:
 *     ske_alg -------------------- input, ske algorithm
 * return: key byte length for ske alg
 * caution: 
 *     1. please make sure ske_alg is valid
 */
u8 ske_sec_get_key_byte_len(SKE_ALG ske_alg)
{
    u8 byte_len;

    switch(ske_alg)
    {
#ifdef AIC_SKE_DES_SUPPORT
    case SKE_ALG_DES :
        byte_len = 8;
        break;
#endif

#ifdef AIC_SKE_TDES_128_SUPPORT
    case SKE_ALG_TDES_128 :
#endif

#ifdef SUPPORT_SKE_TDES_EEE_128
    case SKE_ALG_TDES_EEE_128 :
#endif

#ifdef AIC_SKE_AES_128_SUPPORT
    case SKE_ALG_AES_128 :
#endif

#ifdef AIC_SKE_SM4_SUPPORT
    case SKE_ALG_SM4 :
#endif

#if (defined(AIC_SKE_TDES_128_SUPPORT) || defined(SUPPORT_SKE_TDES_EEE_128) || defined(AIC_SKE_AES_128_SUPPORT) \
    ||defined(AIC_SKE_SM4_SUPPORT))
        byte_len = 16;
        break;
#endif

#ifdef AIC_SKE_TDES_192_SUPPORT
    case SKE_ALG_TDES_192 :
#endif

#ifdef SUPPORT_SKE_TDES_EEE_192
    case SKE_ALG_TDES_EEE_192 :
#endif

#ifdef AIC_SKE_AES_192_SUPPORT
    case SKE_ALG_AES_192 :
#endif

#if (defined(AIC_SKE_TDES_192_SUPPORT) || defined(SUPPORT_SKE_TDES_EEE_192) || defined(AIC_SKE_AES_192_SUPPORT))
        byte_len = 24;
        break;
#endif

#ifdef AIC_SKE_AES_256_SUPPORT
    case SKE_ALG_AES_256 :
        byte_len = 32;
        break;
#endif

    default:
        byte_len = 16;   //default alg SM4
    }

    return byte_len;
}


/* function: set ske key
 * parameters:
 *     alg ------------------------ input, ske algorithm
 *     key ------------------------ input, key
 *     key_bytes ------------------ input, byte length of key
 *     key_idx -------------------- input, key index, only 1 and 2 are valid
 * return: none
 * caution:
 *     1. please make sure the inputs are valid
 */
void ske_set_key(SKE_ALG alg, u8 *key, u16 key_bytes, u16 key_idx)
{
    u32 tmp[8];

    memcpy((u8 *)tmp, key, key_bytes);

    //for 3DES-2key, set key3=key1
    switch(alg)
    {
#ifdef AIC_SKE_TDES_128_SUPPORT
    case SKE_ALG_TDES_128 :
#endif

#ifdef SUPPORT_SKE_TDES_EEE_128
    case SKE_ALG_TDES_EEE_128 :
#endif

#if (defined(AIC_SKE_TDES_128_SUPPORT) || defined(SUPPORT_SKE_TDES_EEE_128))
        memcpy((u8 *)(tmp+4), key, 8);
        key_bytes += 8;
        break;
#endif

    default:
        break;
    }

#ifndef SKE_CPU_BIG_ENDIAN
    uint32_endian_reverse((u8 *)tmp, (u8 *)tmp, key_bytes>>2);
#endif

    ske_set_key_uint32(tmp, key_idx, key_bytes/4);
}


/* function: get the last plaintext block after padding
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 *     in ------------------------- input and output, last plaintext before padding and last plaintext after padding
 *     in_bytes ------------------- input, valid plaintext byte length of the last plaintext before padding
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 */
void ske_sec_get_padding_block(SKE_CTX *ctx, u8 *in , u32 in_bytes)
{
    u32 fill_bytes = ctx->block_bytes - in_bytes;

    if(SKE_ANSI_X923_PADDING == ctx->padding)
    {
        memset(in+in_bytes, 0, fill_bytes-1);
        in[ctx->block_bytes-1] = fill_bytes;
    }
    else if(SKE_PKCS_5_7_PADDING == ctx->padding)
    {
        memset(in+in_bytes, fill_bytes, fill_bytes);
    }
    else if(SKE_ISO_7816_4_PADDING == ctx->padding)
    {
        in[in_bytes] = 0x80;
        memset(in+in_bytes+1, 0, fill_bytes - 1);
    }
    else if(SKE_ZERO_PADDING == ctx->padding)
    {
        if(in_bytes)
        {
            memset(in+in_bytes, 0, fill_bytes);
        }
        else
        {;}
    }
    else
    {;}
}


/* function: check the last plaintext block after padding
 * parameters:
 *     block ---------------------- input, last plaintext block after padding
 *     block_bytes ---------------- input, block byte length
 *     padding -------------------- input, padding scheme, must be SKE_ANSI_X923_PADDING, or SKE_PKCS_5_7_PADDING, 
 *                                  or SKE_ISO_7816_4_PADDING
 *     valid_bytes ---------------- output, valid plaintext byte length of the last block after padding
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. 
 */
u32 ske_sec_check_padding(u8 *block, u32 block_bytes, SKE_PADDING padding, u32 *valid_bytes)
{
    int32_t i, idx;
    u32 padding_bytes;

    if(SKE_ANSI_X923_PADDING == padding)
    {
        i = block_bytes - 1;
        padding_bytes = block[i];
        if((0 == padding_bytes) || (padding_bytes > block_bytes))
        {
            return SKE_PADDING_ERROR;
        }
        else
        {;}

        idx = block_bytes - padding_bytes - 1;
        --i;
        for(; i>idx; i--)
        {
            if(block[i])
            {
                return SKE_PADDING_ERROR;
            }
            else
            {;}
        }

        *valid_bytes = idx + 1;
    }
    else if(SKE_PKCS_5_7_PADDING == padding)
    {
        i = block_bytes - 1;
        padding_bytes = block[i];
        if((0 == padding_bytes) || (padding_bytes > block_bytes))
        {
            return SKE_PADDING_ERROR;
        }
        else
        {;}

        idx = block_bytes - padding_bytes - 1;
        --i;
        for(; i>idx; i--)
        {
            if(block[i] != padding_bytes)
            {
                return SKE_PADDING_ERROR;
            }
            else
            {;}
        }

        *valid_bytes = idx + 1;
    }
    else if(SKE_ISO_7816_4_PADDING == padding)
    {
        i = block_bytes;
        while(--i)
        {
            if(0 != block[i])
            {
                break;
            }
            else
            {;}
        }

        if(0x80 != block[i])
        {
            return SKE_PADDING_ERROR;
        }
        else
        {;}

        *valid_bytes = (u32)i;
    }

    return SKE_SUCCESS;
}


#if (defined(CONFIG_SKE_SUPPORT_MUL_THREAD))
//keep alg,mode,key(sp_key_idx),iv
u32 keep_alg_key_iv(SKE_CTX *ctx, SKE_ALG alg, SKE_MODE mode, u8 *key, u16 sp_key_idx, u8 *iv)
{
    if(NULL == ctx)
    {
        return SKE_BUFFER_NULL;
    }
    else
    {;}

    ctx->alg = alg;
    ctx->mode = mode;
    ctx->block_bytes = ske_sec_get_block_byte_len(alg);
    ctx->block_words = (ctx->block_bytes)/((u8)4);

#ifdef SUPPORT_SKE_MODE_BYPASS
    if((SKE_MODE_BYPASS != mode) && (SKE_MODE_ECB != mode))
#else
    if(SKE_MODE_ECB != mode)
#endif
    {
        if(NULL == iv)
        {
            return SKE_BUFFER_NULL;
        }
        else
        {;}

        memcpy((u8 *)ctx->iv, iv, ctx->block_bytes);
    }
    else
    {;}

    if(NULL != key)
    {
        ctx->key = (u8 *)(ctx->key_buf);
        memcpy(ctx->key, key, ske_sec_get_key_byte_len(alg));
    }
    else
    {
        ctx->key        = NULL;
        ctx->sp_key_idx = sp_key_idx;
    }

    return SKE_SUCCESS;
}
#endif


/* function: ske init config(internal)
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 *     alg ------------------------ input, ske algorithm
 *     mode ----------------------- input, ske algorithm operation mode, like ECB,CBC,OFB,etc.
 *     crypto --------------------- input, encrypting or decrypting
 *     key ------------------------ input, key in bytes
 *     sp_key_idx ----------------- input, index of secure port key, (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX],
 *                                  if the MSB(sp_key_idx) is 1, that means using low 128bit of the 256bit key
 *     iv ------------------------- input, iv in bytes, must be a block
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. if mode is ECB, then there is no iv, in this case iv could be NULL
 *     2. this function is designed for ECB/CBC/CFB/OFB/CTR modes, and input/output unit must be a block
 *     3. if key is from user input, please make sure key is not NULL(now sp_key_idx is useless),
 *        otherwise, key is from secure port, and (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX]
 */
u32 ske_sec_init_internal(SKE_CTX *ctx, SKE_ALG alg, SKE_MODE mode, SKE_CRYPTO crypto, u8 *key, 
        u16 sp_key_idx, u8 *iv)
{
    u32 key_bytes;

    if(SKE_SUCCESS != ske_check_alg(alg))
    {
        return SKE_ERROR;
    }
    else if(SKE_SUCCESS != ske_check_mode(alg, mode))
    {
        return SKE_ERROR;
    }
    else if(crypto > SKE_CRYPTO_DECRYPT)
    {
        return SKE_ERROR;
    }
    else if(NULL == key)   //secure port     //key idx is from 1 to SKE_MAX_KEY_IDX
    {
#ifdef SKE_SECURE_PORT_FUNCTION
        /*  TODO
        if((SKE_ALG_SM4 != alg) || (sp_key_idx > 3))
        {
            return SKE_ERROR;//SKE_BUFFER_NULL;
        }*/
#else
        return SKE_ERROR;
#endif
    }
    else
    {;}

    if(SKE_MODE_ECB == mode)
    {
        iv = NULL;
    }
    else if(NULL == iv)
    {
        return SKE_ERROR;//
    }
    else
    {;}

    //reset SKE
    ske_reset();

    //check and clear runtime alarm state
    if(SKE_ERROR == ske_check_runtime_alarm())
    {
        return SKE_ERROR;
    }
    else
    {;}

    //keep crypto
    ctx->crypto = crypto;

    //keep mode
    ctx->mode = mode;

    //keep the block length
    ctx->block_bytes = ske_sec_get_block_byte_len(alg);
    ctx->block_words = ctx->block_bytes>>2;

    //config and check
    ske_set_endian_uint32();
    ske_set_alg(alg);
    ske_set_mode(mode);
    ske_set_crypto(crypto);

    if(ske_check_config())
    {
        return SKE_ERROR;//SKE_CONFIG_INVALID;
    }
    else
    {;}

    //keep and set iv
    if(NULL != iv)
    {
        memcpy((u8 *)ctx->iv, iv, ctx->block_bytes);
        ske_set_iv_uint32(mode, ctx->iv, ctx->block_words);
    }
    else
    {;}

    //set key
    if(NULL != key)    //key is from user input
    {
        ske_sec_disable_secure_port();

        key_bytes = ske_sec_get_key_byte_len(alg);
        ske_set_key(alg, key, key_bytes, 1);
    }
    else
    {
        //TODO

        ske_sec_enable_secure_port(sp_key_idx);   //must be called here
    }

    return SKE_SUCCESS;
}


/* function: ske init config
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 *     alg ------------------------ input, ske algorithm
 *     mode ----------------------- input, ske algorithm operation mode, like ECB,CBC,OFB,etc.
 *     crypto --------------------- input, encrypting or decrypting
 *     key ------------------------ input, key in bytes
 *     sp_key_idx ----------------- input, index of secure port key, (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX],
 *                                  if the MSB(sp_key_idx) is 1, that means using low 128bit of the 256bit key
 *     iv ------------------------- input, iv in bytes, must be a block
 *     padding -------------------- input, padding scheme, should be SKE_NO_PADDING/SKE_ANSI_X923_PADDING/SKE_PKCS_5_7_PADDING/
 *                                         SKE_ISO_7816_4_PADDING
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. if mode is ECB, then there is no iv, in this case iv could be NULL
 *     2. this function is designed for ECB/CBC/CFB/OFB/CTR modes, and input/output unit must be a block
 *     3. if key is from user input, please make sure key is not NULL(now sp_key_idx is useless),
 *        otherwise, key is from secure port, and (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX]
 */
u32 ske_sec_init(SKE_CTX *ctx, SKE_ALG alg, SKE_MODE mode, SKE_CRYPTO crypto, u8 *key, 
        u16 sp_key_idx, u8 *iv, SKE_PADDING padding)
{
    if (SKE_ISO_7816_4_PADDING < padding)
    {
        return SKE_INPUT_INVALID;
    }

    ctx->padding = padding;

#if defined(CONFIG_SKE_SUPPORT_MUL_THREAD)
    ctx->crypto = crypto;
    
    return keep_alg_key_iv(ctx, alg, mode, key, sp_key_idx, iv);
#else
    return ske_sec_init_internal(ctx, alg, mode, crypto, key, sp_key_idx, iv);
#endif
}


/* function: ske encryption or decryption(CPU style)
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 *     in ------------------------- input, plaintext or ciphertext
 *     out ------------------------ output, ciphertext or plaintext
 *     bytes ---------------------- input, byte length of input or output.
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. this function is designed for ECB/CBC/CFB/OFB/CTR modes, and input/output unit must be a block
 *     2. to save memory, in and out could be the same buffer, in this case, the output will
 *        cover the input.
 *     3. bytes must be a multiple of block byte length.
 */
u32 ske_sec_update_blocks(SKE_CTX *ctx, u8 *in, u8 *out, u32 bytes)
{
    u32 tmp_in[4], tmp_out[4], tmp_out2[4];
    u32 tmp_iv[4];
    u8 *p_out = out;
    u32 i,j;
    u32 ret;

    if(NULL == in)
    {
        return SKE_ERROR;//SKE_BUFFER_NULL;
    }
    else if(bytes & (ctx->block_bytes-1))
    {
        return SKE_ERROR;//SKE_INPUT_INVALID;
    }
    else if (0 == bytes)
    {
        return SKE_SUCCESS;
    }
    else
    {;}

#if defined(CONFIG_SKE_SUPPORT_MUL_THREAD)
    ret = ske_sec_init_internal(ctx, ctx->alg, ctx->mode, ctx->crypto, ctx->key, ctx->sp_key_idx, (u8 *)ctx->iv);
    if(SKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}
#endif

    //input one block ---> calculating ---> output one block
    for (i = 0; i < bytes; i += ctx->block_bytes)
    {
#if ((!defined(CONFIG_SKE_SUPPORT_MUL_THREAD)) && (!defined(CONFIG_SKE_SUPPORT_RESIST_FAULT_INJECTION)))
        memcpy((u8 *)tmp_in, in, ctx->block_bytes);
        ske_simple_set_input_block(tmp_in, ctx->block_words);

        ske_start();
        ret = ske_wait_till_done();
        if(SKE_SUCCESS != ret)
        {
            ske_reset();
            ret = SKE_ERROR;
            goto END;
        }
        else
        {;}

        ske_simple_get_output_block((u32 *)tmp_out, ctx->block_words);
#else 
        /************* first **************/
        //set iv
        ske_set_iv_uint32(ctx->mode, ctx->iv, ctx->block_words);

        memcpy((u8 *)tmp_in, in, ctx->block_bytes);
        ske_simple_set_input_block(tmp_in, ctx->block_words);

        ske_start();
        ret = ske_wait_till_done();
        if(SKE_SUCCESS != ret)
        {
            ske_reset();
            ret = SKE_ERROR;
            goto END;
        }
        else
        {;}

        ske_simple_get_output_block((u32 *)tmp_out, ctx->block_words);

        /************* keep iv **************/
        if((SKE_MODE_CBC == ctx->mode) || (SKE_MODE_CFB == ctx->mode))
        {
            if(SKE_CRYPTO_ENCRYPT == ctx->crypto)
            {
                uint32_copy(tmp_iv, tmp_out, ctx->block_words);
            }
            else
            {
                uint32_copy(tmp_iv, tmp_in, ctx->block_words);
            }
        }
        else if(SKE_MODE_OFB == ctx->mode)
        {
            for(j=0; j<ctx->block_words; j++)
            {
                tmp_iv[j] = tmp_in[j] ^ tmp_out[j];
            }
        }
        else
        {;}

#ifdef CONFIG_SKE_SUPPORT_RESIST_FAULT_INJECTION
        /************* second **************/
        //set iv
        ske_set_iv_uint32(ctx->mode, ctx->iv, ctx->block_words);

        ske_simple_set_input_block(tmp_in, ctx->block_words);

        ske_start();
        ret = ske_wait_till_done();
        if(SKE_SUCCESS != ret)
        {
            ske_reset();
            ret = SKE_ERROR;
            goto END;
        }
        else
        {;}

        ske_simple_get_output_block((u32 *)tmp_out2, ctx->block_words);

        //compare
        if(ske_sec_uint32_memcmp(tmp_out, tmp_out2, ctx->block_words))
        {
            ske_reset();
            ret = SKE_ERROR;
            goto END;
        }
        else
        {;}
#endif

        //update iv
        if(SKE_MODE_ECB != ctx->mode)
        {
            if(SKE_MODE_CTR == ctx->mode)
            {
                ske_big_endian_add_uint8((u8 *)ctx->iv, ctx->block_bytes, 1);
            }
            else
            {
                uint32_copy(ctx->iv, tmp_iv, ctx->block_words);
            }
        }
        else
        {;}
#endif
        //output
        if(out)
        {
            memcpy(out, (u8 *)tmp_out, ctx->block_bytes);
            out += ctx->block_bytes;
        }
        else
        {;}
        
        in += ctx->block_bytes;
    }

END:

    if(SKE_ERROR == ret)
    {
        uint32_clear(ctx->iv, 4);
        if(p_out)
        {
            memset(p_out, 0, bytes);
        }
        else
        {;}
    }
    else
    {;}

    //clear buffer
    uint32_clear(tmp_in, ctx->block_words);
    uint32_clear(tmp_out, ctx->block_words);
    uint32_clear(tmp_out2, ctx->block_words);
    uint32_clear(tmp_iv, ctx->block_words);
    p_out = NULL;
    i=j=0;

    return ret;
}


/* function: ske_lp encryption or decryption for input including tail(CPU style)
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 *     in ------------------------- input, plaintext or ciphertext
 *     out ------------------------ output, ciphertext or plaintext
 *     in_bytes ------------------- input, byte length of input
 *     out_bytes ------------------ output, byte length of output
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. this function is designed for ECB/CBC/CFB/OFB/CTR modes, and the input includes tail
 *     2. to save memory, in and out could be the same buffer, in this case, the output will
 *        cover the input.
 *     3. if with padding scheme, for encryption, in_bytes could be any integer except zero; 
 *        for decryption, in_bytes must be a multiple of block byte length.
 *     4. if without padding scheme, for ECB/CBC, in_bytes must be a multiple of block byte 
 *        length, out_bytes will be the same as in_bytes.
 */
u32 ske_sec_update_including_last_block(SKE_CTX *ctx, u8 *in, u8 *out, u32 in_bytes,
        u32 *out_bytes)
{
    u32 tmp[4];
    u32 blocks_bytes, remainder_bytes;
    u32 ret;

    if(NULL == ctx)
    {
        return SKE_BUFFER_NULL;
    }
    else if (0U == in_bytes)
    {
        return SKE_SUCCESS;
    }
    else if((NULL == in) || (NULL == out))
    {
        return SKE_BUFFER_NULL;
    }
    else if (SKE_ISO_7816_4_PADDING < ctx->padding)
    {
        return SKE_INPUT_INVALID;
    }
    else
    {
        //handle other;
    }

#ifdef CONFIG_SKE_SUPPORT_MUL_THREAD
    ret = ske_sec_init_internal(ctx, ctx->alg, ctx->mode, ctx->crypto, ctx->key, ctx->sp_key_idx, (u8 *)ctx->iv);
    if(SKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}
#endif

    remainder_bytes = in_bytes & (ctx->block_bytes-1);
    blocks_bytes    = in_bytes - remainder_bytes;

    if(SKE_NO_PADDING != ctx->padding)
    {
        if(SKE_CRYPTO_ENCRYPT == ctx->crypto)
        {
            ske_sec_update_blocks(ctx, in, out, blocks_bytes);

            in += blocks_bytes;
            out += blocks_bytes;

            memcpy((u8 *)tmp, in, remainder_bytes);
            ske_sec_get_padding_block(ctx, (u8 *)tmp, remainder_bytes);
            ske_sec_update_blocks(ctx, (u8 *)tmp, out, ctx->block_bytes);
            *out_bytes = blocks_bytes + ctx->block_bytes;
        }
        else
        {
            if(remainder_bytes)
            {
                return SKE_INPUT_INVALID;
            }
            else
            {;}

            blocks_bytes -= ctx->block_bytes;
            if(blocks_bytes)
            {
                ske_sec_update_blocks(ctx, in, out, blocks_bytes);
            }
            else
            {;}

            ske_sec_update_blocks(ctx, in+blocks_bytes, (u8 *)tmp, ctx->block_bytes);

            ret = ske_sec_check_padding((u8 *)tmp, ctx->block_bytes, ctx->padding, out_bytes);
            if(SKE_SUCCESS != ret)
            {
                return ret;
            }
            else
            {;}

            memcpy(out+blocks_bytes, (u8 *)tmp, *out_bytes);

            *out_bytes += blocks_bytes;
        }
    }
    else  //SKE_NO_PADDING 
    {
        if((SKE_MODE_ECB == ctx->mode) || (SKE_MODE_CBC == ctx->mode))
        {
            if(remainder_bytes)
            {
                return SKE_INPUT_INVALID;
            }
            else
            {;}
        }
        else
        {;}

        //now for ECB/CBC, in_bytes is a multiple of block byte length(except 0),
        //but for CFB/OFB/CTR, in_bytes could be any value(except 0), for message tail, use stream style
        blocks_bytes = in_bytes - remainder_bytes;
        ske_sec_update_blocks(ctx, in, out, blocks_bytes);

        if(remainder_bytes)
        {
            in += blocks_bytes;
            out += blocks_bytes;

            ske_sec_update_blocks(ctx, in, (u8 *)tmp, ctx->block_bytes);

            memcpy(out, (u8 *)tmp, remainder_bytes);
        }
        else
        {;}

        *out_bytes = in_bytes;
    }

    return SKE_SUCCESS;
}


/* function: ske finish(secure version)
 * parameters:
 *     ctx ------------------------ input, SKE_CTX context pointer
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. if encryption or decryption is done, please call this 
 */
u32 ske_sec_final(SKE_CTX *ctx)
{
    memset((u8 *)ctx, 0, sizeof(SKE_CTX));

    return SKE_SUCCESS;
}


/* function: ske encrypting or decrypting(CPU style, one-off style)
 * parameters:
 *     alg ------------------------ input, ske algorithm
 *     mode ----------------------- input, ske algorithm operation mode, like ECB,CBC,OFB,etc.
 *     crypto --------------------- input, encrypting or decrypting
 *     key ------------------------ input, key in bytes
 *     sp_key_idx ----------------- input, index of secure port key, (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX],
 *                                  if the MSB(sp_key_idx) is 1, that means using low 128bit of the 256bit key
 *     iv ------------------------- input, iv in bytes, must be a block
 *     padding -------------------- input, padding scheme, should be SKE_NO_PADDING/SKE_ANSI_X923_PADDING/SKE_PKCS_5_7_PADDING/
 *                                         SKE_ISO_7816_4_PADDING
 *     in ------------------------- input, plaintext or ciphertext
 *     out ------------------------ output, ciphertext or plaintext
 *     bytes ---------------------- input, byte length of input or output.
 * return: SKE_SUCCESS(success), other(error)
 * caution:
 *     1. if mode is ECB, then there is no iv, in this case iv could be NULL
 *     2. this function is designed for ECB/CBC/CFB/OFB/CTR modes, and input/output unit is a block
 *     3. if key is from user input, please make sure key is not NULL(now sp_key_idx is useless),
 *        otherwise, key is from secure port, and (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX]
 *     4. to save memory, in and out could be the same buffer, in this case, the output will
 *        cover the input.
 *     5. bytes must be a multiple of block byte length.
 */
u32 ske_sec_crypto(SKE_ALG alg, SKE_MODE mode, SKE_CRYPTO crypto, u8 *key, 
        u16 sp_key_idx, u8 *iv, SKE_PADDING padding, u8 *in, u8 *out, 
        u32 in_bytes, u32 *out_bytes)
{
    u32 ret;
    SKE_CTX ctx[1];

    // enable ske cmu
    ske_init();

    ret = ske_sec_init(ctx, alg, mode, crypto, key, sp_key_idx, iv, padding);
    if(SKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    return ske_sec_update_including_last_block(ctx, in, out, in_bytes, out_bytes);
}



