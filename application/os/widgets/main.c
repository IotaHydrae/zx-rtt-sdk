#include <rtthread.h>
#include "zx_gui.h"
#include "zx_ui_entry.h"

static void __zx_gui_event_cb(rt_uint32_t event)
{
}

static void __zx_gui_mq_cb(const void *buffer, rt_size_t size)
{
}

extern void zx_button_adc_start(void);

int zx_gui_app_init(void)
{
    zx_gui_set_event_cb(__zx_gui_event_cb);
    zx_gui_set_mq_cb(__zx_gui_mq_cb);
    zx_gui_init();

    return 0;
}

/*
 * Disabled for measurement builds.
 *
 * This is what actually brings the widgets demo up (zx_gui_init() in the
 * benchmark application looked like the culprit but never ran -- it is
 * commented out there now and the thread still appeared).  The widgets
 * thread stays runnable and competes with whatever is being benchmarked,
 * so a measurement build wants a plain shell instead.
 */
/* INIT_APP_EXPORT(zx_gui_app_init); */

int main(void)
{
    return 0;
}
