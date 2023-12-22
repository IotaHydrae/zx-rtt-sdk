/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *  Desc: virtual memory allocator
 */

#ifndef MPP_MEM_H
#define MPP_MEM_H

#include <limits.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void *mpp_alloc(size_t size);
void mpp_free(void *ptr);

unsigned int mpp_phy_alloc(size_t size);
void mpp_phy_free(unsigned int addr);

#ifdef __cplusplus
}
#endif

#endif /* MEM_H */
