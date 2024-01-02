/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inc/spinand.h"
#include "inc/manufacturer.h"

#define SPINAND_MFR_ZETTA		0xBA

const struct aic_spinand_info zetta_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock planes_per_lun
    is_die_select*/
    /*ZD35Q1GC-IB*/
    { DEVID(0x71), PAGESIZE(2048), OOBSIZE(64), BPL(1024), PPB(64), PLANENUM(1),
      DIE(0), "zetta 128MB: 2048+64@64@1024", cmd_cfg_table },
};

const struct aic_spinand_info *zetta_spinand_detect(struct aic_spinand *flash)
{
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_ZETTA)
        return NULL;

    return spinand_match_and_init(Id[1], zetta_spinand_table,
                                  ARRAY_SIZE(zetta_spinand_table));
};

static int zetta_spinand_init(struct aic_spinand *flash)
{
    return 0;
};

static const struct spinand_manufacturer_ops zetta_spinand_manuf_ops = {
    .detect = zetta_spinand_detect,
    .init = zetta_spinand_init,
};

const struct spinand_manufacturer zetta_spinand_manufacturer = {
    .id = SPINAND_MFR_ZETTA,
    .name = "zetta",
    .ops = &zetta_spinand_manuf_ops,
};