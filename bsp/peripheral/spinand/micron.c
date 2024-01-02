/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inc/spinand.h"
#include "inc/manufacturer.h"

#define SPINAND_MFR_MICRON 0x2c

const struct aic_spinand_info micron_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock planes_per_lun
    is_die_select*/
    /*MT29F1G01ABAFD*/
    { DEVID(0x14), PAGESIZE(2048), OOBSIZE(128), BPL(1024), PPB(64),
      PLANENUM(1), DIE(0), "micron 128MB: 2048+128@64@1024", cmd_cfg_table },
    /*MT29F2G01ABAGD*/
    /*ZD35Q2GC-IB*/
    /*XT26G02E*/
    { DEVID(0x24), PAGESIZE(2048), OOBSIZE(128), BPL(2048), PPB(64),
      PLANENUM(2), DIE(0), "micron 256MB: 2048+128@64@2048", cmd_cfg_table },
};

const struct aic_spinand_info *micron_spinand_detect(struct aic_spinand *flash)
{
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_MICRON)
        return NULL;

    return spinand_match_and_init(Id[1], micron_spinand_table,
                                  ARRAY_SIZE(micron_spinand_table));
};

static int micron_spinand_init(struct aic_spinand *flash)
{
    return 0;
};

static const struct spinand_manufacturer_ops micron_spinand_manuf_ops = {
    .detect = micron_spinand_detect,
    .init = micron_spinand_init,
};

const struct spinand_manufacturer micron_spinand_manufacturer = {
    .id = SPINAND_MFR_MICRON,
    .name = "micron",
    .ops = &micron_spinand_manuf_ops,
};