/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _SPINOR_DISKIO_H_
#define _SPINOR_DISKIO_H_

#include <stdint.h>
#include <ff.h>
#include "diskio.h"
#include "mtd.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct spinor_blk_device {
    struct mtd_dev *mtd_device;
};

/*!
 * @name SPINOR Disk Function
 * @{
 */

/*!
 * @brief Initializes SPINOR disk.
 *
 * @param device_name the name of device which includes a file system.
 * @retval STA_NOINIT Failed.
 * @retval RES_OK Success.
 */
DSTATUS spinor_disk_initialize(const char *device_name);

/*!
 * Gets SPINOR disk status
 *
 * @param device_name the name of device which includes a file system.
 * @retval STA_NOINIT Failed.
 * @retval RES_OK Success.
 */
DSTATUS spinor_disk_status(const char *device_name);

/*!
 * @brief Reads SPINOR disk.
 *
 * @param device_name the name of device which includes a file system.
 * @param buf The data buffer pointer to store read content.
 * @param sector The start sector number to be read.
 * @param cnt The sector count to be read.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT spinor_disk_read(const char *device_name, uint8_t *buf, uint32_t sector, uint8_t cnt);

/*!
 * @brief Writes SPINOR disk.
 *
 * @param device_name the name of device which includes a file system.
 * @param buf The data buffer pointer to store write content.
 * @param sector The start sector number to be written.
 * @param cnt The sector count to be written.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT spinor_disk_write(const char *device_name, const uint8_t *buf, uint32_t sector, uint8_t cnt);

/*!
 * @brief SPINOR disk IO operation.
 *
 * @param device_name the name of device which includes a file system.
 * @param command The command to be set.
 * @param buf The buffer to store command result.
 * @retval RES_PARERR Failed.
 * @retval RES_OK Success.
 */
DRESULT spinor_disk_ioctl(const char *device_name, uint8_t command, void *buf);

/* @} */
#if defined(__cplusplus)
}
#endif

#endif /* _SPINOR_DISKIO_H_ */

