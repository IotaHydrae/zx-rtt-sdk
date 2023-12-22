/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <dfs_bare.h>
#include <rtconfig.h>
#include "aic_osal.h"
#include "spinor_disk.h"
#include "mtd.h"

static struct spinor_blk_device *blk_device = NULL;
static struct rt_device_blk_geometry info = { 0 };

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT spinor_disk_write(const char *device_name, const uint8_t *buf,
                           uint32_t sector, uint8_t cnt)
{
    rt_size_t phy_pos;
    rt_size_t phy_size;
    rt_size_t ret = 0;
    struct mtd_dev *mtd = blk_device->mtd_device;

    if (!blk_device->mtd_device)
        return RES_NOTRDY;

    return RES_OK;

    /* change the block device's logic address to physical address */
    phy_pos = sector * info.bytes_per_sector;
    phy_size = cnt * info.bytes_per_sector;

    mtd_erase(mtd, phy_pos, phy_size);

    ret = mtd_write(mtd, phy_pos, (uint8_t *)buf, phy_size);
    if (ret) {
        pr_err("Mtd write data failed!\n");
        return -RT_ERROR;
    }

    return RES_OK;
}

DRESULT spinor_disk_read(const char *device_name, uint8_t *buf,
                          uint32_t sector, uint8_t cnt)
{
    rt_size_t ret = 0;
    rt_size_t phy_pos;
    rt_size_t phy_size;
    struct mtd_dev *mtd = blk_device->mtd_device;

    if (!blk_device->mtd_device)
        return RES_NOTRDY;

    /* change the block device's logic address to physical address */
    phy_pos = sector * info.bytes_per_sector;
    phy_size = cnt * info.bytes_per_sector;

    ret = mtd_read(mtd, phy_pos, buf, phy_size);
    if (ret) {
        pr_err("Mtd read data failed!\n");
        return -RT_ERROR;
    }

    return RES_OK;
}

DRESULT spinor_disk_ioctl(const char *device_name, uint8_t command, void *buf)
{
    DRESULT result = RES_OK;

    switch (command) {
        case GET_SECTOR_COUNT:
            if (buf) {
                *(uint32_t *)buf = info.sector_count;
            } else {
                result = RES_PARERR;
            }

            break;

        case GET_SECTOR_SIZE:
            if (buf) {
                *(uint32_t *)buf = info.bytes_per_sector;
            } else {
                result = RES_PARERR;
            }

            break;

        case GET_BLOCK_SIZE:
            if (buf) {
                *(uint32_t *)buf = info.block_size;
            } else {
                result = RES_PARERR;
            }

            break;

        case CTRL_SYNC:
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS spinor_disk_status(const char *device_name)
{
    return RES_OK;
}

DSTATUS spinor_disk_initialize(const char *device_name)
{
    blk_device = (struct spinor_blk_device *)aicos_malloc(
        MEM_CMA, sizeof(struct spinor_blk_device));
    if (!blk_device) {
        pr_err("Error: no memory for create SPI NOR block device");
        return RES_ERROR;
    }

    /*Obtain devices by part name*/
    blk_device->mtd_device = mtd_get_device(device_name);
    if (!blk_device->mtd_device) {
        pr_err("Failed to get mtd %s\n", device_name);
        return RES_NOTRDY;
    }

    info.bytes_per_sector = 512;
    info.block_size = info.bytes_per_sector;
    info.sector_count = blk_device->mtd_device->size / info.bytes_per_sector;

    return RES_OK;
}
