/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "panel_dbi.h"

/* Init sequence, each line consists of command, count of data, data... */
static const u8 lg4572b_commands[] = {
    0x20, 0,
    0x3A, 1, 0x55,
    0xB2, 2, 0x20, 0xC8,
    0xB3, 1, 0x00,
    0xB4, 1, 0x04,
    0xB5, 1, 0x10,
    0xB6, 6, 0x01, 0x18, 0x02, 0x40, 0x10, 0x00,
    0xB7, 5, 0x46, 0x06, 0x0C, 0x00, 0x00,

    0xC0, 2, 0x01, 0x11,
    0xC3, 5, 0x07, 0x03, 0x04, 0x05, 0x04,
    0xC4, 6, 0x32, 0x24, 0x10, 0x10, 0x01, 0x0A,
    0xC5, 1, 0x6A,
    0xC6, 2, 0x24, 0x50,

    0xD0, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0xD2, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0xD4, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0xD1, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0xD3, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0xD5, 9, 0x02, 0x76, 0x54, 0x15, 0x12, 0x03, 0x42, 0x43, 0x03,
    0x11, 0,
    0x00, 1, 120,
    0x29, 0,
    0x2C, 0,
};

#define RESET_PIN "PF.14"

static struct gpio_desc reset;

static int panel_prepare(void)
{
    panel_get_gpio(&reset, RESET_PIN);

    panel_gpio_set_value(&reset, 1);
    aic_delay_ms(5);
    panel_gpio_set_value(&reset, 0);
    aic_delay_ms(5);
    panel_gpio_set_value(&reset, 1);
    aic_delay_ms(5);

    return 0;
}

static struct aic_panel_funcs lg4572b_funcs = {
    .prepare = panel_prepare,
    .enable = panel_dbi_default_enable,
    .disable = panel_default_disable,
    .unprepare = panel_default_unprepare,
    .register_callback = panel_register_callback,
};

static struct display_timing lg4572b_timing = {
    .pixelclock   = 50000000,

    .hactive      = 480,
    .hback_porch  = 2,
    .hfront_porch = 3,
    .hsync_len    = 1,

    .vactive      = 800,
    .vback_porch  = 3,
    .vfront_porch = 2,
    .vsync_len    = 1,
};

static struct panel_dbi dbi = {
    .type = I8080,
    .format = I8080_RGB565_16BIT,
    .commands = {
        .buf = lg4572b_commands,
        .len = ARRAY_SIZE(lg4572b_commands),
    }
};

struct aic_panel dbi_lg4572b = {
    .name = "panel-lg4572b",
    .timings = &lg4572b_timing,
    .funcs = &lg4572b_funcs,
    .dbi = &dbi,
    .connector_type = AIC_DBI_COM,
};
