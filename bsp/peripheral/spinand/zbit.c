/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inc/spinand.h"
#include "inc/manufacturer.h"

#define SPINAND_MFR_ZBIT		0x5E

const struct aic_spinand_info zbit_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock planes_per_lun
    is_die_select*/
    /*ZB35Q01A*/
    { DEVID(0x41), PAGESIZE(2048), OOBSIZE(64), BPL(1024), PPB(64), PLANENUM(1),
      DIE(0), "zbit 128MB: 2048+64@64@1024", cmd_cfg_table },
};

const struct aic_spinand_info *zbit_spinand_detect(struct aic_spinand *flash)
{
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_ZBIT)
        return NULL;

    return spinand_match_and_init(Id[1], zbit_spinand_table,
                                  ARRAY_SIZE(zbit_spinand_table));
};

static int zbit_spinand_init(struct aic_spinand *flash)
{
    return 0;
};

static const struct spinand_manufacturer_ops zbit_spinand_manuf_ops = {
    .detect = zbit_spinand_detect,
    .init = zbit_spinand_init,
};

const struct spinand_manufacturer zbit_spinand_manufacturer = {
    .id = SPINAND_MFR_ZBIT,
    .name = "zbit",
    .ops = &zbit_spinand_manuf_ops,
};