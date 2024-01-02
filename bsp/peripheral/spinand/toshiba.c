/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inc/spinand.h"
#include "inc/manufacturer.h"

#define SPINAND_MFR_TOSHIBA 0x98

const struct aic_spinand_info toshiba_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock planes_per_lun
    is_die_select*/
    /*TC58CVG1S3HRAIJ*/
    { DEVID(0xEB), PAGESIZE(2048), OOBSIZE(128), BPL(2048), PPB(64),
      PLANENUM(1), DIE(0), "toshiba 256MB: 2048+128@64@2048", cmd_cfg_table },
};

const struct aic_spinand_info *toshiba_spinand_detect(struct aic_spinand *flash)
{
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_TOSHIBA)
        return NULL;

    return spinand_match_and_init(Id[1], toshiba_spinand_table,
                                  ARRAY_SIZE(toshiba_spinand_table));
};

static int toshiba_spinand_init(struct aic_spinand *flash)
{
    return 0;
};

static const struct spinand_manufacturer_ops toshiba_spinand_manuf_ops = {
    .detect = toshiba_spinand_detect,
    .init = toshiba_spinand_init,
};

const struct spinand_manufacturer toshiba_spinand_manufacturer = {
    .id = SPINAND_MFR_TOSHIBA,
    .name = "toshiba",
    .ops = &toshiba_spinand_manuf_ops,
};