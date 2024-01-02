#ifdef AIC_PKE_ECDH_SUPPORT

#include <hal_pke_ecdh.h>
#include <utility.h>

/* Function: ECDH compute key
 * Parameters:
 *     local_prikey --------------- input, local private key, big-endian
 *     peer_pubkey ---------------- input, peer public key, big-endian
 *     key ------------------------ output, output key
 *     keyByteLen ----------------- input, byte length of output key
 *     KDF ------------------------ input, KDF function to get key
 * Return:
 *     ECDH_SUCCESS(success); other(error)
 * Caution:
 */
u32 ecdh_compute_key(eccp_curve_t *curve, u8 *local_prikey, u8 *peer_pubkey, u8 *key,
        u32 keyByteLen, KDF_FUNC kdf)
{
    u32 k[ECCP_MAX_WORD_LEN] = {0};
    u32 Px[ECCP_MAX_WORD_LEN] = {0};
    u32 Py[ECCP_MAX_WORD_LEN] = {0};
    u32 pByteLen, nByteLen, nWordLen;
    u32 ret;

    if(NULL == curve || NULL == local_prikey || NULL == peer_pubkey || NULL == key)
    {
        return ECDH_POINTOR_NULL;
    }
    else if(0 == keyByteLen)
    {
        return ECDH_INVALID_INPUT;
    }
    else
    {;}

    pByteLen = GET_BYTE_LEN(curve->eccp_p_bitLen);
    nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
    nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);

    //make sure private key is in [1, n-1]
    reverse_byte_array((u8 *)local_prikey, (u8 *)k, nByteLen);
    ret = uint32_integer_check(k, curve->eccp_n, nWordLen, ECDH_ZERO_ALL, ECDH_INTEGER_TOO_BIG,
            ECDH_SUCCESS);
    if(ECDH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //check public key
    reverse_byte_array(peer_pubkey, (u8 *)Px, pByteLen);
    reverse_byte_array(peer_pubkey+pByteLen, (u8 *)Py, pByteLen);
    ret = eccp_pointVerify(curve, Px, Py);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    ret = eccp_pointMul(curve, k, Px, Py, Px, NULL);
    if(PKE_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

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

    return ECDH_SUCCESS;
}

#endif

