#include <sha224.h>

#ifdef AIC_HASH_SHA224_SUPPORT

/* function: init sha224
 * parameters:
 *     ctx ------------------------ input, SHA224_CTX context pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1.
 */
u32 sha224_init(SHA224_CTX *ctx)
{
    return hash_init(ctx, HASH_SHA224);
}

/* function: sha224 update message
 * parameters:
 *     ctx ------------------------ input, SHA224_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
u32 sha224_update(SHA224_CTX *ctx, const u8 *msg, u32 msg_bytes)
{
    return hash_update(ctx, msg, msg_bytes);
}

/* function: message update done, get the sha224 digest
 * parameters:
 *     digest --------------------- output, sha224 digest, 28 bytes
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the digest buffer is sufficient
 */
u32 sha224_final(SHA224_CTX *ctx, u8 *digest)
{
    return hash_final(ctx, digest);
}

/* function: input whole message and get its sha224 digest
 * parameters:
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message, it could be 0
 *     digest --------------------- output, sha224 digest, 28 bytes
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the digest buffer is sufficient
 */
u32 sha224(u8 *msg, u32 msg_bytes, u8 *digest)
{
    return hash(HASH_SHA224, msg, msg_bytes, digest);
}


#ifdef AIC_HASH_NODE
/* function: input whole message and get its sha224 digest(node style)
 * parameters:
 *     node ----------------------- input, message node pointer
 *     node_num ------------------- input, number of hash nodes, i.e. number of message segments.
 *     digest --------------------- output, sha224 digest, 28 bytes
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the digest buffer is sufficient
 *     2. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 */
u32 sha224_node_steps(HASH_NODE *node, u32 node_num, u8 *digest)
{
    return hash_node_steps(HASH_SHA224, node, node_num, digest);
}
#endif


#ifdef AIC_HASH_DMA
/* function: init dma sha224
 * parameters:
 *     ctx ------------------------ input, SHA224_DMA_CTX context pointer
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 */
u32 sha224_dma_init(SHA224_DMA_CTX *ctx, HASH_CALLBACK callback)
{
    return hash_dma_init(ctx, HASH_SHA224, callback);
}


/* function: dma sha224 update some message blocks
 * parameters:
 *     ctx ------------------------ input, SHA224_DMA_CTX context pointer
 *     msg ------------------------ input, message blocks
 *     msg_bytes ------------------ input, byte length of the input message, must be a multiple of sha224
 *                                  block byte length(64)
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the four parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 sha224_dma_update_blocks(SHA224_DMA_CTX *ctx, u32 msg_h, u32 msg_l, u32 msg_bytes)
{
    return hash_dma_update_blocks(ctx, msg_h, msg_l, msg_bytes);
}
#else
u32 sha224_dma_update_blocks(SHA224_DMA_CTX *ctx, u32 *msg, u32 msg_bytes)
{
    return hash_dma_update_blocks(ctx, msg, msg_bytes);
}
#endif


/* function: dma sha224 final(input the remainder message and get the digest)
 * parameters:
 *     ctx ------------------------ input, SHA224_DMA_CTX context pointer
 *     remainder_msg -------------- input, remainder message
 *     remainder_bytes ------------ input, byte length of the remainder message
 *     digest --------------------- output, sha224 digest, 28 bytes
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the four parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 sha224_dma_final(SHA224_DMA_CTX *ctx, u32 remainder_msg_h, u32 remainder_msg_l, 
        u32 remainder_bytes, u32 digest_h, u32 digest_l)
{
    return hash_dma_final(ctx, remainder_msg_h, remainder_msg_l, remainder_bytes, digest_h, digest_l);
}
#else
u32 sha224_dma_final(SHA224_DMA_CTX *ctx, u32 *remainder_msg, u32 remainder_bytes, u32 *digest)
{
    return hash_dma_final(ctx, remainder_msg, remainder_bytes, digest);
}
#endif


/* function: dma sha224 digest calculate
 * parameters:
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the message, it could be 0
 *     digest --------------------- output, sha224 digest, 28 bytes
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the four parameters are valid
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 sha224_dma(u32 msg_h, u32 msg_l, u32 msg_bytes, u32 digest_h, u32 digest_l, 
        HASH_CALLBACK callback)
{
    return hash_dma(HASH_SHA224, msg_h, msg_l, msg_bytes, digest_h, digest_l, callback);
}
#else
u32 sha224_dma(u32 *msg, u32 msg_bytes, u32 *digest, HASH_CALLBACK callback)
{
    return hash_dma(HASH_SHA224, msg, msg_bytes, digest, callback);
}
#endif

#ifdef AIC_HASH_DMA_NODE
/* function: input whole message and get its sha224 digest(dma node style)
 * parameters:
 *     node ----------------------- input, message node pointer
 *     node_num ------------------- input, number of hash nodes, i.e. number of message segments.
 *     digest --------------------- output, sha224 digest, 28 bytes
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the digest buffer is sufficient
 *     2. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 *     3. for every node or segment except the last, its message length must be a multiple of block length.
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 sha224_dma_node_steps(HASH_DMA_NODE *node, u32 node_num, u32 digest_h, 
        u32 digest_l, HASH_CALLBACK callback)
{
    return hash_dma_node_steps(HASH_SHA224, node, node_num, digest_h, digest_l, callback);
}
#else
u32 sha224_dma_node_steps(HASH_DMA_NODE *node, u32 node_num, u32 *digest, 
        HASH_CALLBACK callback)
{
    return hash_dma_node_steps(HASH_SHA224, node, node_num, digest, callback);
}
#endif
#endif

#endif

#endif

