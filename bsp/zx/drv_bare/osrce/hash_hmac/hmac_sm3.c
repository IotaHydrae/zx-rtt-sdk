#include <hmac_sm3.h>

#ifdef AIC_HASH_SM3_SUPPORT

/* function: init hmac-sm3
 * parameters:
 *     ctx ------------------------ input, HMAC_SM3_CTX context pointer
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, byte length of key, it could be 0
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1.
 */
u32 hmac_sm3_init(HMAC_SM3_CTX *ctx, const u8 *key, u16 sp_key_idx, u32 key_bytes)
{
    return hmac_init(ctx, HASH_SM3, key, sp_key_idx, key_bytes);
}

/* function: hmac-sm3 update message
 * parameters:
 *     ctx ------------------------ input, HMAC_SM3_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
u32 hmac_sm3_update(HMAC_SM3_CTX *ctx, const u8 *msg, u32 msg_bytes)
{
    return hmac_update(ctx, msg, msg_bytes);
}

/* function: message update done, get the hmac
 * parameters:
 *     ctx ------------------------ input, HMAC_CTX context pointer
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the ctx is valid and initialized
 *     2. please make sure the mac buffer is sufficient
 */
u32 hmac_sm3_final(HMAC_SM3_CTX *ctx, u8 *mac)
{
    return hmac_final(ctx, mac);
}

/* function: input key and whole message, get the hmac
 * parameters:
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, byte length of the key
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the mac buffer is sufficient
 */
u32 hmac_sm3(u8 *key, u16 sp_key_idx, u32 key_bytes, u8 *msg, 
        u32 msg_bytes, u8 *mac)
{
    return hmac(HASH_SM3, key, sp_key_idx, key_bytes, msg, msg_bytes, mac);
}

#ifdef AIC_HASH_NODE
/* function: input key and whole message, get the hmac(node style)
 * parameters:
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, byte length of the key
 *     node ----------------------- input, message node pointer
 *     node_num ------------------- input, number of hash nodes, i.e. number of message segments.
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the mac buffer is sufficient
 *     2. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 */
u32 hmac_sm3_node_steps(u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_NODE *node, u32 node_num, u8 *mac)
{
    return hmac_node_steps(HASH_SM3, key, sp_key_idx, key_bytes, node, node_num, mac);
}
#endif

#ifdef AIC_HASH_DMA
/* function: init dma hmac-sm3
 * parameters:
 *     ctx ------------------------ input, HMAC_SM3_DMA_CTX context pointer
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, key byte length
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 */
u32 hmac_sm3_dma_init(HMAC_SM3_DMA_CTX *ctx, const u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_CALLBACK callback)
{
    return hmac_dma_init(ctx, HASH_SM3, key, sp_key_idx, key_bytes, callback);
}

/* function: dma hmac-sm3 update message
 * parameters:
 *     ctx ------------------------ input, HMAC_SM3_DMA_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message, must be a multiple of block byte length
 *                                  of SM3(64)
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the four parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_sm3_dma_update_blocks(HMAC_SM3_DMA_CTX *ctx, u32 msg_h, u32 msg_l, u32 msg_bytes)
{
    return hmac_dma_update_blocks(ctx, msg_h, msg_l, msg_bytes);
}
#else
u32 hmac_sm3_dma_update_blocks(HMAC_SM3_DMA_CTX *ctx, u32 *msg, u32 msg_bytes)
{
    return hmac_dma_update_blocks(ctx, msg, msg_bytes);
}
#endif

/* function: dma hmac-sm3 message update done, get the hmac
 * parameters:
 *     ctx ------------------------ input, HMAC_SM3_DMA_CTX context pointer
 *     remainder_msg -------------- input, message
 *     remainder_bytes ------------ input, byte length of the remainder message
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_sm3_dma_final(HMAC_SM3_DMA_CTX *ctx, u32 remainder_msg_h, u32 remainder_msg_l, 
        u32 remainder_bytes, u32 mac_h, u32 mac_l)
{
    return hmac_dma_final(ctx, remainder_msg_h, remainder_msg_l, remainder_bytes, mac_h, mac_l);
}
#else
u32 hmac_sm3_dma_final(HMAC_SM3_DMA_CTX *ctx, u32 *remainder_msg, u32 remainder_bytes, 
        u32 *mac)
{
    return hmac_dma_final(ctx, remainder_msg, remainder_bytes, mac);
}
#endif

/* function: dma hmac-sm3 input key and message, get the hmac
 * parameters:
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, key byte length
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 *     mac ------------------------ output, hmac
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_sm3_dma(u8 *key, u16 sp_key_idx, u32 key_bytes, u32 msg_h, u32 msg_l, 
        u32 msg_bytes, u32 mac_h, u32 mac_l, HASH_CALLBACK callback)
{
    return hmac_dma(HASH_SM3, key, sp_key_idx, key_bytes, msg_h, msg_l, msg_bytes, mac_h, mac_l, callback);
}
#else
u32 hmac_sm3_dma(u8 *key, u16 sp_key_idx, u32 key_bytes, u32 *msg, u32 msg_bytes, 
        u32 *mac, HASH_CALLBACK callback)
{
    return hmac_dma(HASH_SM3, key, sp_key_idx, key_bytes, msg, msg_bytes, mac, callback);
}
#endif

#ifdef AIC_HASH_DMA_NODE
/* function: dma hmac input key and message, get the hmac(node style)
 * parameters:
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, key byte length
 *     node ----------------------- input, message node pointer
 *     node_num ------------------- input, number of hash nodes, i.e. number of message segments.
 *     mac ------------------------ output, hmac
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the digest buffer is sufficient
 *     2. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 *     3. for every node or segment except the last, its message length must be a multiple of block length.
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_sm3_dma_node_steps(u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_DMA_NODE *node, u32 node_num, u32 mac_h, u32 mac_l, HASH_CALLBACK callback)
{
    return hmac_dma_node_steps(HASH_SM3, key, sp_key_idx, key_bytes, node, node_num, mac_h, mac_l, 
            callback);
}
#else
u32 hmac_sm3_dma_node_steps(u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_DMA_NODE *node, u32 node_num, u32 *mac, HASH_CALLBACK callback)
{
    return hmac_dma_node_steps(HASH_SM3, key, sp_key_idx, key_bytes, node, node_num, mac, callback);
}
#endif
#endif

#endif

#endif
