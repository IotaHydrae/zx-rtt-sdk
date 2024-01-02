/*
 * SPDX-License-Identifier: Apache-2.0
 */
#include "spinand.h"
#include "manufacturer.h"

#define SPINAND_MFR_GIGADEVICE 0xC8

#define GD5FXGQXXEXXG_REG_STATUS2 0xf0

#define GIGADEVICE _CFG_QUAD_ENABLE BIT(0)

const struct aic_spinand_info gigadevice_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock planes_per_lun
    is_die_select*/
    /*GD5F2GM7UE*/
    { DEVID(0x92), PAGESIZE(2048), OOBSIZE(128), BPL(2048), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 256MB: 2048+128@64@2048",
      cmd_cfg_table },
    /*GD5F4GM8UE*/
    { DEVID(0x95), PAGESIZE(2048), OOBSIZE(128), BPL(4096), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 512MB: 2048+128@64@4096",
      cmd_cfg_table },
    /*GD5F1GQ5UE*/
    { DEVID(0x51), PAGESIZE(2048), OOBSIZE(128), BPL(1024), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 128MB: 2048+128@64@1024",
      cmd_cfg_table },
    /*GD5F1GM7UE*/
    { DEVID(0x91), PAGESIZE(2048), OOBSIZE(128), BPL(1024), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 128MB: 2048+128@64@1024",
      cmd_cfg_table },
    /*GD5F2GQ5UE*/
    { DEVID(0x52), PAGESIZE(2048), OOBSIZE(128), BPL(2048), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 256MB: 2048+128@64@2048",
      cmd_cfg_table },
    /*GD5F1GQ5REYIGR*/
    { DEVID(0x41), PAGESIZE(2048), OOBSIZE(128), BPL(1024), PPB(64),
      PLANENUM(1), DIE(0), "GIGADEVICE 128MB: 2048+128@64@1024",
      cmd_cfg_table },
};

const struct aic_spinand_info *
gigadevice_spinand_detect(struct aic_spinand *flash)
{
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_GIGADEVICE)
        return NULL;

    return spinand_match_and_init(Id[1], gigadevice_spinand_table,
                                  ARRAY_SIZE(gigadevice_spinand_table));
};

static int gigadevice_spinand_init(struct aic_spinand *flash)
{
    return 0;
};

static const struct spinand_manufacturer_ops gigadevice_spinand_manuf_ops = {
    .detect = gigadevice_spinand_detect,
    .init = gigadevice_spinand_init,
};

const struct spinand_manufacturer gigadevice_spinand_manufacturer = {
    .id = SPINAND_MFR_GIGADEVICE,
    .name = "GIGADEVICE	",
    .ops = &gigadevice_spinand_manuf_ops,
};