/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aic_core.h>
#include <aic_hal.h>
#include <hal_ske.h>
#include "utility.h"
#include "trng.h"

s32 ske_init(void)
{
    int ret = 0;

    ret = hal_clk_enable(CLK_CE);
    if (ret < 0) {
        hal_log_err("Failed to enable CE clk.\n");
        return -EFAULT;
    }

    ret = hal_clk_enable_deassertrst(CLK_CE);
    if (ret < 0) {
        hal_log_err("Failed to reset CE deassert.\n");
        return -EFAULT;
    }

    return 0;
}
/* function: get ske IP version
 * parameters: none
 * return: ske IP version
 * caution:
 */
u32 ske_get_version(void)
{
    return readl(SKE_BASE + SKE_VERSION);
}

/* function: soft reset ske
 * parameters: none
 * return: none
 * caution: none
 */
void ske_reset(void)
{
    u32 val, flag;

    val = readl(SKE_BASE + SKE_CTRL);
    flag = (1 << SKE_RESET_OFFSET);
    writel(val | flag, SKE_BASE + SKE_CTRL);
}

/* function: set the ske endian
 * parameters: none
 * return: none
 * caution:
 *     1. actually, this config works for only CPU mode now
 */
void ske_set_endian_uint32(void)
{
    u32 mask = ~(1 << SKE_REVERSE_BYTE_ORDER_IN_WORD_OFFSET);
#ifndef SKE_CPU_BIG_ENDIAN
    u32 flag = (1 << SKE_REVERSE_BYTE_ORDER_IN_WORD_OFFSET);
#endif
    u32 val;

    val = readl(SKE_BASE + SKE_CFG);
    writel(val & mask, SKE_BASE + SKE_CFG); //clear bit[12], and now requires CPU is big-endian
#ifndef SKE_CPU_BIG_ENDIAN
    val = readl(SKE_BASE + SKE_CFG);
    writel(val | flag, SKE_BASE + SKE_CFG); //requires CPU is little-endian, input and output reversed by hardware----ske IP
#endif
}

/* function: set ske encrypting or decrypting
 * parameters:
 *     crypto --------------------- input, SKE_CRYPTO_ENCRYPT or SKE_CRYPTO_DECRYPT
 * return: none
 * caution:
 *     1. please make sure crypto is valid
 */
void ske_set_crypto(SKE_CRYPTO crypto)
{
    u32 val, mask, flag;

    val = readl(SKE_BASE + SKE_CFG);
    mask = ~(1 << SKE_CRYPTO_OFFSET);
    writel(val & mask, SKE_BASE + SKE_CFG);

    val = readl(SKE_BASE + SKE_CFG);
    flag = (crypto << SKE_CRYPTO_OFFSET);
    writel(val | flag, SKE_BASE + SKE_CFG);
}

/* function: set ske alg
 * parameters:
 *     ske_alg -------------------- input, ske algorithm
 * return: none
 * caution:
 *     1. please make sure ske_alg is valid
 */
void ske_set_alg(SKE_ALG ske_alg)
{
    u32 val, choice;
    u32 mask = ~((u32)0x03);

    switch (ske_alg) {
        case SKE_ALG_AES_128:
            choice = 0;
            val = readl(SKE_BASE + SKE_AES_CFG);
            writel(val & mask, SKE_BASE + SKE_AES_CFG);
            break;
        case SKE_ALG_AES_192:
            choice = 0;
            val = readl(SKE_BASE + SKE_AES_CFG);
            writel(val & mask, SKE_BASE + SKE_AES_CFG);
            val = readl(SKE_BASE + SKE_AES_CFG);
            writel(val | 0x01, SKE_BASE + SKE_AES_CFG);
            break;
        case SKE_ALG_AES_256:
            choice = 0;
            val = readl(SKE_BASE + SKE_AES_CFG);
            writel(val & mask, SKE_BASE + SKE_AES_CFG);
            val = readl(SKE_BASE + SKE_AES_CFG);
            writel(val | 0x02, SKE_BASE + SKE_AES_CFG);
            break;
        case SKE_ALG_DES:
            choice = 1;
            break;
        case SKE_ALG_SM4:
            choice = 2;
            break;
        case SKE_ALG_TDES_128:
        case SKE_ALG_TDES_192:
            choice = 3;
            break;
        default:
            choice = 2; //default alg SM4
    }

#if (AIC_SKE_ALG_TI_NO == 1)
    choice += 4;
#endif

    val = readl(SKE_BASE + SKE_CFG);
    mask = ~((u32)0x07);
    writel(val & mask, SKE_BASE + SKE_CFG); // clear bit[2:0]
    val = readl(SKE_BASE + SKE_CFG);
    writel(val | choice, SKE_BASE + SKE_CFG); // Set key length
}

/* function: set ske alg operation mode 
 * parameters:
 *     mode ----------------------- input, operation mode
 * return: none
 * caution:
 *     1. please make sure mode is valid
 */
void ske_set_mode(SKE_MODE mode)
{
    u32 val, mask, flag;

    val = readl(SKE_BASE + SKE_CFG);
    mask = ~(7 << SKE_MODE_OFFSET);
    writel(val & mask, SKE_BASE + SKE_CFG); // clear bit 3

    val = readl(SKE_BASE + SKE_CFG);
    flag = (mode << SKE_MODE_OFFSET);
    writel(val | flag, SKE_BASE + SKE_CFG); // set mode
}

/* function: set ske rng seed and start rng 
 * parameters: none
 * return: none
 * caution:  
 *     1. 
 */
u32 ske_set_seed(void)
{
    u32 flag = 1, seek = 0;

    seek = readl(SKE_BASE + SKE_RNG_SD(0));
    if (TRNG_SUCCESS !=
        get_rand((u8 *)&(seek), SKE_SET_SEED_BYTE_LEN)) {
        return SKE_ERROR;
    } else {
        // start rng
        writel(flag, SKE_BASE + SKE_RNG_CTRL); // set mode

        return SKE_SUCCESS;
    }
}

/* function: check whether runtime alarm exists 
 * parameters: none 
 * return: SKE_SUCCESS(correct), other(error)
 * caution:
 */
u32 ske_check_runtime_alarm(void)
{
    u32 val, flag = 1;

    //check and clear runtime alarm state
    val = readl(SKE_BASE + SKE_SR2);
    if (val & flag) {
        writel(val | flag, SKE_BASE + SKE_SR2); // write 1 to clear
        ske_reset();

        return SKE_ERROR; //SKE_RUNTIME_ALARM;
    } else {
        ;
    }

    return SKE_SUCCESS;
}

/* function: check ske config.
 * parameters: none
 * return: 0(correct), other(error) 
 * caution:
 *     1. must be called after config.
 */
u32 ske_check_config(void)
{
    u32 val, flag;

    val = readl(SKE_BASE + SKE_SR1);
    flag = (1 << SKE_ERR_CFG_OFFSET);

    return val & flag;
}

/* function: start ske calc
 * parameters: none 
 * return: none
 * caution:  
 *     1. if it is the first time to calc, then use the key,iv/nonce already set, 
 *        otherwise use the one that hardware kept or updated
 */
void ske_start(void)
{
    u32 val, flag = 1;

    val = readl(SKE_BASE + SKE_CTRL);
    writel(val | flag, SKE_BASE + SKE_CTRL);
}

/* function: wait till done
 * parameters: none 
 * return: 0(success), other(error)
 * caution: 
 */
u32 ske_wait_till_done(void)
{
    u32 val, flag = 1;
    u32 ret;

#if (AIC_SKE_ALG_TI_NO == 1)
    flag = 5; //4;   //
#elif (AIC_SKE_ALG_TI_NO == 2)
    flag = 3; //2;   //
#endif

    val = readl(SKE_BASE + SKE_SR1);
    while ((val & flag) != 0)
    {
        ret = ske_check_runtime_alarm();
        if (SKE_SUCCESS != ret) {
            return ret;
        } else {
            ;
        }
        val = readl(SKE_BASE + SKE_SR1);
    }

    // not update key and iv
    val = readl(SKE_BASE + SKE_CFG);
    flag = (1 << SKE_UPDATE_KEY_OFFSET) | (1 << SKE_UPDATE_IV_OFFSET);
    writel(val & ~flag, SKE_BASE + SKE_CFG);

    return ske_check_runtime_alarm();
}

/* function: set key
 * parameters:
 *     key ------------------------ input, key in word buffer
 *     idx ------------------------ input, key index, only 1 and 2 are valid
 *     key_words ------------------ input, word length of key
 * return: none
 * caution:
 *     1. if idx is 1, set key1 register, else if idx is 2, set key2 register, please
 *        make sure idx is valid
 *     2. for 3DES 2key(128bits), here key_words should be 6, actually, the first and
 *         the last part are the same.
 */
void ske_set_key_uint32(u32 *key, u32 idx, u32 key_words)
{
    u32 val, flag;
    u8 i;

    for (i = 0; i < key_words; i++) {
        writel(key[i], SKE_BASE + SKE_KEY(i));
    }

    val = readl(SKE_BASE + SKE_CFG);
    flag = (1 << SKE_UPDATE_KEY_OFFSET);
    writel(val | flag, SKE_BASE + SKE_CFG); // set update key
}

/* function: set iv if the mode is not ECB
 * parameters:
 *     mode ----------------------- input, ske operation mode
 *     iv ------------------------- input, iv in word buffer
 *     block_words ---------------- input, word length of ske block
 * return: none
 * caution: 
 *     1. please make sure the three parameters are valid
 */
void ske_set_iv_uint32(u32 mode, u32 *iv, u32 block_words)
{
    u32 val, flag;
    u32 i = 0;

    if (SKE_MODE_ECB == mode) {
        val = readl(SKE_BASE + SKE_CFG);
        flag = (1 << SKE_UPDATE_IV_OFFSET);
        writel(val & ~flag, SKE_BASE + SKE_CFG); // set not update IV
        return;
    } else {
        ;
    }

    for (i = 0; i < block_words; i++) {
        writel(iv[i], SKE_BASE + SKE_IV(i));
    }

    val = readl(SKE_BASE + SKE_CFG);
    flag = (1 << SKE_UPDATE_IV_OFFSET);
    writel(val | flag, SKE_BASE + SKE_CFG); // set update IV
}

/* function: input one block
 * parameters:
 *     in ------------------------- input, plaintext or ciphertext in word buffer
 *     block_words ---------------- input, word length of ske block
 * return: none
 * caution:
 *     1. in is a word buffer of only one block.
 */
void ske_simple_set_input_block(u32 *in, u32 block_words)
{
    //for DES/3DES
    writel(in[0], SKE_BASE + SKE_IN(0));
    writel(in[1], SKE_BASE + SKE_IN(1));

    //for AES/SM4
    if (4 == block_words) {
        writel(in[2], SKE_BASE + SKE_IN(2));
        writel(in[3], SKE_BASE + SKE_IN(3));
    } else {
        ;
    }
}

/* function: output one block
 * parameters:
 *     out ------------------------ output, one block output of ske in word buffer
 *     block_words ---------------- input, word length of ske block
 * return: none
 * caution:
 */
void ske_simple_get_output_block(u32 *out, u32 block_words)
{
    //for DES/3DES
    out[0] = readl(SKE_BASE + SKE_OUT(0));
    out[1] = readl(SKE_BASE + SKE_OUT(1));

    //for AES/SM4
    if (4 == block_words) {
        out[2] = readl(SKE_BASE + SKE_OUT(2));
        out[3] = readl(SKE_BASE + SKE_OUT(3));
    } else {
        ;
    }
}

