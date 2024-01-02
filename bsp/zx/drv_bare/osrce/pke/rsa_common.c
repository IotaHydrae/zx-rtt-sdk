#if (defined(AIC_PKE_RSASSA_PSS_SUPPORT) || defined(SUPPORT_RSAES_OAEP))

#include <rsa.h>
#include <hash_kdf.h>
#include <utility.h>
//#include "../../crypto_include/trng.h"

/* function: RSA PKCS#1_v2.2 MGF1(a mask generation function based on a hash function)
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm for MGF1
 *     seed ----------------------- input, seed
 *     seed_bytes ----------------- input, byte length of seed
 *     in ------------------------- input, this is to XOR mask, and this could be NULL
 *     out ------------------------ output, if in is NULL, this is mask directly, otherwise,
 *                                  this is (mask XOR in).
 *     mask_bytes ----------------- input, if in is NULL, this is byte length of out(mask), otherwise,
 *                                  this is byte length of in or out(mask XOR in).
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. out = mask XOR in, if in is NULL, out is mask directly.
 */
u32 rsa_pkcs1_mgf1_with_xor_in(HASH_ALG hash_alg, u8 *seed, u32 seed_bytes, u8 *in,
        u8 *out, u32 mask_bytes)
{
    u8 counter[4] = {0,0,0,0};
    u32 ret;
    HASH_NODE hash_node[2] = 
    {
        {seed, seed_bytes},
        {counter, 4},
    };

    ret = ansi_x9_63_kdf_node_with_xor_in(hash_alg, hash_node, 2, 1, in, out, mask_bytes, 1);
    if(HASH_SUCCESS == ret)
    {
        return RSA_SUCCESS;
    }
    else
    {
        return ret;
    }
}

#endif

