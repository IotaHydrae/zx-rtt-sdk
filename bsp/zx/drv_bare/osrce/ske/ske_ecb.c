#include <stdio.h>
#include <ske_ecb.h>

#ifdef AIC_SKE_MODE_ECB_SUPPORT

u32 ske_sec_ecb_init(SKE_CTX *ctx, SKE_ALG alg, SKE_CRYPTO crypto, u8 *key, u16 sp_key_idx, 
        SKE_PADDING padding)
{
    return ske_sec_init(ctx, alg, SKE_MODE_ECB, crypto, key, sp_key_idx, NULL, padding);
}

u32 ske_sec_ecb_update_blocks(SKE_CTX *ctx, u8 *in, u8 *out, u32 bytes)
{
    return ske_sec_update_blocks(ctx, in, out, bytes);
}

u32 ske_sec_ecb_update_including_last_block(SKE_CTX *ctx, u8 *in, u8 *out, u32 in_bytes, 
        u32 *out_bytes)
{
    return ske_sec_update_including_last_block(ctx, in, out, in_bytes, out_bytes);
}

u32 ske_sec_ecb_final(SKE_CTX *ctx)
{
    return ske_sec_final(ctx);
}

u32 ske_sec_ecb_crypto(SKE_ALG alg, SKE_CRYPTO crypto, u8 *key, u16 sp_key_idx,
        SKE_PADDING padding, u8 *in, u8 *out, u32 in_bytes, u32 *out_bytes)
{
    return ske_sec_crypto(alg, SKE_MODE_ECB, crypto, key, sp_key_idx, NULL, padding, in, out, in_bytes, out_bytes);
}

#endif

