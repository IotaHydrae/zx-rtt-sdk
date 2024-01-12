/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "panel_com.h"
#include "panel_dsi.h"

#define PANEL_DEV_NAME      "dsi_panel_ek79007ad"

static int panel_enable(struct aic_panel *panel)
{
    int ret;

    panel_di_enable(panel, 0);
    panel_dsi_send_perpare(panel);

    panel_dsi_setup_realmode(panel);

    panel_de_timing_enable(panel, 0);
    aic_mdelay(100);
    panel_backlight_enable(panel, 0);

    return 0;
}

static struct aic_panel_funcs panel_funcs = {
    .disable = panel_default_disable,
    .unprepare = panel_default_unprepare,
    .prepare = panel_default_prepare,
    .enable = panel_enable,
    .register_callback = panel_register_callback,
};

/* Init the videomode parameter, dts will override the initial value. */
static struct display_timing ek79007ad_timing = {
    .pixelclock = 63000000, // 63M
    .hactive = 1024,
    .hfront_porch = 216,
    .hback_porch = 160,
    .hsync_len = 140,
    .vactive = 600,
    .vfront_porch = 127,
    .vback_porch = 23,
    .vsync_len = 20,
};

struct panel_dsi dsi = {
    .mode = DSI_MOD_VID_PULSE,
    .format = DSI_FMT_RGB888,
    .lane_num = 4,
};

struct aic_panel dsi_ek79007ad = {
    .name = "panel-ek79007ad",
    .timings = &ek79007ad_timing,
    .funcs = &panel_funcs,
    .dsi = &dsi,
    .connector_type = AIC_MIPI_COM,
};
