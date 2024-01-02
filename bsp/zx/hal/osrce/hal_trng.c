/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aic_core.h>
#include <hal_trng.h>
#include <utility.h>

/* function: get trng IP version
 * parameters: none
 * return: trng IP version
 * caution:
 */
u32 trng_get_version(void)
{
    return readl(TRNG_BASE + TRNG_VERSION);
}


/* function: TRNG global interruption enable
 * parameters: none
 * return: none
 * caution:
 */
void trng_global_int_enable(void)
{
    u32 val, flag;

    val = readl(TRNG_BASE + TRNG_CR);
    flag = (1 << TRNG_GLOBAL_INT_OFFSET);
    writel(val | flag, TRNG_BASE + TRNG_CR);
}

/* function: TRNG global interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_global_int_disable(void)
{
    u32 val, mask;

    val = readl(TRNG_BASE + TRNG_CR);
    mask = ~(1 << TRNG_GLOBAL_INT_OFFSET);
    writel(val & mask, TRNG_BASE + TRNG_CR);
}

/* function: TRNG empty-read interruption enable
 * parameters: none
 * return: none
 * caution:
 *     1. works when global interruption is enabled
 */
void trng_empty_read_int_enable(void)
{
    u32 val, flag;

    val = readl(TRNG_BASE + TRNG_CR);
    flag = (1 << TRNG_READ_EMPTY_INT_OFFSET);
    writel(val | flag, TRNG_BASE + TRNG_CR);
}

/* function: TRNG empty-read interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_empty_read_int_disable(void)
{
    u32 val, mask;

    val = readl(TRNG_BASE + TRNG_CR);
    mask = ~(1 << TRNG_READ_EMPTY_INT_OFFSET);
    writel(val & mask, TRNG_BASE + TRNG_CR);
}

/* function: TRNG data interruption enable
 * parameters: none
 * return: none
 * caution:
 *     1. works when global interruption is enabled
 */
void trng_data_int_enable(void)
{
    u32 val, flag;

    val = readl(TRNG_BASE + TRNG_CR);
    flag = (1<<TRNG_DATA_INT_OFFSET);
    writel(val | flag, TRNG_BASE + TRNG_CR);
}

/* function: TRNG data interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_data_int_disable(void)
{
    u32 val, mask;

    val = readl(TRNG_BASE + TRNG_CR);
    mask = ~(1 << TRNG_DATA_INT_OFFSET);
    writel(val & mask, TRNG_BASE + TRNG_CR);
}

/* function: TRNG enable
 * parameters: none
 * return: none
 * caution:
 */
void trng_enable(void)
{
    u32 val, flag = 1;

    val = readl(TRNG_BASE + TRNG_CR);
    writel(val | flag, TRNG_BASE + TRNG_CR);
}

/* function: TRNG disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_disable(void)
{
    u32 val, mask = ~1;

    val = readl(TRNG_BASE + TRNG_CR);
    writel(val & mask, TRNG_BASE + TRNG_CR);

    //sleep for a while until the entropy is stable before enabling it.
    uint32_sleep(AIC_TRNG_DELAY_COUNTER, 0);
}

#ifdef AIC_TRNG_RO_ENTROPY
/* function: set RO entropy config
 * parameters:
 *     cfg ------------------------ RO entropy config, only the low 4 bits are valid, every bit
 *                                  indicates one RO entropy, the MSB is RO 4, and LSB is RO 1
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. only the low 4 bits of cfg are valid
 *     2. if the low 4 bits of cig is 0, that means to disable all RO entropy
 */
u32 trng_ro_entropy_config(u8 cfg)
{
    u32 val, mask = ~(0x0000000FU);

    if (cfg > 15U) {
        return TRNG_INVALID_INPUT;
    } else {
        ;
    }

    val = readl(TRNG_BASE + RO_CLK_EN);
    writel((val & mask) | cfg, TRNG_BASE + RO_CLK_EN);

    return TRNG_SUCCESS;
}

/* function: set sub RO entropy config
 * parameters:
 *     sn ------------------------- input, RO entropy source series number, must be in [1,4]
 *     value ---------------------- input, the config value of RO sn
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 trng_ro_sub_entropy_config(u8 sn, u16 cfg)
{
    u32 mask_high = ~0xFFFF0000U;
    u32 mask_low = ~0x0000FFFFU;
    u32 val, ret = TRNG_SUCCESS;

    switch (sn) {
        case 1:
            val = readl(TRNG_BASE + RO_SRC_EN1);
            writel((val & mask_high) | (cfg << 16), TRNG_BASE + RO_SRC_EN1);
            break;
        case 2:
            val = readl(TRNG_BASE + RO_SRC_EN1);
            writel((val & mask_low) | (cfg), TRNG_BASE + RO_SRC_EN1);
            break;
        case 3:
            val = readl(TRNG_BASE + RO_SRC_EN2);
            writel((val & mask_high) | (cfg << 16), TRNG_BASE + RO_SRC_EN2);
            break;
        case 4:
            val = readl(TRNG_BASE + RO_SRC_EN2);
            writel((val & mask_low) | (cfg), TRNG_BASE + RO_SRC_EN2);
            break;
        default:
            ret = TRNG_INVALID_INPUT;
            break;
    }

    return ret;
}

/* function: set TRNG mode
 * parameters:
 *     with_post_processing ------- 0:no,  other:yes
 * return: none
 * caution:
 */
void trng_set_mode(u8 with_post_processing)
{
    u32 mask = ~1;
    u32 flag = 1U;
    u32 clear_flag = 0x00000007U;
    u32 val;

    if ((u8)0 != with_post_processing) {
        val = readl(TRNG_BASE + TRNG_MSEL);
        writel(val | flag, TRNG_BASE + TRNG_MSEL);
    } else {
        val = readl(TRNG_BASE + TRNG_MSEL);
        writel(val & mask, TRNG_BASE + TRNG_MSEL);
    }

    val = readl(TRNG_BASE + TRNG_SR);
    writel(val | clear_flag, TRNG_BASE + TRNG_SR); //write 1 to clear
}

/* function: reseed TRNG(works when DRBG is enabled)
 * parameters: none
 * return: none
 * caution:
 *     1. used for DRBG
 */
void trng_reseed(void)
{
    u32 flag = 1;
    u32 clear_flag = 0x00000007;
    u32 val;

    val = readl(TRNG_BASE + TRNG_RESEED);
    writel(val | flag, TRNG_BASE + TRNG_RESEED);

    val = readl(TRNG_BASE + TRNG_SR);
    writel(val | clear_flag, TRNG_BASE + TRNG_SR); //write 1 to clear
}

/* function: TRNG set frequency
 * parameters:
 *     freq ----------------------- input, frequency config, must be in [0,3], and
 *                                  0: 1/4 of input frequency,
 *                                  1: 1/8 ...,
 *                                  2: 1/16 ...,
 *                                  3: 1/32 ...,
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 trng_set_freq(u8 freq)
{
    u32 val;
    u32 mask = ~(((u32)0x0000000F) << TRNG_FREQ_OFFSET);

    if (freq > 3) {
        return TRNG_INVALID_INPUT;
    } else {
        ;
    }

    val = readl(TRNG_BASE + RO_CLK_EN);
    writel((val & mask) | (freq << TRNG_FREQ_OFFSET), TRNG_BASE + RO_CLK_EN);

    return TRNG_SUCCESS;
}

/* function: get some rand words
 * parameters:
 *     a -------------------------- output, random words
 *     words ---------------------- input, word number of output, must be in [1, 8]
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the two parameters are valid
 */
u32 get_rand_uint32(u32 *a, u32 words)
{
    u32 DT_ready_flag = 2;
    u32 HT_error_flag = 1;
    u32 val, i;

    val = readl(TRNG_BASE + TRNG_SR);
    while (0 == (val & DT_ready_flag)) {
        val = readl(TRNG_BASE + TRNG_SR);
        if (0 != (val & HT_error_flag)) {
            return TRNG_HT_ERROR;
        } else {
            ;
        }
    }

    // if now DT ready, but HT error
    val = readl(TRNG_BASE + TRNG_SR);
    if (0 != (val & HT_error_flag)) {
        return TRNG_HT_ERROR;
    } else {
        ;
    }

    for (i = 0; i < words; i++) {
        *(a++) = readl(TRNG_BASE + TRNG_DR); // printf("\r\n %08x", *(a-1));
    }

    val = readl(TRNG_BASE + TRNG_SR);
    writel(val | DT_ready_flag, TRNG_BASE + TRNG_SR); // clear

    return TRNG_SUCCESS;
}

/* function: get some rand words(with post-processing, but without reseed)
 * parameters:
 *     a -------------------------- output, random words
 *     words ---------------------- input, word number of output, must be in [1, 8]
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the two parameters are valid
 */
u32 get_rand_uint32_without_reseed(u32 *a, u32 words)
{
    u32 DT_ready_flag = 2U;
    u32 HT_error_flag = 1U;
    u32 clear_flag = 7U;
    volatile u32 cnt = 0;
    u32 val, i;

    val = readl(TRNG_BASE + TRNG_SR);
    while (0U == (val & DT_ready_flag)) {
        val = readl(TRNG_BASE + TRNG_SR);
        if (0U != (val & HT_error_flag)) {
            trng_disable();
            val = readl(TRNG_BASE + TRNG_SR);
            writel(val | clear_flag, TRNG_BASE + TRNG_SR); // clear (alarm) status
            trng_enable();

            return TRNG_HT_ERROR;
        } else {
            cnt++;
            if (cnt > AIC_TRNG_TIMEOUT_COUNTER_THRESHOLD) {
                return TRNG_TIMEOUT_ERROR;
            } else {
                ;
            }
        }
    }

    // if now DT ready, but HT error
    val = readl(TRNG_BASE + TRNG_SR);
    if (0U != (val & HT_error_flag)) {
        trng_disable();
        val = readl(TRNG_BASE + TRNG_SR);
        writel(val | clear_flag, TRNG_BASE + TRNG_SR); // clear (alarm) status
        trng_enable();

        return TRNG_HT_ERROR;
    } else {
        ;
    }

    for (i = 0; i < words; i++) {
        *(a++) = readl(TRNG_BASE + TRNG_DR); // printf("\r\n %08x", *(a-1));
    }

    val = readl(TRNG_BASE + TRNG_SR);
    writel(val | DT_ready_flag, TRNG_BASE + TRNG_SR); // clear

    return TRNG_SUCCESS;
}

/* function: get some rand words(with post-processing and reseed)
 * parameters:
 *     a -------------------------- output, random words
 *     words ---------------------- input, word number of output, must be in [1, 8]
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the two parameters are valid
 */
u32 get_rand_uint32_with_reseed(u32 *a, u32 words)
{
    u32 ret = get_rand_uint32_without_reseed(a, words);

    if (TRNG_SUCCESS == ret) {
        trng_reseed(); //for next generation.
    } else {
        ;
    };

    return ret;
}

/* function: get rand buffer(internal basis interface)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 *     get_rand_words ------------- input, function pointer to get some random words(at most 8 words)
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_buffer(u8 *rand, u32 bytes, GET_RAND_WORDS get_rand_words)
{
    u32 enable_flag = 1;
    u32 ro_entropy_mask = (0x0000000F);
    u32 i;
    u32 tmp, tmp_len, rng_data;
    u32 count, ret;
    u8 *a = rand;
    u32 val, val1;

    //check input parameters
    if (NULL == rand || NULL == get_rand_words) {
        return TRNG_BUFFER_NULL;
    } else if (0 == bytes) {
        return TRNG_SUCCESS;
    } else {
        //handle other;
    }

    //make sure trng and ro are enabled
    val = readl(TRNG_BASE + TRNG_CR);
    val1 = readl(TRNG_BASE + RO_CLK_EN);
    if (0 == (val & enable_flag)) {
        return TRNG_INVALID_CONFIG;
    } else if (0 == (val1 & ro_entropy_mask)) {
        return TRNG_INVALID_CONFIG;
    } else {
        //handle other;
    }

    tmp_len = bytes;

    tmp = ((u32)a) & 3;
    if (0 != tmp) {
        i = 4 - tmp;

        ret = get_rand_words(&rng_data, 1);
        if (TRNG_SUCCESS != ret) {
            goto END;
        } else {
            if (tmp_len > i) {
                memcpy_(a, (u8 *)(&rng_data), i);
                a = &a[i];
                tmp_len -= i;
            } else {
                memcpy_(a, (u8 *)(&rng_data), tmp_len);
                goto END;
            }
        }
    } else {
        ;
    }

    tmp = tmp_len / 4U;
    while (0U != tmp) {
        if (tmp > 8U) {
            count = 8U;
        } else {
            count = tmp;
        }

        ret = get_rand_words((u32 *)a, count);
        if (TRNG_SUCCESS != ret) {
            goto END;
        } else {
            a = &(a[count << 2]);
            tmp -= count;
        }
    }

    tmp_len = tmp_len & 3U;
    if (0U != tmp_len) {
        ret = get_rand_words(&rng_data, 1);
        if (TRNG_SUCCESS != ret) {
            goto END;
        } else {
            memcpy_(a, (u8 *)(&rng_data), tmp_len);
        }
    }

    ret = TRNG_SUCCESS;

END:

#ifdef TRNG_POKER_TEST
    if (TRNG_SUCCESS == ret) {
        poker_test(rand, bytes);
    }
#endif

    return ret;
}

#endif

