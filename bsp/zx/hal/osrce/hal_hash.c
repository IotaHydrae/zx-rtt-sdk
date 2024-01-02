/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <aic_core.h>
#include <hal_hash.h>
#include <utility.h>

// hash callback function type
typedef void (*HASH_CALLBACK)(void);

/*
 * function: get HFE IP version
 * parameters: none
 * return: HFE IP version
 * caution:
 */
u32 hash_get_version(void)
{
    return readl(HASH_BASE + HASH_VERSION);
}

/*
 * function: set hash to be CPU mode
 * parameters: none
 * return: none
 * caution:
 */
void hash_set_cpu_mode(void)
{
    u32 val, mask;
    
    val = readl(HASH_BASE + HASH_CFG);
    mask = ~(1 << HASH_DMA_OFFSET);
    writel(val & mask, HASH_BASE + HASH_CFG);
}

/*
 * function: set hash to be DMA mode
 * parameters: none
 * return: none
 * caution:
 */
void hash_set_dma_mode(void)
{
    u32 val, flag;

    val = readl(HASH_BASE + HASH_CFG);
    flag = (1 << HASH_DMA_OFFSET);
    writel(val | flag, HASH_BASE + HASH_CFG);
}

/*
 * function: set the specific hash algorithm
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm
 * return: none
 * caution: 
 *     1. please make sure hash_alg is valid
 */
void hash_set_alg(enum hash_alg alg)
{
    u32 val, mask = (~0x0000000FU);

    val = readl(HASH_BASE + HASH_CFG);
    writel(val & mask, HASH_BASE + HASH_CFG);

    val = readl(HASH_BASE + HASH_CFG);
    writel(val | alg, HASH_BASE + HASH_CFG);
}

/*
 * function: enable hash interruption in CPU mode or DMA mode
 * parameters: none
 * return: none
 * caution: none
 */
void hash_enable_interruption(void)
{
    u32 val, flag;

    val = readl(HASH_BASE + HASH_CFG);
    flag = (1 << HASH_INTERRUPTION_OFFSET);
    writel(val | flag, HASH_BASE + HASH_CFG);
}

/*
 * function: disable hash interruption in CPU mode or DMA mode
 * parameters: none
 * return: none
 * caution: none
 */
void hash_disable_interruption(void)
{
    u32 val, mask;

    val = readl(HASH_BASE + HASH_CFG);
    mask = ~(1 << HASH_INTERRUPTION_OFFSET);
    writel(val & mask, HASH_BASE + HASH_CFG);
}

/*
 * function: set the tag whether current block is the last message block or not
 * parameters:
 *     tag ------------------------ input, 0(no), other(yes) 
 * return: none
 * caution: 
 *     1. if it is the last block, please config HASH_MSG_LEN,
 *        then the hardware will do the padding and post-processing.
 */
void hash_set_last_block(u32 tag)
{
    u32 val, mask, flag;

    if (0U != tag) { //current block is the last one of the message
        val = readl(HASH_BASE + HASH_CFG);
        flag = (1 << HASH_LAST_OFFSET);
        writel(val | flag, HASH_BASE + HASH_CFG);
    } else { //current block is not the last one of the message
        val = readl(HASH_BASE + HASH_CFG);
        mask = (~(1 << HASH_LAST_OFFSET));
        writel(val & mask, HASH_BASE + HASH_CFG);
    }
}

/*
 * function: get current HASH iterator value
 * parameters:
 *     iterator ------------------- output, current hash iterator
 *     hash_iterator_words -------- input, iterator word length
 * return: none
 * caution:
 *     1.
 */
void hash_get_iterator(u8 *iterator, u32 hash_iterator_words)
{
    u32 temp;
    u32 i;

    if (0 != ((u32)iterator & 3)) { // for the case that iterator is not aligned by word
        for (i = 0; i < hash_iterator_words; i++) {
            temp = readl(HASH_BASE + HASH_OUT(i));
            memcpy((u8 *)(&(iterator[(i << 2)])), (u8 *)(&temp), 4);
        }
    } else {
        for (i = 0; i < hash_iterator_words; i++) {
            ((u32 *)iterator)[i] = readl(HASH_BASE + HASH_OUT(i));
        }
    }
}

/*
 * function: input current iterator value
 * parameters:
 *     iterator ------------------- input, hash iterator value
 *     hash_iterator_words -------- input, iterator word length
 * return: none
 * caution:
 *     1. iterator must be word aligned
 */
void hash_set_iterator(u32 *iterator, u32 hash_iterator_words)
{
    u32 i;

    for (i = 0U; i < hash_iterator_words; i++) {
        writel(iterator[i], HASH_BASE + HASH_IN(i));
    }
}

/*
 * function: clear HASH_PCR_LEN
 * parameters:none
 * return: none
 * caution:none
 */
void hash_clear_msg_len(void)
{
    u32 flag = 0;

    writel(flag, HASH_BASE + HASH_PCR_LEN(0));
    writel(flag, HASH_BASE + HASH_PCR_LEN(1));
    writel(flag, HASH_BASE + HASH_PCR_LEN(2));
    writel(flag, HASH_BASE + HASH_PCR_LEN(3));
}

/*
 * function: set the total byte length of the whole message
 * parameters:
 *     msg_total_bytes ------------ input, total byte length of the whole message
 *     words ---------------------- input, word length of array msg_total_bytes
 * return: none
 * caution:
 *     1.
 */
void hash_set_msg_total_byte_len(u32 *msg_total_bytes, u32 words)
{
    while(0U != (words--))
    {
        writel(msg_total_bytes[words], HASH_BASE + HASH_PCR_LEN(words));
    }
}

/*
 * function: set dma output bytes length
 * parameters:
 *     bytes ---------------------- input,  byte length of the written data for hash hardware
 * return: none
 * caution:
 *     1.
 */
void hash_set_dma_output_len(u32 bytes)
{
    writel(bytes, HASH_BASE + HASH_DMA_WLEN);
}

/*
 * function: start HASH iteration calc
 * parameters: none
 * return: none
 * caution:
 */
void hash_start(void)
{
    u32 start_flag = 1;
    u32 clean_flag = 0;
    u32 val;

    //while((HASH_SR1 & flag) == 1)
    //{;}

    val = readl(HASH_BASE + HASH_SR2);
    writel(val | clean_flag, HASH_BASE + HASH_SR2);

    val = readl(HASH_BASE + HASH_CTRL);
    writel(val | start_flag, HASH_BASE + HASH_CTRL);
}

/*
 * function: wait till done
 * parameters: none
 * return: none
 * caution:
 */
void hash_wait_till_done(void)
{
    u32 finish_flag = 1;
    u32 clean_flag = 0;
    u32 val;

    val = readl(HASH_BASE + HASH_SR2);
    while (0 == (val & finish_flag)) {
        val = readl(HASH_BASE + HASH_SR2);
    }

    writel(clean_flag, HASH_BASE + HASH_SR2);
}

/* function: DMA wait till done
 * parameters:
 *     callback ------------------- callback function pointer
 * return: none
 * caution:
 */
void hash_dma_wait_till_done(HASH_CALLBACK callback)
{
    u32 finish_flag = 1;
    u32 clean_flag = 0;
    u32 val;

    val = readl(HASH_BASE + HASH_SR2);
    while (0U == (val & finish_flag)) {
        if (NULL != callback) {
            callback();
        } else {
            ;
        }
        val = readl(HASH_BASE + HASH_SR2);
    }

    writel(clean_flag, HASH_BASE + HASH_SR2);
}

/* function: input message(at most a block)
 * parameters:
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of msg, can not be greater than block bytes
 * return: none
 * caution:
 *     1. msg_bytes can not be greater than block bytes
 */
void hash_input_msg_u8(u8 *msg, u32 msg_bytes)
{
    u32 msg_words = msg_bytes / 4U;
    u32 remainder_bytes = msg_bytes & (3U);
    u32 tmp = 0U;
    u32 i;

    if (0U != (((u32)msg) & 3U)) {
        for (i = 0U; i < msg_words; i++) {
            memcpy((u8 *)&tmp, msg, 4);
            writel(tmp, HASH_BASE + HASH_M_DIN(i));
            msg = &(msg[4]);
        }
    } else {
        for (i = 0U; i < msg_words; i++) {
            writel(*((u32 *)msg), HASH_BASE + HASH_M_DIN(i));
            msg = &(msg[4]);
        }
    }

    if (0 != remainder_bytes) {
        tmp = 0U;
        memcpy((u8 *)&tmp, msg, remainder_bytes);
        writel(tmp, HASH_BASE + HASH_M_DIN(i));
    } else {
        ;
    }
}

#ifdef AIC_HASH_DMA
/* function: basic HASH DMA operation
 * parameters:
 *     in ------------------------- input, message of some blocks, or message including the last byte(last block)
 *     out ------------------------ output, hash digest or hmac.
 *     inByteLen ------------------ input, actual byte length of input msg
 *     callback ------------------- callback function pointer
 * return: none
 * caution:
 *     1. for DMA operation, the unit of input and output is 4 words, so, please make sure the buffer
 *        out is sufficient.
 *     2. if just to input message, not to get digest or hmac, please set para out to be NULL and WLEN to be 0.
 *        if to get the digest or hmac, para out can not be NULL, and please set WLEN to be digest length.
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
void hash_dma_operate(u32 in_h, u32 in_l, u32 out_h, u32 out_l, u32 inByteLen,
                      HASH_CALLBACK callback)
#else
void hash_dma_operate(u32 *in, u32 *out, u32 inByteLen, HASH_CALLBACK callback)
#endif
{
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    writel(in_l, HASH_BASE + HASH_DMA_SA);
    writel(out_l, HASH_BASE + HASH_DMA_DA);
#else
    //src addr
    writel(in & 0xFFFFFFFF, HASH_BASE + HASH_DMA_SA);

    //dst addr
    if (NULL != out) {
        writel(out & 0xFFFFFFFF, HASH_BASE + HASH_DMA_DA);
    } else {
        ;
    }
#endif

    //data byte length
    writel(inByteLen, HASH_BASE + HASH_DMA_RLEN);

    hash_start();

    hash_dma_wait_till_done(callback);
}

#endif

