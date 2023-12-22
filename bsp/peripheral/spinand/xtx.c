/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "inc/spinand.h"
#include "inc/manufacturer.h"

#define SPINAND_MFR_XTX		0x0B
//#define SPINAND_MFR_XTX		0x2C
#define XTX_CFG_QUAD_ENABLE BIT(0)

#define XT26G01C_STATUS_ECC_MASK            (0xF << 4)
#define XT26G01C_STATUS_ECC_NO_BITFLIPS     (0 << 4)
#define XT26G01C_STATUS_ECC_UNCOR_ERROR     (0xF << 4)

struct spi_nand_cmd_cfg xtx_cmd_cfg_table[] = {
    /*opcode    opcode_bits addr_bytes	addr_bits	dummy_bytes	data_nbits*/
    { SPINAND_CMD_READ_FROM_CACHE, 1, 2, 1, 1, 1 },
    { SPINAND_CMD_READ_FROM_CACHE_X2, 1, 2, 1, 1, 2 },
    { SPINAND_CMD_READ_FROM_CACHE_X4, 1, 2, 1, 1, 4 },
    { SPINAND_CMD_PROG_LOAD, 1, 2, 1, 0, 1 },
    { SPINAND_CMD_PROG_LOAD_X4, 1, 2, 1, 0, 4 },
    { SPINAND_CMD_END },
};

int xt26g01c_ecc_get_status(struct aic_spinand *flash, u8 status)
{
    switch (status & XT26G01C_STATUS_ECC_MASK) {
        case XT26G01C_STATUS_ECC_NO_BITFLIPS:
            return 0;
        case XT26G01C_STATUS_ECC_UNCOR_ERROR:
            return -SPINAND_ERR_ECC;
        default:
            break;
    }

    return status & XT26G01C_STATUS_ECC_MASK;
}

const struct aic_spinand_info xtx_spinand_table[] = {
    /*devid page_size oob_size block_per_lun pages_per_eraseblock is_die_select*/
    /*XT26G11C device*/
    { 0x15, 2048, 128, 1024, 64, 0, "XTX 128MB: 2048+128@64@1024",
      xtx_cmd_cfg_table },
    /*XT26G02C device*/
    { 0x12, 2048, 128, 2048, 64, 0, "XTX 256MB: 2048+128@64@2048",
      xtx_cmd_cfg_table },
    /*XT26G01C device*/
    { 0x11, 2048, 128, 1024, 64, 0, "XTX 128MB: 2048+128@64@1024",
      xtx_cmd_cfg_table, xt26g01c_ecc_get_status},
    /*XT26G01B device*/
    { 0xF1, 2048, 64, 1024, 64, 0, "XTX 128MB: 2048+64@64@1024",
      xtx_cmd_cfg_table },
    /*XT26G02E device
    { 0x24, 2048, 128, 2048, 64, 1, "XTX 256MB: 2048+128@64@2048",
      xtx_cmd_cfg_table },*/
};

const struct aic_spinand_info *xtx_spinand_detect(struct aic_spinand *flash)
{
    /*XT26G11C XT26G02C device id 1 Bity*/
    u8 *Id = flash->id.data;

    if (Id[0] != SPINAND_MFR_XTX)
        return NULL;

    return spinand_match_and_init(Id[1], xtx_spinand_table,
                                  ARRAY_SIZE(xtx_spinand_table));
};

static int xtx_spinand_init(struct aic_spinand *flash)
{
    /* Enable quad mode */
    return spinand_config_set(flash, XTX_CFG_QUAD_ENABLE, XTX_CFG_QUAD_ENABLE);
};

static const struct spinand_manufacturer_ops xtx_spinand_manuf_ops = {
    .detect = xtx_spinand_detect,
    .init = xtx_spinand_init,
};

const struct spinand_manufacturer xtx_spinand_manufacturer = {
    .id = SPINAND_MFR_XTX,
    .name = "XTX",
    .ops = &xtx_spinand_manuf_ops,
};
