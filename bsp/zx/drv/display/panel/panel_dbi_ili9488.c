/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "panel_dbi.h"

/* Init sequence, each line consists of command, count of data, data... */
static const u8 ili9488_commands[] = {
    // Positive Gamma Control
    0xe0, 15, 0x00, 0x03, 0x09, 0x08, 0x16, 0x0a, 0x3f, 0x78, 0x4C, 0x09, 0x0a, 0x08, 0x16, 0x1A, 0x0f,
    // Negative Gamma Control
    0xe1, 15, 0x00, 0x16, 0x19, 0x03, 0x0f, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0e, 0x0D, 0x35, 0x37, 0x0f,

    0xc0, 2, 0x17, 0x15,                // Power Control 1
    0xc1, 1, 0x41,                      // Power Control 2
    0xc5, 3, 0x00, 0x12, 0x80,          // VCOM Control
    0x36, 1, 0x48,                      // Memory Access Control
    0x3a, 1, 0x55,                      // Pixel Interface Format RGB565 8080 16-bit
    0xb0, 1, 0x00,                      // Interface Mode Control

    // Frame Rate Control
    // 0xb1, 2, 0xd0, 0x11,             // 60Hz
    0xb1, 2, 0xd0, 0x14,                // 90Hz

    0xb4, 1, 0x02,                      // Display Inversion Control
    0xb6, 3, 0x02, 0x02, 0x3b,          // Display Function Control
    0xb7, 1, 0xc6,                      // Entry Mode Set
    0xf7, 4, 0xa9, 0x51, 0x2c, 0x82,    // Adjust Control 3
    0x11, 0,                            // Exit Sleep
    0x00, 1, 60,
    0x29, 0,                            // Display on
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

static struct aic_panel_funcs ili9488_funcs = {
    .prepare = panel_prepare,
    .enable = panel_dbi_default_enable,
    .disable = panel_default_disable,
    .unprepare = panel_default_unprepare,
    .register_callback = panel_register_callback,
};

static struct display_timing ili9488_timing = {
    .pixelclock   = 50000000,

    .hactive      = 320,
    .hback_porch  = 2,
    .hfront_porch = 3,
    .hsync_len    = 1,

    .vactive      = 480,
    .vback_porch  = 3,
    .vfront_porch = 2,
    .vsync_len    = 1,
};

static struct panel_dbi dbi = {
    .type = I8080,
    .format = I8080_RGB565_16BIT,
    .commands = {
        .buf = ili9488_commands,
        .len = ARRAY_SIZE(ili9488_commands),
    }
};

struct aic_panel dbi_ili9488 = {
    .name = "panel-ili9488",
    .timings = &ili9488_timing,
    .funcs = &ili9488_funcs,
    .dbi = &dbi,
    .connector_type = AIC_DBI_COM,
};
