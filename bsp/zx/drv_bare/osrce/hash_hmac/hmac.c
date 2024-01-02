//#include <stdio.h>

#include <hash.h>
#include <hmac.h>
#include <utility.h>

/* function: init HMAC
 * parameters:
 *     ctx ------------------------ input, HMAC_CTX context pointer
 *     hash_alg ------------------- input, specific hash algorithm
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, byte length of key, it could be 0
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure hash_alg is valid
 */
u32 hmac_init(HMAC_CTX *ctx, HASH_ALG hash_alg, const u8 *key, u16 sp_key_idx, u32 key_bytes)
{
    u32 block_byte_len, digest_byte_len;
    u32 i, ret;

    if(NULL == ctx)
    {
        return HASH_BUFFER_NULL;
    }
    else if(HASH_SUCCESS != check_hash_alg(hash_alg))
    {
        return HASH_INPUT_INVALID;
    }
    else if(NULL == key)
    {
#ifdef HMAC_SECURE_PORT_FUNCTION
        //TODO
#else
        return HASH_BUFFER_NULL; //key_bytes = 0;
#endif
    }
    else
    {
        //handle other;
    }

#ifdef HMAC_SECURE_PORT_FUNCTION
    if(NULL != key)   //key is from user input
    {
        //hash_hmac_disable_secure_port();
    }
    else      //key is from secure port
    {
        //hash_hmac_enable_secure_port(sp_key_idx);
        //hash_hmac_enable_secure_port(sp_key_idx+1);
    }
#endif

    block_byte_len = CAST2UINT32(hash_get_block_word_len(hash_alg))<<2;
    digest_byte_len = CAST2UINT32(hash_get_digest_word_len(hash_alg))<<2;

    //get K0
    if(key_bytes <= block_byte_len)
    {
        memcpy_((u8 *)(ctx->K0), (u8 *)key, key_bytes);
        memset_(((u8 *)(ctx->K0)) + key_bytes, 0, block_byte_len - key_bytes);
    }
    else
    {
        //K0 = hash(key)||000..00
        ret = hash_init(ctx->hash_ctx, hash_alg);
        if(HASH_SUCCESS != ret)
        {
            goto END;
        }
        else
        {;}

        ret = hash_update(ctx->hash_ctx, (u8 *)key, key_bytes);
        if(HASH_SUCCESS != ret)
        {
            goto END;
        }
        else
        {;}

        ret = hash_final(ctx->hash_ctx, (u8 *)(ctx->K0));
        if(HASH_SUCCESS != ret)
        {
            goto END;
        }
        else
        {;}

        memset_(((u8 *)(ctx->K0)) + digest_byte_len, 0, block_byte_len - digest_byte_len);
    }

    //get K0 ^ ipad
    digest_byte_len = block_byte_len/4U;
    for(i=0; i<digest_byte_len; i++)
    {
        ctx->K0[i] ^= HMAC_IPAD;
    }

    ret = hash_init(ctx->hash_ctx, hash_alg);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {
        ret = hash_update(ctx->hash_ctx, (u8 *)(ctx->K0), block_byte_len);
    }

END:
    if(HASH_SUCCESS != ret)
    {
        memset_((u8 *)ctx, 0, sizeof(HMAC_CTX));
    }
    else
    {;}

    return ret;
}


/* function: hmac update message
 * parameters:
 *     ctx ------------------------ input, HMAC_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
u32 hmac_update(HMAC_CTX *ctx, const u8 *msg, u32 msg_bytes)
{
    if(NULL == ctx)
    {
        return HASH_BUFFER_NULL;
    }
    else
    {
        return hash_update(ctx->hash_ctx, msg, msg_bytes);
    }
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
u32 hmac_final(HMAC_CTX *ctx, u8 *mac)
{
    HASH_ALG hash_alg;
    u32 block_word_len, digest_word_len;
    u32 i, ret;

    if(NULL == ctx || NULL == mac)
    {
        return HASH_BUFFER_NULL;
    }
    else
    {;}

    hash_alg = ctx->hash_ctx->hash_alg;
    digest_word_len = hash_get_digest_word_len(hash_alg);

    //set mac as hash((K0^ipad)||message)
    //caution: here context will be cleaned up
    ret = hash_final(ctx->hash_ctx, mac);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    //get K0 ^ opad
    block_word_len = hash_get_block_word_len(hash_alg);
    for(i=0; i<block_word_len; i++)
    {
        ctx->K0[i] ^= HMAC_IPAD_XOR_OPAD;
    }

    ret = hash_init(ctx->hash_ctx, hash_alg);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx->hash_ctx, (u8 *)(ctx->K0), ctx->hash_ctx->block_byte_len);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx->hash_ctx, mac, ctx->hash_ctx->digest_byte_len);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_final(ctx->hash_ctx, mac);

END:
    if(HASH_SUCCESS != ret)
    {
        memset_(mac, 0, digest_word_len<<2);
    }
    else
    {;}

    memset_((u8 *)ctx, 0, sizeof(HMAC_CTX));

    return ret;
}


/* function: input key and whole message, get the hmac
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm
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
u32 hmac(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, u8 *msg, 
        u32 msg_bytes, u8 *mac)
{
    HMAC_CTX ctx[1];
    u32 ret;

    if(NULL == mac)
    {
        return HASH_BUFFER_NULL;
    }
    else
    {;}

    ret = hmac_init(ctx, hash_alg, key, sp_key_idx, key_bytes);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hash_update(ctx->hash_ctx, (u8 *)msg, msg_bytes);
    if(HASH_SUCCESS != ret)
    {
        goto END;
    }
    else
    {;}

    ret = hmac_final(ctx, mac);

END:
    memset_((u8 *)ctx, 0, sizeof(HMAC_CTX));

    return ret;
}


#ifdef AIC_HASH_NODE
/* function: input key and whole message, get the hmac(node style)
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, byte length of the key
 *     node ----------------------- input, message node pointer
 *     node_num ------------------- input, number of hash nodes, i.e. number of message segments.
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the mac buffer is sufficient
 *     2. here hmac is not for SHA3.
 *     3. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 */
u32 hmac_node_steps(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_NODE *node, u32 node_num, u8 *mac)
{
    HMAC_CTX ctx[1];
    u32 i, ret;

    ret = hmac_init(ctx, hash_alg, key, sp_key_idx, key_bytes);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    for(i=0U; i<node_num; i++)
    {
        ret = hmac_update(ctx, node[i].msg_addr, node[i].msg_bytes);
        if(HASH_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }

    return hmac_final(ctx, mac);
}
#endif


#ifdef AIC_HASH_DMA
/* function: init dma hmac
 * parameters:
 *     ctx ------------------------ input, HMAC_DMA_CTX context pointer
 *     hash_alg ------------------- input, specific hash algorithm
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, key byte length
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure hash_alg is valid
 *     2. here hmac is not for SHA3.
 */
u32 hmac_dma_init(HMAC_DMA_CTX *ctx, HASH_ALG hash_alg, const u8 *key, u16 sp_key_idx, 
        u32 key_bytes, HASH_CALLBACK callback)
{
    u32 ret;
    HMAC_CTX tmp_ctx[1];

    if(NULL == ctx)
    {
        return HASH_BUFFER_NULL;
    }
    else if(HASH_SUCCESS != check_hash_alg(hash_alg))
    {
        return HASH_INPUT_INVALID;
    }
    else if(NULL == key)
    {
        key_bytes = 0;
    }
    else
    {
        //handle other;
    }

    ret = hmac_init(tmp_ctx, hash_alg, (u8 *)key, sp_key_idx, key_bytes);
    if(HASH_SUCCESS == ret)
    {
        ctx->hash_dma_ctx->hash_alg        = hash_alg;
        ctx->hash_dma_ctx->block_word_len  = (tmp_ctx->hash_ctx->block_byte_len)/((u8)4);
        ctx->hash_dma_ctx->digest_byte_len = hash_get_digest_word_len(hash_alg)<<2;
        ctx->hash_dma_ctx->callback        = callback;
        uint32_copy(ctx->hash_dma_ctx->total, tmp_ctx->hash_ctx->total, CAST2UINT32(ctx->hash_dma_ctx->block_word_len)>>3);
        memcpy_((u8 *)(ctx->K0), (u8 *)(tmp_ctx->K0), CAST2UINT32(ctx->hash_dma_ctx->block_word_len)<<2);

#ifdef AIC_HASH_MUL_THREAD
        ctx->hash_dma_ctx->first_update_flag = (u8)0;
        ctx->hash_dma_ctx->iterator_word_len = hash_get_iterator_word_len(hash_alg);
        memcpy_((u8 *)(ctx->hash_dma_ctx->iterator), (u8 *)(tmp_ctx->hash_ctx->iterator), 
                CAST2UINT32(ctx->hash_dma_ctx->iterator_word_len)<<2);
#else
        hash_set_dma_mode();
        hash_set_dma_output_len(0);
#endif
    }
    else
    {;}

    return ret;
}


/* function: dma hmac update message
 * parameters:
 *     ctx ------------------------ input, HMAC_DMA_CTX context pointer
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message, must be a multiple of block byte length of HASH
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the four parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_dma_update_blocks(HMAC_DMA_CTX *ctx, u32 msg_h, u32 msg_l, u32 msg_bytes)
#else
u32 hmac_dma_update_blocks(HMAC_DMA_CTX *ctx, u32 *msg, u32 msg_bytes)
#endif
{
    if(NULL == ctx)
    {
        return HASH_BUFFER_NULL;
    }
    else
    {
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
        return hash_dma_update_blocks(ctx->hash_dma_ctx, msg_h, msg_l, msg_bytes);
#else
        return hash_dma_update_blocks(ctx->hash_dma_ctx, msg, msg_bytes);
#endif
    }
}


/* function: dma hmac message update done, get the hmac
 * parameters:
 *     ctx ------------------------ input, HMAC_DMA_CTX context pointer
 *     remainder_msg -------------- input, message
 *     remainder_bytes ------------ input, byte length of the last message
 *     mac ------------------------ output, hmac
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the three parameters are valid, and ctx is initialized
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_dma_final(HMAC_DMA_CTX *ctx, u32 remainder_msg_h, u32 remainder_msg_l, 
        u32 remainder_bytes, u32 mac_h, u32 mac_l)
#else
u32 hmac_dma_final(HMAC_DMA_CTX *ctx, u32 *remainder_msg, u32 remainder_bytes, u32 *mac)
#endif
{
    u32 i;
    u32 ret;
    HASH_CTX tmp_ctx[1];

#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    if((NULL == ctx) || ((0U == mac_h) && (0U == mac_l)))
#else
    if((NULL == ctx) || (NULL == mac))
#endif
    {
        return HASH_BUFFER_NULL;
    }
    else
    {;}

#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    ret = hash_dma_final(ctx->hash_dma_ctx, remainder_msg_h, remainder_msg_l, remainder_bytes, mac_h, mac_l);
#else
    ret = hash_dma_final(ctx->hash_dma_ctx, remainder_msg, remainder_bytes, mac);//print_buf_U8(mac, 32, "mac---------");
#endif
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //get K0 ^ opad
    for(i=0; i<ctx->hash_dma_ctx->block_word_len; i++)
    {
        ctx->K0[i] ^= HMAC_IPAD_XOR_OPAD;
    }

    ret = hash_init(tmp_ctx, ctx->hash_dma_ctx->hash_alg);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    ret = hash_update(tmp_ctx, (u8 *)(ctx->K0), CAST2UINT32(ctx->hash_dma_ctx->block_word_len)<<2);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    //tmp_iterator may not be accessed by bytes
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    uint32_copy(ctx->K0, (u32 *)mac_l, CAST2UINT32(ctx->hash_dma_ctx->digest_byte_len)>>2);
#else
    uint32_copy(ctx->K0, mac, CAST2UINT32(ctx->hash_dma_ctx->digest_byte_len)>>2);
#endif
    ret = hash_update(tmp_ctx, (u8 *)(ctx->K0), ctx->hash_dma_ctx->digest_byte_len);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
        return hash_final(tmp_ctx, (u8 *)mac_l);
#else
        return hash_final(tmp_ctx, (u8 *)mac);
#endif
    }
}


/* function: dma hmac input key and message, get the hmac
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm
 *     key ------------------------ input, key
 *     sp_key_idx ----------------- input, index of secure port key
 *     key_bytes ------------------ input, key byte length
 *     msg ------------------------ input, message
 *     msg_bytes ------------------ input, byte length of the input message
 *     mac ------------------------ output, hmac
 *     callback ------------------- callback function pointer
 * return: HASH_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure hash_alg is valid
 *     2. here hmac is not for SHA3.
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_dma(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, u32 msg_h, 
        u32 msg_l, u32 msg_bytes, u32 mac_h, u32 mac_l, HASH_CALLBACK callback)
#else
u32 hmac_dma(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, u32 *msg, 
        u32 msg_bytes, u32 *mac, HASH_CALLBACK callback)
#endif
{
    u32 ret;
    HMAC_DMA_CTX ctx[1];

    ret = hmac_dma_init(ctx, hash_alg, key, sp_key_idx, key_bytes, callback);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    return hmac_dma_final(ctx, msg_h, msg_l, msg_bytes, mac_h, mac_l);
#else
    return hmac_dma_final(ctx, msg, msg_bytes, mac);
#endif
}


#ifdef AIC_HASH_DMA_NODE
/* function: dma hmac input key and message, get the hmac(node style)
 * parameters:
 *     hash_alg ------------------- input, specific hash algorithm
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
 *     2. please make sure hash_alg is valid
 *     3. here hmac is not for SHA3.
 *     4. if the whole message consists of some segments, every segment is a node, a node includes
 *        address and byte length.
 *     5. for every node or segment except the last, its message length must be a multiple of block length.
 */
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
u32 hmac_dma_node_steps(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_DMA_NODE *node, u32 node_num, u32 mac_h, u32 mac_l, HASH_CALLBACK callback)
#else
u32 hmac_dma_node_steps(HASH_ALG hash_alg, u8 *key, u16 sp_key_idx, u32 key_bytes, 
        HASH_DMA_NODE *node, u32 node_num, u32 *mac, HASH_CALLBACK callback)
#endif
{
    u32 i, ret;
    HMAC_DMA_CTX ctx[1];

    ret = hmac_dma_init(ctx, hash_alg, key, sp_key_idx, key_bytes, callback);
    if(HASH_SUCCESS != ret)
    {
        return ret;
    }
    else
    {;}

    for(i=0; i<node_num-1U; i++)
    {
#ifdef AIC_HASH_ADDRESS_HIGH_LOW
        ret = hmac_dma_update_blocks(ctx, node[i].msg_addr_h, node[i].msg_addr_l, node[i].msg_bytes);
#else
        ret = hmac_dma_update_blocks(ctx, node[i].msg_addr, node[i].msg_bytes);
#endif
        if(HASH_SUCCESS != ret)
        {
            return ret;
        }
        else
        {;}
    }

#ifdef AIC_HASH_ADDRESS_HIGH_LOW
    return hmac_dma_final(ctx, node[i].msg_addr_h, node[i].msg_addr_l, node[i].msg_bytes, mac_h, mac_l);
#else
    return hmac_dma_final(ctx, node[i].msg_addr, node[i].msg_bytes, mac);
#endif
}
#endif

#endif

