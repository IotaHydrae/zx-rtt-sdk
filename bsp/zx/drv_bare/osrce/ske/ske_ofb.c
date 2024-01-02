#include <ske_ofb.h>

#ifdef AIC_SKE_MODE_OFB_SUPPORT

u32 ske_sec_ofb_init(SKE_CTX *ctx, SKE_ALG alg, SKE_CRYPTO crypto, u8 *key, u16 sp_key_idx, 
        u8 *iv, SKE_PADDING padding)
{
    return ske_sec_init(ctx, alg, SKE_MODE_OFB, crypto, key, sp_key_idx, iv, padding);
}

u32 ske_sec_ofb_update_blocks(SKE_CTX *ctx, u8 *in, u8 *out, u32 bytes)
{
    return ske_sec_update_blocks(ctx, in, out, bytes);
}

u32 ske_sec_ofb_update_including_last_block(SKE_CTX *ctx, u8 *in, u8 *out, u32 in_bytes, 
        u32 *out_bytes)
{
    return ske_sec_update_including_last_block(ctx, in, out, in_bytes, out_bytes);
}

u32 ske_sec_ofb_final(SKE_CTX *ctx)
{
    return ske_sec_final(ctx);
}

u32 ske_sec_ofb_crypto(SKE_ALG alg, SKE_CRYPTO crypto, u8 *key, u16 sp_key_idx, u8 *iv,
        SKE_PADDING padding, u8 *in, u8 *out, u32 in_bytes, u32 *out_bytes)
{
    return ske_sec_crypto(alg, SKE_MODE_OFB, crypto, key, sp_key_idx, iv, padding, in, out, in_bytes, out_bytes);
}

#endif
