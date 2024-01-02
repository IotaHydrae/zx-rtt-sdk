//#include <stdio.h>

#include <aic_core.h>
#include <utility.h>
#include <trng.h>

#ifdef AIC_TRNG_RO_ENTROPY
/* function: get rand(for internal test)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_internal(u8 *rand, u32 bytes)
{
    return get_rand_buffer(rand, bytes, get_rand_uint32);
}

/* function: get rand with post processing
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 *     get_rand_words ------------- input, function pointer to get some random words(at most 8 words)
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_with_post_processing(u8 *rand, u32 bytes, GET_RAND_WORDS get_rand_words)
{
    u32 flag = 0;
    volatile u32 errorCnt = AIC_TRNG_ERROR_COUNTER_THRESHOLD;
    u32 val, ret = TRNG_ERROR;

    //with post-processing
    val = readl(TRNG_BASE + TRNG_MSEL);
    if(flag == val)
    {
        trng_disable();
        trng_set_mode(1);
        trng_enable();
    }
    else
    {;}

    while(0U != (errorCnt--))
    {
        ret = get_rand_buffer(rand, bytes, get_rand_words);
        if ((TRNG_HT_ERROR == ret)||(TRNG_TIMEOUT_ERROR == ret))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    return ret;
}

/* function: get rand with fast speed(with entropy reducing, for such as clearing tmp buffer)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_fast(u8 *rand, u32 bytes)
{
#ifdef AIC_TRNG_GENERATE_BY_HARDWARE
    return get_rand_with_post_processing(rand, bytes, get_rand_uint32_without_reseed);
#else
    //return get_rand(rand, bytes);
    volatile u32 i;

    for(i=0;i<2U;i++)
    {
        memset_(rand, 0, bytes);
    }

    return TRNG_SUCCESS;
#endif
}

#ifdef AIC_TRNG_GENERATE_BY_HARDWARE
/* function: get rand(without entropy reducing)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand(u8 *rand, u32 bytes)
{
    return get_rand_with_post_processing(rand, bytes, get_rand_uint32_with_reseed);
}
#else

extern u8 SM3_Hash(u8 * message, u32 byteLen, u8 digest[32]);

static u32 seed=0x23ba78de;
static u8 sm3_buf[32];
static u8 buf_index=0;
//trng config flag
static u32 trng_cfg_flag = 0;

u32 get_rand_register(void)
{
    static u32 i=0;
    u8 buf[32];
    u32 tmp = 0;

    if(0U == trng_cfg_flag)
    {
        (void)SM3_Hash((u8 *)&seed, 4, sm3_buf);

        trng_cfg_flag=1;
    }
    else
    {;}

    if(buf_index<28U)
    {
        tmp = *((u32 *)(sm3_buf+buf_index));
        buf_index+=4U;
    }
    else if(buf_index == 28U)
    {
        tmp = *((u32 *)(sm3_buf+28));
        memcpy_(buf, sm3_buf, 32);
        i++;
        *((u32 *)(buf+16)) += 1U;
        (void)SM3_Hash(buf, 32, sm3_buf);
        buf_index=0;
    }
    else
    {
        //handle other;
    }

    return tmp;
}

u32 get_rand(u8 *rand, u32 byteLen)
{
    //u8 *rand_bak = rand;
    //u32 len_bak = byteLen;
    u32 word_len, result;
    u8 left_len = (u8)((u32)rand & 0x3U);

    // if the data addr is not aligned by word
    if ((u8)0 != left_len)
    {
        // wait the data is ready
        result = get_rand_register();

        if (byteLen > (4U - (u32)left_len)) {
            memcpy_(rand, (u8 *)(&result), 4U - (u32)left_len);
            byteLen -= (4U - (u32)left_len);
            rand = &(rand[(4U - left_len)]);
        }
        else
        {
            memcpy_(rand, (u8 *)(&result), byteLen);
            //trng_disable();
            //print_buf_U8(rand_bak, len_bak, "rand");
            return 0;//TRNG_SUCCESS;
        }
    }

    word_len = byteLen >> 2;
    left_len = (u8)(byteLen & 0x3U);

    // obtain the data by word
    while (0U != word_len--)
    {
        *((u32 *)rand) = get_rand_register();
        rand = &(rand[4]);
    }

    // if the byteLen is not aligned by word
    if ((u8)0 != left_len)
    {
        result = get_rand_register();
        memcpy_(rand, (u8 *)(&result), left_len);
    }

    //print_buf_U8(rand_bak, len_bak, "rand");
    return 0;//TRNG_SUCCESS;
}
#endif

#endif
