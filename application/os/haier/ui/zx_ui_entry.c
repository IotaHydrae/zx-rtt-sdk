
#include <stdio.h>
#include <stdlib.h>
#include "zx_ui_entry.h"
#include "aic_dec.h"

FAKE_IMAGE_DECLARE(bg_dark);
static lv_obj_t *img_bg = NULL;

static void cait_task(struct _lv_timer_t *t)
{
    static int i = 0;
    i ? lv_obj_add_state(t->user_data, LV_STATE_CHECKED) : lv_obj_clear_state(t->user_data, LV_STATE_CHECKED);
    i = !i;
}

void zx_ui_entry(void)
{
    FAKE_IMAGE_INIT(bg_dark, 600, 1280, 0, 0x00000000);

    img_bg = lv_img_create(lv_scr_act());
    lv_img_set_src(img_bg, FAKE_IMAGE_NAME(bg_dark));
    lv_obj_set_pos(img_bg, 0, 0);


	lv_obj_t *swi1 = lv_switch_create(lv_scr_act());
    lv_obj_set_size(swi1, 81, 36);
    lv_obj_clear_state(swi1, LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(swi1, NULL, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi1, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(swi1, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi1, lv_color_black(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(swi1, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(swi1, 2, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(swi1, -5, LV_PART_KNOB);
    lv_obj_set_style_radius(swi1, 0, LV_PART_KNOB);
    lv_obj_set_style_radius(swi1, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(swi1, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(swi1, lv_color_make(107, 107, 107), LV_PART_MAIN);
    lv_obj_set_style_border_color(swi1, lv_color_make(234, 189, 133), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(swi1, lv_color_make(107, 107, 107), LV_PART_KNOB);
    lv_obj_set_style_bg_color(swi1, lv_color_make(234, 189, 133), LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_pos(swi1, 430 - 2, 765 + 2);

	lv_obj_t *swi2 = lv_switch_create(lv_scr_act());
    lv_obj_set_size(swi2, 81, 36);
    lv_obj_clear_state(swi2, LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(swi2, NULL, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi2, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(swi2, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi2, lv_color_black(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(swi2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(swi2, 2, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(swi2, -5, LV_PART_KNOB);
    lv_obj_set_style_radius(swi2, 0, LV_PART_KNOB);
    lv_obj_set_style_radius(swi2, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(swi2, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(swi2, lv_color_make(107, 107, 107), LV_PART_MAIN);
    lv_obj_set_style_border_color(swi2, lv_color_make(234, 189, 133), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(swi2, lv_color_make(107, 107, 107), LV_PART_KNOB);
    lv_obj_set_style_bg_color(swi2, lv_color_make(234, 189, 133), LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_pos(swi2, 430 - 2, 880);

	lv_obj_t *swi3 = lv_switch_create(lv_scr_act());
    lv_obj_set_size(swi3, 81, 36);
    lv_obj_clear_state(swi3, LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(swi3, NULL, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi3, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(swi3, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(swi3, lv_color_black(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(swi3, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(swi3, 2, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(swi3, -5, LV_PART_KNOB);
    lv_obj_set_style_radius(swi3, 0, LV_PART_KNOB);
    lv_obj_set_style_radius(swi3, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(swi3, 0, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(swi3, lv_color_make(107, 107, 107), LV_PART_MAIN);
    lv_obj_set_style_border_color(swi3, lv_color_make(234, 189, 133), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(swi3, lv_color_make(107, 107, 107), LV_PART_KNOB);
    lv_obj_set_style_bg_color(swi3, lv_color_make(234, 189, 133), LV_PART_KNOB | LV_STATE_CHECKED);
	lv_obj_set_pos(swi3, 430 - 1, 994);
	// lv_timer_t *t1 = lv_timer_create(cait_task, 1000, swi3);

	rt_kprintf("test\n");
	rt_kprintf("test\n");
	rt_kprintf("test\n");
	rt_kprintf("test\n");
	rt_kprintf("test\n");
	rt_kprintf("test\n");

	mpp_video_start();
}

