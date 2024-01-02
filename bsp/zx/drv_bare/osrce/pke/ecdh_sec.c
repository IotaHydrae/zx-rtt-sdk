#if (defined(AIC_PKE_SEC) && defined(ECDH_SEC))

#include <hal_pke_ecdh.h>
#include <utility_sec.h>
#include <hal_pke.h>
#include <trng.h>
#include "eccp_sec_common.h"

/* Function: ECDH compute key
 * Parameters:
 *     local_prikey --------------- input, local private key, big-endian
 *     peer_pubkey ---------------- input, peer public key, big-endian
 *     key ------------------------ output, output key
 *     keyByteLen ----------------- input, byte length of output key
 *     KDF ------------------------ input, KDF function to get key
 * Return:
 *     ECDH_SUCCESS_S(success); other(error)
 * Caution:
 */
u32 ecdh_compute_key_s(eccp_curve_t *curve, u8 *local_prikey, u8 *peer_pubkey, u8 *key,
        u32 keyByteLen, KDF_FUNC kdf)
{
    u32 k[ECCP_MAX_WORD_LEN] = {0};
    u32 Px[ECCP_MAX_WORD_LEN] = {0};
    u32 Py[ECCP_MAX_WORD_LEN] = {0};
    u32 pByteLen = 0, pWordLen = 0, nByteLen = 0, nWordLen = 0;
    u32 tmp_step;
    u32 ret = ECDH_ERROR_S;
    u16 eccp_curve_crc16;
    eccp_curve_t * curve1;
    eccp_sec_ctx_t ctx[1];

    if(NULL == curve || NULL == local_prikey || NULL == peer_pubkey || NULL == key)
    {
        return ECDH_ERROR_S;
    }
    else if(0 == keyByteLen)
    {
        return ECDH_ERROR_S;
    }
    else
    {;}

    //init curve
    curve1 = eccp_curve_init(ctx, (const eccp_curve_t *)curve);
    if(NULL == curve1)
    {
        return ECDH_ERROR_S;
    }
    else
    {;}

    //check crc16 of curve paras
    eccp_curve_crc16 = calc_eccp_curve_crc16(curve);
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDH_ERROR_S;
        goto END;
    }
    else
    {;}

    pByteLen = GET_BYTE_LEN(curve1->eccp_p_bitLen);
    pWordLen = GET_WORD_LEN(curve1->eccp_p_bitLen);
    nByteLen = GET_BYTE_LEN(curve1->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve1->eccp_n_bitLen);

    //make sure private key is in [1, n-1]
    reverse_byte_array((u8 *)local_prikey, (u8 *)k, nByteLen);
    ret = uint32_integer_check_sec(k, curve1->eccp_n, nWordLen, ECDH_ERROR_S, ECDH_ERROR_S,
            ECDH_SUCCESS_S);
    if(ECDH_SUCCESS_S != ret)
    {
        ret = ECDH_ERROR_S;
        goto END;
    }
    else
    {;}

    //check public key
    reverse_byte_array(peer_pubkey, (u8 *)Px, pByteLen);
    reverse_byte_array(peer_pubkey+pByteLen, (u8 *)Py, pByteLen);

    ret = eccp_pointVerify(curve1, Px, Py);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDH_ERROR_S;
        goto END;
    }
    else
    {;}

    tmp_step = pke_get_operand_bytes();

#ifndef PKE_RAM_GUARD
    //copy back curve paras that not covered by output
    uint32_copy(curve1->eccp_b,   (u32 *)(PKE_A(4,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
#endif

    ret = eccp_pointMul_sec(curve1, k, Px, Py, Px, NULL);
    if(PKE_SUCCESS != ret)
    {
        ret = ECDH_ERROR_S;
        goto END;
    }
    else
    {;}

#ifndef PKE_RAM_GUARD
    //copy back curve paras that not covered by output
    uint32_copy(curve1->eccp_p,   (u32 *)(PKE_B(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_h, (u32 *)(PKE_A(3,tmp_step)), pWordLen);
    uint32_copy(curve1->eccp_p_n0,(u32 *)(PKE_B(4,tmp_step)), 1);
    uint32_copy(curve1->eccp_n,   (u32 *)(PKE_B(5,tmp_step)), nWordLen);
#endif

    reverse_byte_array((u8 *)Px, (u8 *)Px, pByteLen);

    if(kdf)
    {
        kdf(Px, pByteLen, key, keyByteLen);
    }
    else
    {
        if(keyByteLen > pByteLen)
        {
            keyByteLen = pByteLen;
        }
        else
        {;}

        memcpy_(key, (u8 *)Px, keyByteLen);
    }

    //check crc16 of curve paras
    if(0 != ecc_crc16_check(curve1, eccp_curve_crc16))
    {
        ret = ECDH_ERROR_S;
        goto END;
    }
    else
    {;}

    ret = ECDH_SUCCESS_S;

END:

    eccp_curve_uninit(ctx);
    if(ECDH_SUCCESS_S != ret)
    {
        (void)get_rand_fast((u8 *)key, keyByteLen);
    }
    else
    {;}

    (void)get_rand_fast((u8 *)k, nByteLen);
    (void)get_rand_fast((u8 *)Px, pByteLen);
    (void)get_rand_fast((u8 *)Py, pByteLen);

    return ret;
}
#endif

