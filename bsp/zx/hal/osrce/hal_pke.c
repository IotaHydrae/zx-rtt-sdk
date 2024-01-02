//#include <stdio.h>

#include <aic_core.h>
#include <aic_common.h>
#include <aic_hal.h>
#include <hal_pke.h>
#include "trng.h"
#include "utility.h"
#ifdef AIC_PKE_SEC
#include "utility_sec.h"
#endif

s32 pke_init(void)
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

/* function: get pke IP version
 * parameters: none
 * return: pke IP version
 * caution:
 */
u32 pke_get_version(void)
{
    return readl(PKE_BASE + PKE_VERSION);
}

/* function: clear finished and interrupt tag
 * parameters: none
 * return: none
 * caution:
 */
void pke_clear_interrupt(void)
{
    u32 val, mask = ~((u32)1);

    val = readl(PKE_BASE + PKE_RISR);
#if 1
    writel(val & mask, PKE_BASE + PKE_RISR); //write 0 to clear
#else
    u32 flag = 1;

    if (val & flag) {
        writel(val & mask, PKE_BASE + PKE_RISR); //write 0 to clear
    } else {
        ;
    }
#endif
}

/* function: enable pke interrupt
 * parameters: none
 * return: none
 * caution:
 */
//static void pke_enable_interrupt(void)
//{
//    u32 val, flag = (u32)1;
//
//    val = readl(PKE_BASE + PKE_IMCR);
//    writel(val | flag, PKE_BASE + PKE_IMCR);
//}

/* function: disable pke interrupt
 * parameters: none
 * return: none
 * caution:
 */
//static void pke_disable_interrupt(void)
//{
//    u32 val, mask = ~((u32)1);
//
//    val = readl(PKE_BASE + PKE_IMCR);
//    writel(val & mask, PKE_BASE + PKE_IMCR);
//}

/* function: set operand width
 * parameters:
 *     bitLen --------------------- input, bit length of operand
 * return: uint bytes of hardware operand.
 * caution: please make sure 0 < bitLen <= AIC_PKE_OPERAND_MAX_BIT_LEN
 */
u32 pke_set_operand_width(u32 bitLen)
{
    u32 val, mask = ~(0x07FFFF);
    u32 cfg = 0, len;
    u32 step_bytes = 0;

    len = (bitLen + 255) / 256;

    if (1 == len) {
        cfg = 2;
        step_bytes = 0x24;
    } else if (2 == len) {
        cfg = 3;
        step_bytes = 0x44;
    } else if (len <= 4) {
        cfg = 4;
        step_bytes = 0x84;
    } else if (len <= 8) {
        cfg = 5;
        step_bytes = 0x104;
    } else if (len <= 16) {
        cfg = 6;
        step_bytes = 0x204;
    } else {
        ;
    }

    cfg = (cfg << 16) | (bitLen); //cfg = (cfg<<16)|(len<<8);

    val = readl(PKE_BASE + PKE_CFG);
    writel(val & mask, PKE_BASE + PKE_CFG);
    val = readl(PKE_BASE + PKE_CFG);
    writel(val | cfg, PKE_BASE + PKE_CFG);
    //printf("\r\n %u, PKE_CFG = %08x", len, PKE_CFG);

    return step_bytes;
}

/* function: get current operand byte length
 * parameters: none
 * return: current operand byte length
 * caution: none
 */
u32 pke_get_operand_bytes(void)
{
    u32 val, step_bytes;

    val = readl(PKE_BASE + PKE_CFG);
    switch ((val >> 16) & 0x07U) {
        case 2:
            step_bytes = 0x24;
            break;
        case 3:
            step_bytes = 0x44;
            break;
        case 4:
            step_bytes = 0x84;
            break;
        case 5:
            step_bytes = 0x104;
            break;
        case 6:
            step_bytes = 0x204;
            break;
        default:
            step_bytes = 0x24;
    }

    return step_bytes;
}

/* function: set operation micro code
 * parameters:
 *     addr ----------------------- input, specific micro code
 * return: none
 * caution:
 */
void pke_set_microcode(u32 addr)
{
    writel(addr, PKE_BASE + PKE_MC_PTR);
}

/* function: set exe config
 * parameters:
 *     cfg ------------------------ input, specific config value
 * return: none
 * caution:
 */
void pke_set_exe_cfg(u32 cfg)
{
    writel(cfg, PKE_BASE + PKE_EXE_CONF);
}

/* function: start pke calc
 * parameters: none
 * return: none
 * caution:
 */
void pke_start(void)
{
    u32 val, flag = PKE_START_CALC;

    val = readl(PKE_BASE + PKE_CTRL);
    writel(val | flag, PKE_BASE + PKE_CTRL);
}

/* function: return calc return code
 * parameters: none
 * return 0(success), other(error)
 * caution:
 */
u32 pke_check_rt_code(void)
{
    u32 val, mask = 0x07u;

    val = readl(PKE_BASE + PKE_RT_CODE);
    return (u8)(val & mask);
}

/* function: wait till done
 * parameters: none
 * return: none
 * caution:
 */
void pke_wait_till_done(void)
{
    u32 val, flag = 1;

    val = readl(PKE_BASE + PKE_RISR);
    while (!(val & flag)) {
        val = readl(PKE_BASE + PKE_RISR);
    }
}

/* function: set operation micro code, start hardware, wait till done, and return code
 * parameters:
 *     micro_code ----------------- input, specific micro code
 * return: PKE_SUCCESS(success), other(inverse not exists or error)
 * caution:
 */
u32 pke_set_micro_code_start_wait_return_code(u32 micro_code)
{
    pke_set_microcode(micro_code);
    pke_clear_interrupt();
    pke_start();
    pke_wait_till_done();

    return pke_check_rt_code();
}

/* function: ainv = a^(-1) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     ainv ----------------------- output, ainv = a^(-1) mod modulus
 *     modWordLen ----------------- input, word length of modulus and ainv
 *     aWordLen ------------------- input, word length of a
 * return: PKE_SUCCESS(success), other(inverse not exists or error)
 * caution:
 *     1. please make sure aWordLen <= modWordLen <= OPERAND_MAX_WORD_LEN and a < modulus
 */
u32 pke_modinv(const u32 *modulus, const u32 *a, u32 *ainv, u32 modWordLen,
               u32 aWordLen)
{
    u32 step_bytes, step_words;
    u32 ret;

    //pke_set_operand_width(modWordLen<<5);
    step_bytes = pke_set_operand_width(get_valid_bits(modulus, modWordLen));
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     modWordLen); //B3 modulus
    if (step_words > modWordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + modWordLen,
                     step_words - modWordLen);
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)a, aWordLen); //B0 a
    if (step_words > aWordLen) {
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + aWordLen,
                     step_words - aWordLen);
    } else {
        ;
    }

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MODINV);
    if (PKE_SUCCESS == ret) {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), ainv,
                         modWordLen); //A0 ainv
    } else if (PKE_NO_MODINV != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), modWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), aWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), modWordLen << 2);
#endif
    } else {
        ;
    }

    return ret;
}

/* function: out = (a+b) mod modulus or out = (a-b) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b mod modulus or out = (a-b) mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 *     micro_code ----------------- input, must be MICROCODE_MODADD or MICROCODE_MODSUB
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a,b must be less than modulus
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_modadd_modsub_internal(const u32 *modulus, const u32 *a, const u32 *b,
                               u32 *out, u32 wordLen, u32 micro_code)
{
    u32 step_bytes, step_words;
    u32 ret;

    step_bytes = pke_set_operand_width(wordLen << 5);
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     wordLen); //B3 modulus
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), (u32 *)a, wordLen); //A0 a
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)b, wordLen); //B0 b

    if (step_words > wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + wordLen,
                     step_words - wordLen);
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + wordLen,
                     step_words - wordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + wordLen,
                     step_words - wordLen);
    } else {
        ;
    }

    ret = pke_set_micro_code_start_wait_return_code(micro_code);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), wordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out,
                         wordLen); //A0 result

        return PKE_SUCCESS;
    }
}

/* function: out = (a+b) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a,b must be less than modulus
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_modadd(const u32 *modulus, const u32 *a, const u32 *b, u32 *out,
               u32 wordLen)
{
    return pke_modadd_modsub_internal(modulus, a, b, out, wordLen,
                                      MICROCODE_MODADD);
}

/* function: out = (a-b) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a-b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a,b must be less than modulus
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_modsub(const u32 *modulus, const u32 *a, const u32 *b, u32 *out,
               u32 wordLen)
{
    return pke_modadd_modsub_internal(modulus, a, b, out, wordLen,
                                      MICROCODE_MODSUB);
}

/* function: out = a+b or out = a-b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b or out = a-b
 *     wordLen -------------------- input, word length of a, b, out
 *     micro_code ----------------- input, must be MICROCODE_INTADD or MICROCODE_INTSUB
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. if a+b output may overflow, if a-b please make sure a > b
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN

u32 pke_add_sub_internal(const u32 *a, const u32 *b, u32 *out, u32 wordLen,
        u32 micro_code)
{
    u32 step_bytes, step_words;
    u32 ret;

    step_bytes = pke_set_operand_width(wordLen<<5);
    step_words = step_bytes>>2;

    pke_load_operand((u32 *)(PKE_A(1,step_bytes)), (u32 *)a, wordLen);          //A1 a
    pke_load_operand((u32 *)(PKE_B(1,step_bytes)), (u32 *)b, wordLen);          //B1 b

    if(step_words > wordLen)
    {
        uint32_clear((u32 *)(PKE_A(1,step_bytes))+wordLen, step_words-wordLen);
        uint32_clear((u32 *)(PKE_B(1,step_bytes))+wordLen, step_words-wordLen);
    }
    else
    {;}

    ret = pke_set_micro_code_start_wait_return_code(micro_code);
    if(PKE_SUCCESS != ret)
    {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(1,step_bytes)), wordLen<<2);
        get_rand_fast((u8 *)(PKE_B(1,step_bytes)), wordLen<<2);
#endif
        return ret;
    }
    else
    {
        pke_read_operand((u32 *)(PKE_A(1,step_bytes)), out, wordLen);                //A1 result

        return PKE_SUCCESS;
    }
} */

/* function: out = a+b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b
 *     wordLen -------------------- input, word length of a, b, out
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a+b may overflow
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_add(const u32 *a, const u32 *b, u32 *out, u32 wordLen)
{
#if 0
    return pke_add_sub_internal(a, b, out, wordLen, MICROCODE_INTADD);
#else
    u32 i, carry, temp, temp2;

    carry = 0;
    for (i = 0; i < wordLen; i++) {
        temp2 = a[i];
        temp = a[i] + b[i];
        out[i] = temp + carry;
        if (temp < temp2 || out[i] < carry) {
            carry = 1;
        } else {
            carry = 0;
        }
    }

    return PKE_SUCCESS;
#endif
}

/* function: out = a-b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a-b
 *     wordLen -------------------- input, word length of a, b, out
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure a > b
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_sub(const u32 *a, const u32 *b, u32 *out, u32 wordLen)
{
#if 0
    return pke_add_sub_internal(a, b, out, wordLen, MICROCODE_INTSUB);
#else
    u32 i, carry, tmp, tmp2;

    carry = 0;
    for (i = 0; i < wordLen; i++) {
        tmp = a[i] - b[i];
        tmp2 = tmp - carry;
        if (tmp > a[i] || tmp2 > tmp) {
            carry = 1;
        } else {
            carry = 0;
        }
        out[i] = tmp2;
    }

    return PKE_SUCCESS;
#endif
}

/* function: out = a*b
 * parameters:
 *     a -------------------------- input, integer a
 *     a_wordLen ------------------ input, word length of a
 *     b -------------------------- input, integer b
 *     b_wordLen ------------------ input, word length of b
 *     out ------------------------ output, out = a*b
 *     out_wordLen----------------- input, word length of out
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure out buffer word length is bigger than (2*max_bit_len(a,b)+0x1F)>>5
 *     2. please make sure a_wordLen/b_wordLen is not bigger than OPERAND_MAX_WORD_LEN/2
 */
u32 pke_mul_internal(const u32 *a, const u32 *b, u32 *out, u32 a_wordLen,
                     u32 b_wordLen, u32 out_wordLen)
{
    u32 step_bytes, step_words;
    u32 ret;

    step_bytes = pke_set_operand_width(out_wordLen << 5); //for pke lp
    //step_bytes = pke_set_operand_width(GET_MAX_LEN(out_wordLen<<5,512));  //for pke hp
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), (u32 *)a, a_wordLen); //A0 a
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)b, b_wordLen); //B0 b

    uint32_clear((u32 *)(PKE_A(0, step_bytes)) + a_wordLen,
                 step_words - a_wordLen);
    uint32_clear((u32 *)(PKE_B(0, step_bytes)) + b_wordLen,
                 step_words - b_wordLen);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_INTMUL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), a_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), b_wordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(1, step_bytes)), out,
                         out_wordLen); //A1 result

        return PKE_SUCCESS;
    }
}

/* function: out = a*b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b
 *     ab_wordLen ----------------- input, word length of a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure out buffer word length is bigger than (2*max_bit_len(a,b)+0x1F)>>5
 *     2. please make sure ab_wordLen is not bigger than OPERAND_MAX_WORD_LEN/2
 */
#if 1
u32 pke_mul(const u32 *a, const u32 *b, u32 *out, u32 ab_wordLen)
{
    u32 bitLen, tempLen;

    bitLen = get_valid_bits(a, ab_wordLen);
    tempLen = get_valid_bits(b, ab_wordLen);

    bitLen = GET_MAX_LEN(bitLen, tempLen);
    tempLen = GET_WORD_LEN(bitLen << 1);
    if (tempLen < (ab_wordLen << 1)) {
        tempLen = (ab_wordLen << 1) - 1;
    } else {
        tempLen = (ab_wordLen << 1);
    }

    return pke_mul_internal(a, b, out, ab_wordLen, ab_wordLen, tempLen);
}
#else
u32 pke_mul(const u32 *a, const u32 *b, u32 *out, u32 ab_wordLen)
{
    uint64_t UV;
    u32 i, j, *U, *V;
    u32 bitLen, tempLen;

    bitLen = get_valid_bits(a, ab_wordLen);
    tempLen = get_valid_bits(b, ab_wordLen);

    bitLen = GET_MAX_LEN(bitLen, tempLen);
    tempLen = GET_WORD_LEN(bitLen << 1);
    if (tempLen < (ab_wordLen << 1)) {
        tempLen = (ab_wordLen << 1) - 1;
    } else {
        tempLen = (ab_wordLen << 1);
    }

    uint32_clear(out, tempLen);

    V = (u32 *)(&UV);
    U = V + 1;
    for (i = 0; i < ab_wordLen; i++) {
        *U = 0;
        for (j = 0; j < ab_wordLen; j++) {
            UV = ((uint64_t)a[i]) * b[j] + out[i + j] + (*U);
            out[i + j] = (*V);
        }
        out[i + j] = (*U);
    }

    return PKE_SUCCESS;
}
#endif

/* function: calc n0(- modulus ^(-1) mod 2^w) for modMul, and pointMul. etc.
 * parameters: none
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling, please make sure the modulus is set in PKE_A(0)
 *     2. please make sure the modulus is odd, and word length of the modulus
 *        is not bigger than OPERAND_MAX_WORD_LEN
 *     3. the result is set in the internal register, no need to output.

u32 pke_pre_calc_mont_N0(void)
{
    return pke_set_micro_code_start_wait_return_code(MICROCODE_MGMR_PRE_N0);
} */

/* function: calc H(R^2 mod modulus) and n0'( - modulus ^(-1) mod 2^w ) for modMul,modExp, and pointMul. etc.
 *           here w is bit width of word, i,e. 32.
 * parameters:
 *     modulus -------------------- input, modulus
 *     bitLen --------------------- input, bit length of modulus
 *     H -------------------------- output, R^2 mod modulus
 *     n0 ------------------------- output,  - modulus ^(-1) mod 2^w, here w is 32 actually
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. please make sure word length of buffer H is equal to wordLen(word length of modulus),
 *        and n0 only need one word.
 *     3. bitLen must not be bigger than AIC_PKE_OPERAND_MAX_BIT_LEN
 */
u32 pke_pre_calc_mont(const u32 *modulus, u32 bitLen, u32 *H, u32 *n0)
{
    u32 step_bytes, step_words;
    u32 wordLen = GET_WORD_LEN(bitLen);
    u32 ret;

    step_bytes = pke_set_operand_width(bitLen);
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     wordLen); //B3 modulus

    if (step_words > wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + wordLen,
                     step_words - wordLen);
        uint32_clear((u32 *)(PKE_A(3, step_bytes)) + wordLen,
                     step_words - wordLen);
    } else {
        ;
    }

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MGMR_PRE);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(3, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(4, step_bytes)), 1 << 2);
#endif
        return ret;
    }

    if (NULL != H) {
        pke_read_operand((u32 *)(PKE_A(3, step_bytes)), H, wordLen); //A3 H
    } else {
        ;
    }

    if (NULL != n0) {
        pke_read_operand((u32 *)(PKE_B(4, step_bytes)), n0, 1); //B4 n0
    } else {
        ;
    }

    return PKE_SUCCESS;
}

/* function: like function pke_pre_calc_mont(), but this one is without output here
 * parameters:
 *     modulus -------------------- input, modulus
 *     wordLen -------------------- input, word length of modulus
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_pre_calc_mont_no_output(const u32 *modulus, u32 wordLen)
{
    return pke_pre_calc_mont(modulus, get_valid_bits(modulus, wordLen), NULL,
                             NULL);
}

/* function: load modulus and pre-calculated mont parameters H(R^2 mod modulus) and n0'(- modulus ^(-1) mod 2^w) for hardware operation
 * parameters:
 *     modulus -------------------- input, modulus
 *     modulus_h ------------------ input, R^2 mod modulus
 *     modulus_n0 ----------------- input, - modulus ^(-1) mod 2^w, here w is 32 actually
 *     bitLen --------------------- input, bit length of modulus
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. bitLen must not be bigger than AIC_PKE_OPERAND_MAX_BIT_LEN
 */
u32 pke_load_modulus_and_pre_monts(u32 *modulus, u32 *modulus_h,
                                   u32 *modulus_n0, u32 bitLen)
{
    u32 step_bytes, step_words;
    u32 wordLen = GET_WORD_LEN(bitLen);

    step_bytes = pke_set_operand_width(bitLen);
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     wordLen); //B3 modulus
    pke_load_operand((u32 *)(PKE_A(3, step_bytes)), modulus_h, wordLen); //A3 h
    if (step_words > wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + wordLen,
                     step_words - wordLen);
        uint32_clear((u32 *)(PKE_A(3, step_bytes)) + wordLen,
                     step_words - wordLen);
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(4, step_bytes)), modulus_n0, 1);

    return PKE_SUCCESS;
}

/* function: set modulus and pre-calculated mont parameters H(R^2 mod modulus) and n0'(- modulus ^(-1) mod 2^w) for hardware operation
 * parameters:
 *     modulus -------------------- input, modulus
 *     modulus_h ------------------ input, R^2 mod modulus
 *     modulus_n0 ----------------- input, - modulus ^(-1) mod 2^w, here w is 32 actually
 *     bitLen --------------------- input, bit length of modulus
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. bitLen must not be bigger than AIC_PKE_OPERAND_MAX_BIT_LEN
 */
u32 pke_set_modulus_and_pre_monts(u32 *modulus, u32 *modulus_h, u32 *modulus_n0,
                                  u32 bitLen)
{
    if ((NULL == modulus_h) || (NULL == modulus_n0)) {
        return pke_pre_calc_mont(modulus, bitLen, NULL, NULL);
    } else {
        return pke_load_modulus_and_pre_monts(modulus, modulus_h, modulus_n0,
                                              bitLen);
    }
}

/* function: out = a*b (mod modulus)
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. a, b must be less than modulus
 *     3. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 *     4. before calling this function, please make sure the modulus and the pre-calculated mont 
 *        arguments of modulus are located in the right address.
 */
u32 pke_modmul_internal(const u32 *a, const u32 *b, u32 *out, u32 wordLen)
{
    u32 step_bytes, step_words;
    u32 ret;

    //step_bytes = pke_set_operand_width(wordLen<<5);
    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), (u32 *)a, wordLen); //A0 a
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)b, wordLen); //B0 b
    if (step_words > wordLen) {
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + wordLen,
                     step_words - wordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + wordLen,
                     step_words - wordLen);
    } else {
        ;
    }

    //pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MODMUL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), wordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out, wordLen); //A0 out

        return PKE_SUCCESS;
    }
}

/* function: out = a*b mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. a, b must be less than modulus
 *     3. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u32 pke_modmul(const u32 *modulus, const u32 *a, const u32 *b, u32 *out,
               u32 wordLen)
{
    u32 ret;

    ret = pke_pre_calc_mont(modulus, get_valid_bits(modulus, wordLen), NULL,
                            NULL);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

        return pke_modmul_internal(a, b, out, wordLen);
    }
}

/* function: mod exponent, this could be used for rsa encrypting,decrypting,signing,verifing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling this function, please make sure the pre-calculated mont arguments 
 *        of modulus are located in the right address
 *     2. modulus must be odd
 *     3. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 */
u32 pke_modexp(const u32 *modulus, const u32 *exponent, const u32 *base,
               u32 *out, u32 mod_wordLen, u32 exp_wordLen)
{
    u32 step_bytes, step_words;
    u32 ret;

    step_bytes = pke_set_operand_width(mod_wordLen << 5);
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_A(1, step_bytes)), (u32 *)exponent,
                     exp_wordLen); //A1 exponent
    if (step_words > exp_wordLen) {
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     mod_wordLen); //B3 modulus
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)base,
                     mod_wordLen); //B0 base

    if (step_words > mod_wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_MODEXP);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MODEXP);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), exp_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), mod_wordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out,
                         mod_wordLen); //A0 result

        return PKE_SUCCESS;
    }
}

/* function: check input before mod exponent
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 * return: PKE_SUCCESS(input is valid, allow to calculate)
 *         PKE_FINISHED(mod exponent finished)
 *         other(error)
 * caution:
 *     1. modulus must be odd
 *     2. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 */
u32 pke_modexp_check_input(const u32 *modulus, const u32 *exponent,
                           const u32 *base, u32 *out, u32 mod_wordLen,
                           u32 exp_wordLen)
{
    int32_t flag;

    //base should be in [0,modulus]
    flag =
        uint32_BigNumCmp((u32 *)base, mod_wordLen, (u32 *)modulus, mod_wordLen);
    if (flag > 0) {
        return PKE_INVALID_INPUT;
    } else {
        ;
    }

    //if base is 0 or n
    if ((0 == flag) ||
        (1 == uint32_BigNum_Check_Zero((u32 *)base, mod_wordLen))) {
        if (uint32_BigNum_Check_Zero((u32 *)exponent, exp_wordLen)) //0^0 mod n
        {
            return PKE_INVALID_INPUT;
        } else //if a is 0, e is not 0, the output is 0
        {
            uint32_clear(out, mod_wordLen);
            return PKE_FINISHED;
        }
    } else if (uint32_BigNum_Check_Zero(
                   (u32 *)exponent,
                   exp_wordLen)) //base is in [1,modulus-1], e is 0, the output is 1
    {
        pke_set_operand_uint32_value(out, mod_wordLen, 1);
        return PKE_FINISHED;
    } else {
        ;
    }

    return PKE_SUCCESS;
}

/* function: mod exponent(for high level use, operands are all U8 big-endian big number), this could
 *     be used for rsa encrypting,decrypting,signing,verifing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_bitLen ----------------- input, real bit length of modulus and base number
 *     exp_bitLen ----------------- input, real bit length of exponent
 *     calc_pre_monts ------------- input, if it is 0, no need to calculate the pre-calculated mont arguments
 *                                  of modulus, otherwise calculate.
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. this is for high level application or protocol to use RSA mod exponent directly. all
 *        operands of this API are U8 big-endian big number.
 *     2. modulus must be odd
 *     3. please make sure exp_bitLen <= mod_bitLen <= AIC_PKE_OPERAND_MAX_BIT_LEN
 */
u32 pke_modexp_U8(const u8 *modulus, const u8 *exponent, const u8 *base,
                  u8 *out, u32 mod_bitLen, u32 exp_bitLen, u32 calc_pre_monts)
{
    u32 step_bytes, step_words;
    u32 mod_byteLen = GET_BYTE_LEN(mod_bitLen);
    u32 mod_wordLen = GET_WORD_LEN(mod_bitLen);
    u32 exp_byteLen = GET_BYTE_LEN(exp_bitLen);
    u32 exp_wordLen = GET_WORD_LEN(exp_bitLen);
    u32 ret;

    step_bytes = pke_set_operand_width(mod_bitLen);
    step_words = step_bytes >> 2;

    pke_load_operand_U8((u32 *)(PKE_B(3, step_bytes)), (u8 *)modulus,
                        mod_byteLen); //B3 modulus
    if (step_words > mod_wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
    } else {
        ;
    }

    if (calc_pre_monts) {
        pke_pre_calc_mont((u32 *)(PKE_B(3, step_bytes)), mod_bitLen, NULL,
                          NULL);
    } else {
        ;
    }

    pke_load_operand_U8((u32 *)(PKE_A(1, step_bytes)), (u8 *)exponent,
                        exp_byteLen); //A1 exponent
    if (step_words > exp_wordLen) {
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
    } else {
        ;
    }

    pke_load_operand_U8((u32 *)(PKE_B(0, step_bytes)), (u8 *)base,
                        mod_byteLen); //B0 base

    ret = pke_modexp_check_input((const u32 *)(PKE_B(3, step_bytes)),
                                 (const u32 *)(PKE_A(1, step_bytes)),
                                 (const u32 *)(PKE_B(0, step_bytes)),
                                 (u32 *)(PKE_A(0, step_bytes)), mod_wordLen,
                                 exp_wordLen);
    if (PKE_FINISHED == ret) {
        goto END;
    } else if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_MODEXP);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MODEXP);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), exp_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), mod_wordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

END:

    pke_read_operand_U8((u32 *)(PKE_A(0, step_bytes)), out,
                        mod_byteLen); //A0 result

    return PKE_SUCCESS;
}

/* function: c = a mod b
 * parameters:
 *     a -------------------------- input, integer a
 *     aWordLen ------------------- input, word length of integer
 *     b -------------------------- input, integer b, modulus
 *     b_h ------------------------ input, H parameter of b
 *     b_n0 ----------------------- input, - modulus ^(-1) mod 2^w, here w is 32 actually
 *     bWordLen ------------------- input, word length of integer b and b_h
 *     c -------------------------- output, c = a mod b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. b must be odd, and please make sure bWordLen is real word length of b
 *     2. real bit length of a can not be bigger than 2*(real bit length of b), so aWordLen can 
 *        not be bigger than 2*bWordLen
 *     3. pleae make sure aWordLen <= 2*OPERAND_MAX_WORD_LEN, bWordLen <= OPERAND_MAX_WORD_LEN
 */
u32 pke_mod(u32 *a, u32 aWordLen, u32 *b, u32 *b_h, u32 *b_n0, u32 bWordLen,
            u32 *c)
{
    u32 step_bytes;
    int32_t flag;
    u32 bBitLen, bitLen, tmpLen;
#ifdef PKE_RAM_GUARD
    u32 t1[OPERAND_MAX_WORD_LEN], t2[OPERAND_MAX_WORD_LEN];
#else
    u32 *t1, *t2;
#endif
    u32 ret;

    flag = uint32_BigNumCmp(a, aWordLen, b, bWordLen);
    if (flag < 0) {
        aWordLen = get_valid_words(a, aWordLen);
        uint32_copy(c, a, aWordLen);
        uint32_clear(c + aWordLen, bWordLen - aWordLen);

        return PKE_SUCCESS;
    } else if (0 == flag) {
        uint32_clear(c, bWordLen);

        return PKE_SUCCESS;
    } else {
        ;
    }

    bBitLen = get_valid_bits(b, bWordLen);
    step_bytes = pke_set_operand_width(bBitLen);

#ifndef PKE_RAM_GUARD
    t1 = (u32 *)(PKE_A(1, step_bytes));
    t2 = (u32 *)(PKE_B(2, step_bytes));
#endif

    bitLen = bBitLen & 0x1F;

    //get t2 = a high part mod b
    if (bitLen) {
        tmpLen = aWordLen - bWordLen + 1;
        uint32_copy(t2, a + bWordLen - 1, tmpLen);
        Big_Div2n(t2, tmpLen, bitLen);
        if (tmpLen < bWordLen) {
            uint32_clear(t2 + tmpLen, bWordLen - tmpLen);
        } else if (uint32_BigNumCmp(t2, bWordLen, b, bWordLen) >= 0) {
            ret = pke_sub(t2, b, t2, bWordLen);
            if (PKE_SUCCESS != ret) {
                return ret;
            } else {
                ;
            }
        } else {
            ;
        }
    } else {
        tmpLen = aWordLen - bWordLen;
        if (uint32_BigNumCmp(a + bWordLen, tmpLen, b, bWordLen) >= 0) {
            ret = pke_sub(a + bWordLen, b, t2, bWordLen);
            if (PKE_SUCCESS != ret) {
                return ret;
            } else {
                ;
            }
        } else {
            uint32_copy(t2, a + bWordLen, tmpLen);
            uint32_clear(t2 + tmpLen, bWordLen - tmpLen);
        }
    }

    //set the pre-calculated mont parameters
    ret = pke_set_modulus_and_pre_monts(b, b_h, b_n0, bBitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //get t1 = 1000...000 mod b
    uint32_clear(t1, bWordLen);
    if (bitLen) {
        t1[bWordLen - 1] = 1 << (bitLen);
    } else {
        ;
    }

    ret = pke_sub(t1, b, t1, bWordLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //get t2 = a_high * 1000..000 mod b
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_modmul_internal(t1, t2, t2, bWordLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //get t1 = a low part mod b
    if (bitLen) {
        uint32_copy(t1, a, bWordLen);
        t1[bWordLen - 1] &= ((1 << (bitLen)) - 1);
        if (uint32_BigNumCmp(t1, bWordLen, b, bWordLen) >= 0) {
            ret = pke_sub(t1, b, t1, bWordLen);
            if (PKE_SUCCESS != ret) {
                return ret;
            } else {
                ;
            }
        } else {
            ;
        }
    } else {
        if (uint32_BigNumCmp(a, bWordLen, b, bWordLen) >= 0) {
            ret = pke_sub(a, b, t1, bWordLen);
            if (PKE_SUCCESS != ret) {
                return ret;
            } else {
                ;
            }
        } else {
#ifdef PKE_RAM_GUARD
            uint32_copy(t1, a, bWordLen);
#else
            t1 = a;
#endif
        }
    }

    return pke_modadd(b, t1, t2, c, bWordLen);
}

/********************************** ECCp functions *************************************/

/* function: ECCP curve point mul(random point), Q=[k]P
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     k -------------------------- input, scalar
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 *     Qx ------------------------- output, x coordinate of point Q
 *     Qy ------------------------- output, y coordinate of point Q
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure k in [1,n-1], n is order of ECCP curve
 *     2. please make sure input point P is on the curve
 *     3. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 *     4. even if the input point P is valid, the output may be infinite point, in this case
 *        it will return error.
 */
u32 eccp_pointMul(eccp_curve_t *curve, u32 *k, u32 *Px, u32 *Py, u32 *Qx,
                  u32 *Qy)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    u32 nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->eccp_p, curve->eccp_p_h,
                                        curve->eccp_p_n0, curve->eccp_p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), Px, pWordLen); //B0 Px
    pke_load_operand((u32 *)(PKE_B(1, step_bytes)), Py, pWordLen); //B1 Py
    pke_load_operand((u32 *)(PKE_A(5, step_bytes)), curve->eccp_a,
                     pWordLen);                                   //A5 a
    pke_load_operand((u32 *)(PKE_A(4, step_bytes)), k, nWordLen); //A4 k
    pke_load_operand((u32 *)(PKE_B(5, step_bytes)), curve->eccp_n,
                     nWordLen); //B5 n

    if (step_words > pWordLen) {
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_B(1, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(5, step_bytes)) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    if (step_words > nWordLen) {
        uint32_clear((u32 *)(PKE_A(4, step_bytes)) + nWordLen,
                     step_words - nWordLen);
        uint32_clear((u32 *)(PKE_B(5, step_bytes)) + nWordLen,
                     step_words - nWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_ECCP_POINT_MUL);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_PMUL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(5, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(4, step_bytes)), nWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(5, step_bytes)), nWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(4, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

    pke_read_operand((u32 *)(PKE_A(0, step_bytes)), Qx, pWordLen); //A0 Qx
    if (NULL != Qy) {
        pke_read_operand((u32 *)(PKE_A(1, step_bytes)), Qy, pWordLen); //A1 Qy
    } else {
        ;
    }

    return PKE_SUCCESS;
}

/* function: ECCP curve point add, Q=P1+P2
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     P1x ------------------------ input, x coordinate of point P1
 *     P1y ------------------------ input, y coordinate of point P1
 *     P2x ------------------------ input, x coordinate of point P2
 *     P2y ------------------------ input, y coordinate of point P2
 *     Qx ------------------------- output, x coordinate of point Q=P1+P2
 *     Qy ------------------------- output, y coordinate of point Q=P1+P2
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P1 and P2 are both on the curve
 *     2. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 *     3. even if the input point P1 and P2 are valid, the output may be infinite point,
 *        in this case it will return error.
 */
u32 eccp_pointAdd(eccp_curve_t *curve, u32 *P1x, u32 *P1y, u32 *P2x, u32 *P2y,
                  u32 *Qx, u32 *Qy)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->eccp_p, curve->eccp_p_h,
                                        curve->eccp_p_n0, curve->eccp_p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    //pke_pre_calc_mont() may cover A1, so load A1(P1x) here
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), P1x, pWordLen); //A0 P1x
    pke_load_operand((u32 *)(PKE_A(1, step_bytes)), P1y, pWordLen); //A1 P1y
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), P2x, pWordLen); //B0 P2x
    pke_load_operand((u32 *)(PKE_B(1, step_bytes)), P2y, pWordLen); //B1 P2y
    pke_load_operand((u32 *)(PKE_A(5, step_bytes)), curve->eccp_a,
                     pWordLen); //A5 a

    if (step_words > pWordLen) {
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_B(1, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(5, step_bytes)) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_ECCP_POINT_ADD);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_PADD);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(5, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

    pke_read_operand((u32 *)(PKE_A(0, step_bytes)), Qx, pWordLen); //A0 Qx
    if (NULL != Qy) {
        pke_read_operand((u32 *)(PKE_A(1, step_bytes)), Qy, pWordLen); //A1 Qy
    } else {
        ;
    }

    return PKE_SUCCESS;
}

#ifdef ECCP_POINT_DOUBLE
/* function: ECCP curve point double, Q=[2]P
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 *     Qx ------------------------- output, x coordinate of point Q=[2]P
 *     Qy ------------------------- output, y coordinate of point Q=[2]P
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P is on the curve
 *     2. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 */
u32 eccp_pointDouble(eccp_curve_t *curve, u32 *Px, u32 *Py, u32 *Qx, u32 *Qy)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->eccp_p, curve->eccp_p_h,
                                        curve->eccp_p_n0, curve->eccp_p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    //pke_pre_calc_mont() may cover A1, so load A1(Px) and other paras here
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), Px, pWordLen); //A0 Px
    pke_load_operand((u32 *)(PKE_A(1, step_bytes)), Py, pWordLen); //A1 Py
    pke_load_operand((u32 *)(PKE_A(5, step_bytes)), curve->eccp_a,
                     pWordLen); //A5 a

    if (step_words > pWordLen) {
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(5, step_bytes)) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_ECCP_POINT_DBL);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_PDBL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(5, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), Qx, pWordLen); //A0 Qx
        pke_read_operand((u32 *)(PKE_A(1, step_bytes)), Qy, pWordLen); //A1 Qy

        return PKE_SUCCESS;
    }
}
#endif

/* function: check whether the input point P is on ECCP curve or not
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 * return: PKE_SUCCESS(success, on the curve), other(error or not on the curve)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 *     2. after calculation, A1 and A2 will be changed!
 */
u32 eccp_pointVerify(eccp_curve_t *curve, u32 *Px, u32 *Py)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->eccp_p, curve->eccp_p_h,
                                        curve->eccp_p_n0, curve->eccp_p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    //pke_pre_calc_mont() may cover A1, so load A1(Px) and other paras here
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), Px, pWordLen); //B0 Px
    pke_load_operand((u32 *)(PKE_B(1, step_bytes)), Py, pWordLen); //B1 Py
    pke_load_operand((u32 *)(PKE_A(5, step_bytes)), curve->eccp_a,
                     pWordLen); //A5 a
    pke_load_operand((u32 *)(PKE_A(4, step_bytes)), curve->eccp_b,
                     pWordLen); //A4 b

    if (step_words > pWordLen) {
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_B(1, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(5, step_bytes)) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)(PKE_A(4, step_bytes)) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_ECCP_POINT_VER);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_PVER);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(5, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(4, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(4, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        return PKE_SUCCESS;
    }
}

/* function: get ECCP public key from private key(the key pair could be used in SM2/ECDSA/ECDH, etc.)
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     priKey --------------------- input, private key, big-endian
 *     pubKey --------------------- output, public key, big-endian
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 */
u32 eccp_get_pubkey_from_prikey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey)
{
    u32 step_bytes;
    u32 nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
    u32 nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
    u32 pByteLen = GET_BYTE_LEN(curve->eccp_p_bitLen);
    u32 k[ECCP_MAX_WORD_LEN];
    u32 *x;
    u32 *y;
    u32 ret;

    step_bytes = pke_set_operand_width(curve->eccp_p_bitLen);
    x = (u32 *)(PKE_A(0, step_bytes));
    y = (u32 *)(PKE_A(1, step_bytes));

    k[nWordLen - 1] = 0; //clear if curve->eccp_n_bitLen is not a multiple of 32
    reverse_byte_array(priKey, (u8 *)k, nByteLen);

    //make sure k in [1, n-1]
    ret = uint32_integer_check(k, curve->eccp_n, nWordLen, PKE_ZERO_ALL,
                               PKE_INTEGER_TOO_BIG, PKE_SUCCESS);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

#ifdef AIC_PKE_SM2_SUPPORT
    if (curve == sm2_curve) {
        if ((k[0] == sm2_curve->eccp_n[0] - 1) &&
            (0 == uint32_BigNumCmp(k + 1, nWordLen - 1, (curve->eccp_n) + 1,
                                   nWordLen - 1))) {
            return PKE_INTEGER_TOO_BIG;
        } else {
            ;
        }
    } else {
        ;
    }
#endif

    //get pubKey
    ret = eccp_pointMul(curve, k, curve->eccp_Gx, curve->eccp_Gy, x, y);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        reverse_byte_array((u8 *)x, pubKey, pByteLen);
        reverse_byte_array((u8 *)y, pubKey + pByteLen, pByteLen);

        return PKE_SUCCESS;
    }
}

/* function: get ECCP key pair(the key pair could be used in SM2/ECDSA/ECDH)
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     priKey --------------------- output, private key, big-endian
 *     pubKey --------------------- output, public key, big-endian
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 */
u32 eccp_getkey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey)
{
    u32 tmpLen;
    u32 nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
    u32 ret;

ECCP_GETKEY_LOOP:

    ret = get_rand(priKey, nByteLen);
    if (TRNG_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //make sure k has the same bit length as n
    tmpLen = (curve->eccp_n_bitLen) & 7;
    if (tmpLen) {
        priKey[0] &= (1 << (tmpLen)) - 1;
    } else {
        ;
    }

    ret = eccp_get_pubkey_from_prikey(curve, priKey, pubKey);
    if (PKE_ZERO_ALL == ret || PKE_INTEGER_TOO_BIG == ret) {
        goto ECCP_GETKEY_LOOP;
    } else {
        return ret;
    }
}

/****************************** ECCp functions finished ********************************/

#ifdef SUPPORT_C25519
/**************************** X25519 & Ed25519 functions *******************************/

/* function: c25519 point mul(random point), Q=[k]P
 * parameters:
 *     curve ---------------------- input, c25519 curve struct pointer
 *     k -------------------------- input, scalar
 *     Pu ------------------------- input, u coordinate of point P
 *     Qu ------------------------- output, u coordinate of point Q
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P is on the curve
 *     2. even if the input point P is valid, the output may be infinite point, in this case return error.
 *     3. please make sure the curve is c25519
 */
u32 x25519_pointMul(mont_curve_t *curve, u32 *k, u32 *Pu, u32 *Qu)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->p_bitLen);
    u32 nWordLen = GET_WORD_LEN(curve->n_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->p, curve->p_h, curve->p_n0,
                                        curve->p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)PKE_A(0, step_bytes), Pu, pWordLen); //A0 Pu
    pke_load_operand((u32 *)PKE_B(0, step_bytes), curve->a24,
                     pWordLen);                                 //B0 a24
    pke_load_operand((u32 *)PKE_A(4, step_bytes), k, nWordLen); //A4 k

    if (step_words > pWordLen) {
        uint32_clear((u32 *)PKE_A(0, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(0, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(3, step_bytes) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    if (step_words > nWordLen) {
        uint32_clear((u32 *)PKE_A(4, step_bytes) + nWordLen,
                     step_words - nWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_C25519_PMUL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(4, step_bytes)), nWordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

    pke_read_operand((u32 *)PKE_A(1, step_bytes), Qu, pWordLen); //A1 Qu

    return PKE_SUCCESS;
}

#if 0
/* function: out = a^b mod n
 * parameters:
 *     a -------------------------- input, base number, 8 words
 *     b -------------------------- input, exponent number, 8 words
 *     n -------------------------- input, modulus number, 8 words
 *     out ------------------------ output, out = a^b mod n
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure n is odd, b is not zero
 *     2. this function is used in Ed25519 to decode point
 */
u32 mod_exp(u32 a[8], u32 b[8], u32 n[8], u32 out[8])
{
    u32 t[8];
    int32_t cfg_bak, bitLen;
    u32 ret;

    pke_pre_calc_mont(n, 256, NULL, NULL);

    cfg_bak = PKE_EXE_CONF;
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_MONT);

    //t = A0 = aR mod n
    ret = pke_modmul_internal(a, (u32 *)(PKE_A(3,step_bytes)), t, Ed25519_WORD_LEN);     //A3: R^2 mod n
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    bitLen = get_valid_bits(b, Ed25519_WORD_LEN);
    bitLen -= 2;
    for(; bitLen>=0; bitLen--)
    {
        ret = pke_modmul_internal((u32 *)(PKE_A(0,step_bytes)), (u32 *)(PKE_A(0,step_bytes)), (u32 *)(PKE_A(0,step_bytes)), Ed25519_WORD_LEN);
        if(PKE_SUCCESS != ret)
        {
            return ret;
        }
        else if(b[bitLen/32] & (1<<(bitLen&31)))
        {
            ret = pke_modmul_internal((u32 *)(PKE_A(0,step_bytes)), t, (u32 *)(PKE_A(0,step_bytes)), Ed25519_WORD_LEN);
            if(PKE_SUCCESS != ret)
            {
                return ret;
            }
            else
            {;}
        }
        else
        {;}
    }

    //t = 1
    pke_set_operand_uint32_value(t, 8, 1);

    //get result
    ret = pke_modmul_internal((u32 *)(PKE_A(0,step_bytes)), t, out, Ed25519_WORD_LEN);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    pke_set_exe_cfg(cfg_bak);

    return PKE_SUCCESS;
}
#endif

/* function: Ed25519 decode point
 * parameters:
 *     in_y ----------------------- input, encoded Ed25519 point
 *     out_x ---------------------- output, x coordinate of input point
 *     out_y ---------------------- output, y coordinate of input point
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1.
 */
u32 ed25519_decode_point(u8 in_y[32], u8 out_x[32], u8 out_y[32])
{
    u32 u[Ed25519_WORD_LEN];
    u32 v[Ed25519_WORD_LEN];
    u32 t[Ed25519_WORD_LEN] = { 0 };
    u32 t2[Ed25519_WORD_LEN];
    u32 t3[Ed25519_WORD_LEN];
    u32 ret;

    //get y
    memcpy_((u8 *)u, in_y, Ed25519_BYTE_LEN);
    u[Ed25519_WORD_LEN - 1] &= 0x7FFFFFFF;

    //make sure y < prime p
    if (uint32_BigNumCmp(u, Ed25519_WORD_LEN, ed25519->p, Ed25519_WORD_LEN) >=
        0) {
        return PKE_INVALID_INPUT;
    } else {
        ;
    }

    //set type
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

    //set pre-calculated paras
    ret = pke_set_modulus_and_pre_monts(ed25519->p, ed25519->p_h, ed25519->p_n0,
                                        ed25519->p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(u, u, v, Ed25519_WORD_LEN); //v = y^2
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    t[0] = 1;
    ret = pke_modsub(ed25519->p, v, t, u, Ed25519_WORD_LEN); //u = y^2 - 1
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(ed25519->d, v, v, Ed25519_WORD_LEN); //v = d*y^2
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modadd(ed25519->p, v, t, v, Ed25519_WORD_LEN); //v = d*y^2 + 1
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(v, v, t2, Ed25519_WORD_LEN); //t2 = v^2
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(v, t2, t3, Ed25519_WORD_LEN); //t3 = v^3
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t3, u, t, Ed25519_WORD_LEN); //t = u*v^3
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t2, t2, t2, Ed25519_WORD_LEN); //t2 = v^4
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t2, t3, t2, Ed25519_WORD_LEN); //t2 = v^7
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t2, u, t2, Ed25519_WORD_LEN); //t2 = u*v^7
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //t3 = (p-5)/8
    uint32_copy(t3, ed25519->p, Ed25519_WORD_LEN);
    t3[0] -= 5;
    Big_Div2n(t3, Ed25519_WORD_LEN, 3);

    //t2 = (u*v^7 )^((p-5)/8)
#if 0
    ret = mod_exp(t2, t3, ed25519->p, t2);
#else
    ret =
        pke_modexp(ed25519->p, t3, t2, t2, Ed25519_WORD_LEN, Ed25519_WORD_LEN);
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(
        t2, t, t, Ed25519_WORD_LEN); //t = x = (u*v^3)*(u*v^7 )^((p-5)/8)
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t, t, t2, Ed25519_WORD_LEN); //t2 = x^2
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    ret = pke_modmul_internal(t2, v, t2, Ed25519_WORD_LEN); //t2 = v*x^2
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    if (0 == uint32_BigNumCmp(
                 t2, Ed25519_WORD_LEN, u,
                 Ed25519_WORD_LEN)) //if v x^2 = u (mod p), x is a square root.
    {
        goto result;
    } else {
        ;
    }

    ret = pke_sub(ed25519->p, u, t3, Ed25519_WORD_LEN); //t3 = -u mod p
    if (PKE_SUCCESS != ret) {
        return ret;
    } else if (0 ==
               uint32_BigNumCmp(t2, Ed25519_WORD_LEN, t3, Ed25519_WORD_LEN)) {
        //v = (p-1)/4
        uint32_copy(v, ed25519->p, Ed25519_WORD_LEN);
        v[0] -= 1;
        Big_Div2n(v, Ed25519_WORD_LEN, 2);

        //t2 = 2
        pke_set_operand_uint32_value(t2, Ed25519_WORD_LEN, 2);

        //u = 2^((p-1)/4)
#if 0
        ret = mod_exp(t2, v, ed25519->p, u);
#else
        ret = pke_modexp(ed25519->p, v, t2, u, Ed25519_WORD_LEN,
                         Ed25519_WORD_LEN);
        pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
#endif
        if (PKE_SUCCESS != ret) {
            return ret;
        } else {
            ;
        }

        ret = pke_modmul_internal(t, u, t,
                                  Ed25519_WORD_LEN); //t = x*(2^((p-1)/4))
        if (PKE_SUCCESS != ret) {
            return ret;
        } else {
            ;
        }

        goto result;
    } else {
        ;
    }

    return PKE_INVALID_INPUT; //root not exist

result:

    //if x=0 and x is odd, decode fail
    if (uint32_BigNum_Check_Zero(t, Ed25519_WORD_LEN) &&
        (in_y[Ed25519_BYTE_LEN - 1] & 0x80)) {
        return PKE_INVALID_INPUT;
    } else {
        ;
    }

    //get out_x
    if ((u8)((t[0] & 1) << 7) == (in_y[Ed25519_BYTE_LEN - 1] & 0x80)) {
        memcpy_(out_x, (u8 *)t, Ed25519_BYTE_LEN);
    } else {
        ret = pke_sub(ed25519->p, t, v, Ed25519_WORD_LEN); //v = -x mod p
        if (PKE_SUCCESS != ret) {
            return ret;
        } else {
            memcpy_(out_x, (u8 *)v, Ed25519_BYTE_LEN);
        }
    }

    //get out_y
    memcpy_(out_y, in_y, Ed25519_BYTE_LEN);
    out_y[Ed25519_BYTE_LEN - 1] &= 0x7F;

    return PKE_SUCCESS;
}

/* function: edwards25519 curve point mul(random point), Q=[k]P
 * parameters:
 *     curve ---------------------- input, edwards25519 curve struct pointer
 *     k -------------------------- input, scalar
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 *     Qx ------------------------- output, x coordinate of point Q
 *     Qy ------------------------- output, y coordinate of point Q
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P is on the curve
 *     2. even if the input point P is valid, the output may be neutral point (0, 1), it is valid
 *     3. please make sure the curve is edwards25519
 *     4. k could not be zero now.
 */
u32 ed25519_pointMul(edward_curve_t *curve, u32 *k, u32 *Px, u32 *Py, u32 *Qx,
                     u32 *Qy)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->p_bitLen);
    u32 nWordLen = GET_WORD_LEN(curve->n_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->p, curve->p_h, curve->p_n0,
                                        curve->p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)PKE_A(1, step_bytes), Px, pWordLen);       //A1 Px
    pke_load_operand((u32 *)PKE_A(2, step_bytes), Py, pWordLen);       //A2 Py
    pke_load_operand((u32 *)PKE_B(0, step_bytes), curve->d, pWordLen); //B0 d
    pke_load_operand((u32 *)PKE_A(0, step_bytes), k, nWordLen);        //A0 k

    if (step_words > pWordLen) {
        uint32_clear((u32 *)PKE_A(1, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_A(2, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(3, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_A(3, step_bytes) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    if (step_words > nWordLen) {
        uint32_clear((u32 *)PKE_A(0, step_bytes) + nWordLen,
                     step_words - nWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_Ed25519_PMUL);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), nWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(2, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(3, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

    pke_read_operand((u32 *)PKE_A(1, step_bytes), Qx, pWordLen); //A1 Qx
    if (NULL != Qy) {
        pke_read_operand((u32 *)PKE_A(2, step_bytes), Qy, pWordLen); //A2 Qx
    } else {
        ;
    }

    return PKE_SUCCESS;
}

/* function: edwards25519 point add, Q=P1+P2
 * parameters:
 *     curve ---------------------- input, edwards25519 curve struct pointer
 *     P1x ------------------------ input, x coordinate of point P1
 *     P1y ------------------------ input, y coordinate of point P1
 *     P2x ------------------------ input, x coordinate of point P2
 *     P2y ------------------------ input, y coordinate of point P2
 *     Qx ------------------------- output, x coordinate of point Q=P1+P2
 *     Qy ------------------------- output, y coordinate of point Q=P1+P2
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P1 and P2 are both on the curve
 *     2. the output point may be neutral point (0, 1), it is valid
 *     3. please make sure the curve is edwards25519
 */
u32 ed25519_pointAdd(edward_curve_t *curve, u32 *P1x, u32 *P1y, u32 *P2x,
                     u32 *P2y, u32 *Qx, u32 *Qy)
{
    u32 step_bytes, step_words;
    u32 pWordLen = GET_WORD_LEN(curve->p_bitLen);
    u32 ret;

    //set ecc_p, ecc_p_h, ecc_p_n0, etc.
    ret = pke_set_modulus_and_pre_monts(curve->p, curve->p_h, curve->p_n0,
                                        curve->p_bitLen);
    if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    step_bytes = pke_get_operand_bytes();
    step_words = step_bytes >> 2;

    //pke_pre_calc_mont() may cover some addresses, so load parameters here
    pke_load_operand((u32 *)PKE_A(1, step_bytes), P1x, pWordLen);      //A1 P1x
    pke_load_operand((u32 *)PKE_A(2, step_bytes), P1y, pWordLen);      //A2 P1y
    pke_load_operand((u32 *)PKE_B(1, step_bytes), P2x, pWordLen);      //B1 P2x
    pke_load_operand((u32 *)PKE_B(2, step_bytes), P2y, pWordLen);      //B2 P2y
    pke_load_operand((u32 *)PKE_B(0, step_bytes), curve->d, pWordLen); //B0 d

    if (step_words > pWordLen) {
        uint32_clear((u32 *)PKE_A(1, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_A(2, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(1, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(2, step_bytes) + pWordLen,
                     step_words - pWordLen);
        uint32_clear((u32 *)PKE_B(0, step_bytes) + pWordLen,
                     step_words - pWordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_Ed25519_PADD);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_A(2, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(1, step_bytes)), pWordLen << 2);
        get_rand_fast((u8 *)(PKE_B(2, step_bytes)), pWordLen << 2);
#endif
        return ret;
    } else {
        ;
    }

    pke_read_operand((u32 *)PKE_A(1, step_bytes), Qx, pWordLen); //A1 Qx
    pke_read_operand((u32 *)PKE_A(2, step_bytes), Qy, pWordLen); //A2 Qy

    return PKE_SUCCESS;
}

/**************************** X25519 & Ed25519 finished ********************************/
#endif

#ifdef AIC_PKE_SEC
/*********************************** sec functions *************************************/

/* function: pke sec init
 * parameters: none
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 */
u32 pke_sec_init(void)
{
    u32 rand[4];

    if (TRNG_SUCCESS != get_rand((u8 *)&rand, 16)) {
        return PKE_STOP;
    } else {
        ;
    }

    writel(rand[0], PKE_BASE + PKE_RAND_SEED);
    writel(0, PKE_BASE + PKE_RC_EN);
    writel(rand[1], PKE_BASE + PKE_RC_KEY);
    writel(rand[2], PKE_BASE + PKE_RC_D_NONCE);
    writel(rand[3], PKE_BASE + PKE_RC_A_NONCE);
    writel(1, PKE_BASE + PKE_RC_EN);

    return PKE_SUCCESS;
}

/* function: pke sec uninit
 * parameters: none
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 */
u32 pke_sec_uninit(void)
{
    writel(0, PKE_BASE + PKE_RC_EN);

    return PKE_SUCCESS;
}

/* function: mod exponent, this could be used for rsa encrypting,decrypting,signing,verifing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling this function, please make sure the pre-calculated mont arguments 
 *        of modulus are located in the right address.
 *     2. modulus must be odd
 *     3. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 */
u32 pke_modexp_ladder(const u32 *modulus, const u32 *exponent, const u32 *base,
                      u32 *out, u32 mod_wordLen, u32 exp_wordLen)
{
    u32 step_bytes, step_words;
    u32 ret;

    step_bytes = pke_set_operand_width(mod_wordLen << 5);
    step_words = step_bytes >> 2;

    pke_load_operand((u32 *)(PKE_A(1, step_bytes)), (u32 *)exponent,
                     exp_wordLen); //A1 exponent
    if (step_words > exp_wordLen) {
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(3, step_bytes)), (u32 *)modulus,
                     mod_wordLen); //B3 modulus
    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)base,
                     mod_wordLen); //B0 base
    if (step_words > mod_wordLen) {
        uint32_clear((u32 *)(PKE_B(3, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(PKE_EXE_CFG_MODEXP_MONT_LADDER);

    ret = pke_set_micro_code_start_wait_return_code(MICROCODE_MODEXP);
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), exp_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), mod_wordLen << 2);
#endif
        return ret;
    } else {
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out,
                         mod_wordLen); //A0 result

        return PKE_SUCCESS;
    }
}

/* function: mod exponent with private key and public key, this could be used for rsa decrypting,signing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent, actually private key d
 *     pub ------------------------ input, public key e
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 *     pub_wordLen ---------------- input, word length of pub
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling this function, please make sure the pre-calculated mont arguments 
 *        of modulus are located in the right address.
 *     2. modulus must be odd
 *     3. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 *     4. please make sure pub_wordLen <= 2
 *     5. please make sure value of exponent should be bigger than 1
 */
u32 pke_modexp_with_pub(const u32 *modulus, const u32 *exponent, const u32 *pub,
                        const u32 *base, u32 *out, u32 mod_wordLen,
                        u32 exp_wordLen, u32 pub_wordLen)
{
    u32 step_bytes, step_words;
    u32 exp_bitLen = get_valid_bits(exponent, exp_wordLen);
    u32 bak_n0_inverse, ret;

    step_bytes = pke_set_operand_width(mod_wordLen << 5);
    step_words = step_bytes >> 2;

    bak_n0_inverse = *((volatile u32 *)((PKE_B(4, step_bytes))));

    exp_wordLen = GET_WORD_LEN(exp_bitLen);

    ret = uint32_get_rand_big_number_msb_0((u32 *)(PKE_A(1, step_bytes)),
                                           exp_bitLen); //A1 d2
    if (ret) {
        return ret;
    } else {
        ;
    }

    //set d
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), (u32 *)exponent,
                     exp_wordLen);
    if (step_words > exp_wordLen) {
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
    } else {
        ;
    }

    //set B3 modulus, and get A0 d1
    //for modulus > exponent
    ret = pke_modadd_modsub_internal(modulus, (u32 *)(PKE_A(0, step_bytes)),
                                     (u32 *)(PKE_A(1, step_bytes)),
                                     (u32 *)(PKE_A(0, step_bytes)), mod_wordLen,
                                     MICROCODE_MODSUB);
    if (PKE_SUCCESS != ret) {
        goto END;
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(1, step_bytes)), (u32 *)pub,
                     pub_wordLen); //B1 pub
    if (step_words > pub_wordLen) {
        uint32_clear((u32 *)(PKE_B(1, step_bytes)) + pub_wordLen,
                     step_words - pub_wordLen);
    } else {
        ;
    }

    pke_load_operand((u32 *)(PKE_B(0, step_bytes)), (u32 *)base,
                     mod_wordLen); //B0 base
    if (step_words > mod_wordLen) {
        uint32_clear((u32 *)(PKE_B(0, step_bytes)) + mod_wordLen,
                     step_words - mod_wordLen);
    } else {
        ;
    }

    pke_set_exe_cfg(
        PKE_EXE_CFG_MODEXP_WITH_PUB); //print_BN_buf_U32((u32 *)(PKE_B(4,step_bytes)), 1, "B4-------1");

    ret = pke_set_micro_code_start_wait_return_code(
        MICROCODE_MODEXP); //print_BN_buf_U32((u32 *)(PKE_B(4,step_bytes)), 1, "B4-----------2");

END:
    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), exp_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(3, step_bytes)), mod_wordLen << 2);
#endif
    } else {
        *((volatile u32 *)((PKE_B(4, step_bytes)))) = bak_n0_inverse;
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out,
                         mod_wordLen); //A0 result
    }

    return ret;
}

/* function: mod exponent, this could be used for rsa encrypting,decrypting,signing,verifing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent, actually private key d
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = base^(exponent) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling this function, please make sure the pre-calculated mont arguments
 *        of modulus are located in the right address.
 *     2. modulus must be odd
 *     3. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 *     4. please make sure value of exponent should be bigger than 1
 */
u32 pke_modexp_without_pub(const u32 *modulus, const u32 *exponent,
                           const u32 *base, u32 *out, u32 mod_wordLen,
                           u32 exp_wordLen)
{
    u32 step_bytes, step_words;
    u32 bitLen;
    u32 bak_n0_inverse, ret;

    step_bytes = pke_set_operand_width(mod_wordLen << 5);
    step_words = step_bytes >> 2;

    bak_n0_inverse = *((volatile u32 *)((PKE_B(4, step_bytes))));

    bitLen = get_valid_bits(modulus, mod_wordLen);

GET_RAND:

    //A4 get r < n
    ret =
        uint32_get_rand_big_number_msb_0((u32 *)(PKE_A(4, step_bytes)), bitLen);
    if (ret) {
        return ret;
    } else {
        ;
    }

    //set B3 modulus, and get A2 r_inv
    ret = pke_modinv((u32 *)modulus, (u32 *)(PKE_A(4, step_bytes)),
                     (u32 *)(PKE_A(2, step_bytes)), mod_wordLen, mod_wordLen);
    if (PKE_NO_MODINV == ret) {
        goto GET_RAND;
    } else if (PKE_SUCCESS != ret) {
        return ret;
    } else {
        ;
    }

    //A1 d1
    bitLen = get_valid_bits(exponent, exp_wordLen);
    exp_wordLen = GET_WORD_LEN(bitLen);

    ret =
        uint32_get_rand_big_number_msb_0((u32 *)(PKE_A(1, step_bytes)), bitLen);
    if (ret) {
        return ret;
    } else {
        ;
    }

    //A0 d
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)), (u32 *)exponent,
                     exp_wordLen);
    if (step_words > exp_wordLen) {
        uint32_clear((u32 *)(PKE_A(0, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
        uint32_clear((u32 *)(PKE_A(1, step_bytes)) + exp_wordLen,
                     step_words - exp_wordLen);
    } else {
        ;
    }

    //B1 d2
    //for modulus > exponent
    ret = pke_modadd_modsub_internal((u32 *)(PKE_B(3, step_bytes)),
                                     (u32 *)(PKE_A(0, step_bytes)),
                                     (u32 *)(PKE_A(1, step_bytes)),
                                     (u32 *)(PKE_B(1, step_bytes)), mod_wordLen,
                                     MICROCODE_MODSUB);
    if (PKE_SUCCESS != ret) {
        goto END;
    } else {
        ;
    }

    //B0  c*r % n
    pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
    ret = pke_modmul_internal((u32 *)(PKE_A(4, step_bytes)), base,
                              (u32 *)(PKE_B(0, step_bytes)), mod_wordLen);
    if (PKE_SUCCESS != ret) {
        goto END;
    } else {
        ;
    }

    //A0 r
    pke_load_operand((u32 *)(PKE_A(0, step_bytes)),
                     (u32 *)(PKE_A(4, step_bytes)), mod_wordLen);

    pke_set_exe_cfg(
        PKE_EXE_CFG_MODEXP_WITHOUT_PUB); //print_BN_buf_U32((u32 *)(PKE_B(4,step_bytes)), 1, "B4-------1");

    ret = pke_set_micro_code_start_wait_return_code(
        MICROCODE_MODEXP); //print_BN_buf_U32((u32 *)(PKE_B(4,step_bytes)), 1, "B4-----------2");

END:

    if (PKE_SUCCESS != ret) {
#ifdef AIC_PKE_SEC
        get_rand_fast((u8 *)(PKE_A(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(1, step_bytes)), exp_wordLen << 2);
        get_rand_fast((u8 *)(PKE_A(2, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(0, step_bytes)), mod_wordLen << 2);
        get_rand_fast((u8 *)(PKE_B(1, step_bytes)), exp_wordLen << 2);
#endif
    } else {
        *((volatile u32 *)((PKE_B(4, step_bytes)))) = bak_n0_inverse;
        pke_read_operand((u32 *)(PKE_A(0, step_bytes)), out,
                         mod_wordLen); //A0 result
    }

    return ret;
}

/* function: ECCP curve sec point mul, Q=[k]P, P is a random point on curve
 * parameters:
 *     curve ---------------------- input, eccp_curve_t curve struct pointer
 *     k -------------------------- input, scalar
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 *     Qx ------------------------- output, x coordinate of point Q
 *     Qy ------------------------- output, y coordinate of point Q
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure k in [1,n-1], n is order of ECCP curve
 *     2. please make sure input point P is on the curve
 *     3. please make sure bit length of the curve is not bigger than AIC_PKE_ECCP_MAX_BIT_LEN
 */
u32 eccp_pointMul_sec(eccp_curve_t *curve, u32 *k, u32 *Px, u32 *Py, u32 *Qx,
                      u32 *Qy)
{
    return eccp_pointMul(curve, k, Px, Py, Qx, Qy);
}

#endif

