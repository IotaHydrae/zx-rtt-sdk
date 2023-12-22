
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lvgl.h"

#include <stdio.h>
#include <stdlib.h>

#define SRC2_PATH_HEAD "L:/ram/ramfs/"

#define _480to600_y_offset	60

static int user_task_case = 0;

lv_obj_t *img001_scr;
lv_obj_t *img001_page;

lv_obj_t *img007_scr;
lv_obj_t *img009_scr;
lv_obj_t *img009_time_label;
lv_obj_t *img010_scr;

#define CANVAS_WIDTH  3700
#define CANVAS_HEIGHT  480

lv_obj_t *img002_scr;
lv_obj_t *img003_scr;
lv_obj_t *img004_scr;
lv_obj_t *img005_scr;
lv_obj_t *img011_scr;

void _process_roll_forward(lv_obj_t *obj, int value)
{
	if(value < 200)
	{
		lv_obj_set_style_bg_main_stop(obj, value, 0);
		lv_obj_set_style_bg_grad_stop(obj, value + 55, 0);
	}
	else
	{
		lv_obj_set_style_bg_main_stop(obj, value, 0);
		lv_obj_set_style_bg_grad_stop(obj, 255, 0);
	}
    lv_label_set_text_fmt(img009_time_label, "%d", 255 - value);
    lv_obj_align(img009_time_label, LV_ALIGN_CENTER, 150, -70);
}


void _process_roll_ready(struct _lv_anim_t *a)
{
	lv_scr_load(img010_scr);
}


static void jump_scr_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
	if(code == LV_EVENT_CLICKED)
	{
		lv_obj_t *scr = obj->user_data;
		if(scr != NULL)
			lv_scr_load(scr);
		if(scr == img009_scr)
		{
			if(!lv_anim_count_running())
			{
				lv_anim_t a;
				lv_anim_init(&a);
				lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) _process_roll_forward);
				lv_anim_set_ready_cb(&a, (lv_anim_ready_cb_t)_process_roll_ready);
				lv_anim_set_var(&a, img009_scr);
				lv_anim_set_time(&a, 5000);
				lv_anim_set_values(&a, 0, 255);
				lv_anim_start(&a);
			}
		}
		LV_LOG_USER("jump_scr_event_cb pass.");
	}
}

lv_obj_t *lv_user_img_create(lv_obj_t * canvas, lv_coord_t x, lv_coord_t y, const void * src, lv_obj_t *jump_scr)
{
	lv_obj_t *img = lv_img_create(canvas);
	lv_img_set_src(img, src);
	lv_obj_set_pos(img, x, y);
	lv_obj_add_event_cb(img, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
	img->user_data = jump_scr;
	return img;
}
void img001_scr_create(void)
{
	img001_page = lv_obj_create(img001_scr);
	lv_obj_set_size(img001_page, 1180, 480);
	lv_obj_align(img001_page, LV_ALIGN_BOTTOM_MID, 60, -70);
	// lv_obj_set_flex_flow(img001_page, LV_FLEX_FLOW_COLUMN_WRAP);
	lv_obj_set_style_bg_color(img001_page, lv_color_black(), 0);
	lv_obj_set_style_border_width(img001_page, 0, 0);
	lv_obj_set_scrollbar_mode(img001_page, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(img001_page, LV_DIR_HOR);

	lv_obj_t *img;
	img002_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img002_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img002_scr, _480to600_y_offset, 0);
	img = lv_img_create(img002_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image002.png");

	img003_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img003_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img003_scr, _480to600_y_offset, 0);
	img = lv_img_create(img003_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image003.png");

	img004_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img004_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img004_scr, _480to600_y_offset, 0);
	img = lv_img_create(img004_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image004.png");

	img005_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img005_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img005_scr, _480to600_y_offset, 0);
	img = lv_img_create(img005_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image005.png");

	img011_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img011_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img011_scr, _480to600_y_offset, 0);
	img = lv_img_create(img011_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image011.png");

	lv_obj_t *img002_jump_img001 = lv_obj_create(img002_scr);
	lv_obj_set_size(img002_jump_img001, 63, 63);
	lv_obj_add_event_cb(img002_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img002_jump_img001, 45, 68);
	img002_jump_img001->user_data = img001_scr;
	lv_obj_t *img002_jump_img003 = lv_obj_create(img002_scr);
	lv_obj_set_size(img002_jump_img003, 240, 35);
	lv_obj_add_event_cb(img002_jump_img003, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img002_jump_img003, 808, 54);
	img002_jump_img003->user_data = img003_scr;
	lv_obj_t *img002_jump_img004 = lv_obj_create(img002_scr);
	lv_obj_set_size(img002_jump_img004, 300, 300);
	lv_obj_add_event_cb(img002_jump_img004, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img002_jump_img004, 826, 142);
	img002_jump_img004->user_data = img004_scr;

	lv_obj_t *img003_jump_img001 = lv_obj_create(img003_scr);
	lv_obj_set_size(img003_jump_img001, 63, 63);
	lv_obj_add_event_cb(img003_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img003_jump_img001, 45, 68);
	img003_jump_img001->user_data = img001_scr;
	lv_obj_t *img003_jump_img002 = lv_obj_create(img003_scr);
	lv_obj_set_size(img003_jump_img002, 213, 35);
	lv_obj_add_event_cb(img003_jump_img002, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img003_jump_img002, 305, 54);
	img003_jump_img002->user_data = img002_scr;

	lv_obj_t *img004_jump_img001 = lv_obj_create(img004_scr);
	lv_obj_set_size(img004_jump_img001, 75, 75);
	lv_obj_add_event_cb(img004_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img004_jump_img001, 24, 36);
	img004_jump_img001->user_data = img001_scr;
	lv_obj_t *img004_jump_img005 = lv_obj_create(img004_scr);
	lv_obj_set_size(img004_jump_img005, 63, 63);
	lv_obj_add_event_cb(img004_jump_img005, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img004_jump_img005, 40, 265);
	img004_jump_img005->user_data = img005_scr;
	lv_obj_t *img004_jump_img011 = lv_obj_create(img004_scr);
	lv_obj_set_size(img004_jump_img011, 1000, 260);
	lv_obj_add_event_cb(img004_jump_img011, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img004_jump_img011, 270, 170);
	img004_jump_img011->user_data = img011_scr;

	lv_obj_t *img005_jump_img001 = lv_obj_create(img005_scr);
	lv_obj_set_size(img005_jump_img001, 75, 75);
	lv_obj_add_event_cb(img005_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img005_jump_img001, 24, 36);
	img005_jump_img001->user_data = img001_scr;
	lv_obj_t *img005_jump_img004 = lv_obj_create(img005_scr);
	lv_obj_set_size(img005_jump_img004, 63, 63);
	lv_obj_add_event_cb(img005_jump_img004, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img005_jump_img004, 40, 160);
	img005_jump_img004->user_data = img004_scr;
	lv_obj_t *img005_jump_img007 = lv_obj_create(img005_scr);
	lv_obj_set_size(img005_jump_img007, 1000, 400);
	lv_obj_add_event_cb(img005_jump_img007, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img005_jump_img007, 166, 36);
	img005_jump_img007->user_data = img007_scr;

	lv_obj_t *img011_jump_img001 = lv_obj_create(img011_scr);
	lv_obj_set_size(img011_jump_img001, 75, 75);
	lv_obj_add_event_cb(img011_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img011_jump_img001, 24, 360);
	img011_jump_img001->user_data = img001_scr;
	lv_obj_t *img011_jump_img004 = lv_obj_create(img011_scr);
	lv_obj_set_size(img011_jump_img004, 75, 75);
	lv_obj_add_event_cb(img011_jump_img004, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img011_jump_img004, 24, 36);
	img011_jump_img004->user_data = img004_scr;
	lv_obj_t *img011_jump_img009 = lv_obj_create(img011_scr);
	lv_obj_set_size(img011_jump_img009, 100, 350);
	lv_obj_add_event_cb(img011_jump_img009, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img011_jump_img009, 1170, 86);
	img011_jump_img009->user_data = img009_scr;

#if 1	//打开隐藏白色区域
	lv_obj_set_style_bg_opa(img002_jump_img001, 0, 0);
	lv_obj_set_style_bg_opa(img002_jump_img003, 0, 0);
	lv_obj_set_style_bg_opa(img002_jump_img004, 0, 0);
	lv_obj_set_style_bg_opa(img003_jump_img001, 0, 0);
	lv_obj_set_style_bg_opa(img003_jump_img002, 0, 0);
	lv_obj_set_style_bg_opa(img004_jump_img001, 0, 0);
	lv_obj_set_style_bg_opa(img004_jump_img005, 0, 0);
	lv_obj_set_style_bg_opa(img004_jump_img011, 0, 0);
	lv_obj_set_style_bg_opa(img005_jump_img001, 0, 0);
	lv_obj_set_style_bg_opa(img005_jump_img004, 0, 0);
	lv_obj_set_style_bg_opa(img005_jump_img007, 0, 0);
	lv_obj_set_style_bg_opa(img011_jump_img001, 0, 0);
	lv_obj_set_style_bg_opa(img011_jump_img004, 0, 0);
	lv_obj_set_style_bg_opa(img011_jump_img009, 0, 0);
	lv_obj_set_style_border_width(img002_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img002_jump_img003, 0, 0);
	lv_obj_set_style_border_width(img002_jump_img004, 0, 0);
	lv_obj_set_style_border_width(img003_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img003_jump_img002, 0, 0);
	lv_obj_set_style_border_width(img004_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img004_jump_img005, 0, 0);
	lv_obj_set_style_border_width(img004_jump_img011, 0, 0);
	lv_obj_set_style_border_width(img005_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img005_jump_img004, 0, 0);
	lv_obj_set_style_border_width(img005_jump_img007, 0, 0);
	lv_obj_set_style_border_width(img011_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img011_jump_img004, 0, 0);
	lv_obj_set_style_border_width(img011_jump_img009, 0, 0);
	lv_obj_set_scroll_dir(img002_jump_img001, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img002_jump_img003, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img002_jump_img004, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img003_jump_img001, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img003_jump_img002, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img004_jump_img001, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img004_jump_img005, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img004_jump_img011, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img005_jump_img001, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img005_jump_img004, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img005_jump_img007, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img011_jump_img001, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img011_jump_img004, LV_DIR_NONE);
	lv_obj_set_scroll_dir(img011_jump_img009, LV_DIR_NONE);
	
    lv_user_img_create(img001_page, -130 + 147, 36, SRC2_PATH_HEAD"main01.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 467, 36, SRC2_PATH_HEAD"main02.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 787, 36, SRC2_PATH_HEAD"main03.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 1107, 36, SRC2_PATH_HEAD"main04.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 1427, 36, SRC2_PATH_HEAD"main05.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 1746, 36, SRC2_PATH_HEAD"main06.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 2113, 0, SRC2_PATH_HEAD"main07.png", NULL);
    lv_user_img_create(img001_page, -130 + 2222, 32, SRC2_PATH_HEAD"main08.png", img004_scr);
    lv_user_img_create(img001_page, -130 + 2574, 32, SRC2_PATH_HEAD"main09.png", img007_scr);
    lv_user_img_create(img001_page, -130 + 2574, 248, SRC2_PATH_HEAD"main10.png", img007_scr);
    lv_user_img_create(img001_page, -130 + 2931, 32, SRC2_PATH_HEAD"main11.png", img007_scr);
    lv_user_img_create(img001_page, -130 + 2931, 248, SRC2_PATH_HEAD"main12.png", img007_scr);
    lv_user_img_create(img001_page, -130 + 3288, 32, SRC2_PATH_HEAD"main13.png", img007_scr);
    lv_user_img_create(img001_page, -130 + 3288, 248, SRC2_PATH_HEAD"main14.png", img007_scr);

    lv_user_img_create(img001_scr, 28, 29, SRC2_PATH_HEAD"top01.png", NULL);
    lv_user_img_create(img001_scr, 22, 50, SRC2_PATH_HEAD"top02.png", NULL);
    lv_user_img_create(img001_scr, 45, 162, SRC2_PATH_HEAD"top03.png", img002_scr);
    lv_user_img_create(img001_scr, 45, 258, SRC2_PATH_HEAD"top04.png", NULL);
    lv_user_img_create(img001_scr, 45, 353, SRC2_PATH_HEAD"top05.png", NULL);
    lv_user_img_create(img001_scr, 26, 67, SRC2_PATH_HEAD"top06.png", NULL);

#endif
}



LV_FONT_DECLARE(ali_font_40_m);
LV_FONT_DECLARE(ali_font_80_m);


static void roller_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        int num = lv_roller_get_selected(obj);
        LV_LOG_USER("Selected value: %d", num);
		lv_obj_t *mask = e->user_data;
		lv_obj_set_style_bg_main_stop(mask, 200 - 150 + (lv_roller_get_option_cnt(obj) - num) * 5, 0);
		// lv_obj_set_style_bg_grad_stop(mask, 255 - 150 + (lv_roller_get_option_cnt(obj) - num) * 2, 0);
		lv_obj_set_style_bg_grad_stop(mask, 230, 0);
    }
}

void img007_scr_create(void)
{
	lv_obj_t * label;
	lv_obj_t *obj_mask1 = lv_obj_create(img007_scr);
	lv_obj_set_size(obj_mask1, 640, 480);
	lv_obj_set_pos(obj_mask1, 0, 0);
	lv_obj_set_style_bg_opa(obj_mask1, 255, 0);
	lv_obj_set_style_bg_color(obj_mask1, lv_color_black(), 0);
	lv_obj_set_style_bg_grad_color(obj_mask1, lv_color_make(0xff, 0x63, 0x00), 0);
	lv_obj_set_style_bg_grad_dir(obj_mask1, LV_GRAD_DIR_VER, 0);
	// lv_obj_set_style_bg_main_stop(obj_mask1, 200 - 100, 0);
	// lv_obj_set_style_bg_grad_stop(obj_mask1, 255 - 100, 0);
	lv_obj_set_style_radius(obj_mask1, 0, 0);
	lv_obj_set_style_border_width(obj_mask1, 0, 0);
	lv_obj_set_scroll_dir(obj_mask1, LV_DIR_NONE);

	lv_obj_t *obj_mask2 = lv_obj_create(img007_scr);
	lv_obj_set_size(obj_mask2, 640, 480);
	lv_obj_set_pos(obj_mask2, 640, 0);
	lv_obj_set_style_bg_opa(obj_mask2, 255, 0);
	lv_obj_set_style_bg_color(obj_mask2, lv_color_black(), 0);
	lv_obj_set_style_bg_grad_color(obj_mask2, lv_color_make(0x32, 0xB9, 0x94), 0);
	lv_obj_set_style_bg_grad_dir(obj_mask2, LV_GRAD_DIR_VER, 0);
	// lv_obj_set_style_bg_main_stop(obj_mask2, 200 - 100, 0);
	// lv_obj_set_style_bg_grad_stop(obj_mask2, 255 - 100, 0);
	lv_obj_set_style_radius(obj_mask2, 0, 0);
	lv_obj_set_style_border_width(obj_mask2, 0, 0);
	lv_obj_set_scroll_dir(obj_mask2, LV_DIR_NONE);

	label = lv_label_create(img007_scr);
    lv_obj_set_style_text_font(label, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label, "温度");
	lv_obj_set_pos(label, 200, 200);
	label = lv_label_create(img007_scr);
    lv_obj_set_style_text_font(label, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label, "℃");
	lv_obj_set_pos(label, 526, 200);
	label = lv_label_create(img007_scr);
    lv_obj_set_style_text_font(label, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label, "时间");
	lv_obj_set_pos(label, 708, 200);
	label = lv_label_create(img007_scr);
    lv_obj_set_style_text_font(label, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label, "MIN");
	lv_obj_set_pos(label, 958, 200);

	lv_obj_t *roller1 = lv_roller_create(img007_scr);
	
    lv_roller_set_options(roller1,
						"100\n110\n120\n130\n140\n150\n160\n170\n180\n190\n200",
                        LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller1, 3);
	lv_obj_set_size(roller1, 140, 480);
    lv_obj_center(roller1);
	lv_obj_set_style_bg_color(roller1, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_text_color(roller1, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_width(roller1, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_SELECTED);
	lv_obj_set_style_text_font(roller1, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_font(roller1, &ali_font_80_m, LV_PART_SELECTED);
	lv_obj_set_style_text_color(roller1, lv_color_make(128, 128, 128), LV_PART_MAIN);
	lv_obj_set_style_text_color(roller1, lv_color_white(), LV_PART_SELECTED);
	lv_obj_set_style_pad_all(roller1, 50, LV_PART_SELECTED);
	lv_obj_set_style_text_line_space(roller1, 40, LV_PART_MAIN);
	lv_obj_set_style_text_letter_space(roller1, 5, LV_PART_MAIN);
	lv_obj_align_to(roller1, NULL, LV_ALIGN_OUT_LEFT_MID, 464, 0);

	lv_obj_t *roller2 = lv_roller_create(img007_scr);
    lv_roller_set_options(roller2,
                        "10\n15\n20\n25\n30\n35\n40\n45\n50\n55\n60\n65\n70\n75\n80\n85\n90",
                        LV_ROLLER_MODE_NORMAL);

    lv_roller_set_visible_row_count(roller2, 5);
	lv_obj_set_size(roller2, 120, 480);
    lv_obj_center(roller2);
	lv_obj_set_style_bg_color(roller2, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_text_color(roller2, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_bg_opa(roller2, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_width(roller2, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(roller2, LV_OPA_TRANSP, LV_PART_SELECTED);
	lv_obj_set_style_text_font(roller2, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_font(roller2, &ali_font_80_m, LV_PART_SELECTED);
	lv_obj_set_style_text_color(roller2, lv_color_make(128, 128, 128), LV_PART_MAIN);
	lv_obj_set_style_text_color(roller2, lv_color_white(), LV_PART_SELECTED);
	lv_obj_set_style_pad_all(roller2, 50, LV_PART_SELECTED);
	lv_obj_set_style_text_line_space(roller2, 40, LV_PART_MAIN);
	lv_obj_set_style_text_letter_space(roller2, 5, LV_PART_MAIN);
	lv_obj_align_to(roller2, NULL, LV_ALIGN_OUT_RIGHT_MID, -464, 0);
#if 1
    lv_obj_add_event_cb(roller1, roller_event_handler, LV_EVENT_ALL, obj_mask1);
    lv_obj_add_event_cb(roller2, roller_event_handler, LV_EVENT_ALL, obj_mask2);
	lv_obj_set_style_bg_grad_stop(obj_mask1, 230, 0);
	lv_obj_set_style_bg_grad_stop(obj_mask2, 230, 0);
	lv_obj_set_style_bg_main_stop(obj_mask1, 200 - 150 + lv_roller_get_option_cnt(roller1) * 5, 0);
	lv_obj_set_style_bg_main_stop(obj_mask2, 200 - 150 + lv_roller_get_option_cnt(roller2) * 5, 0);
#endif

	lv_user_img_create(img007_scr, 24, 36, SRC2_PATH_HEAD"list03.png", img001_scr);

	lv_obj_t *img007_jump_img009 = lv_obj_create(img007_scr);
	lv_obj_set_size(img007_jump_img009, 75, 408);
	lv_obj_add_event_cb(img007_jump_img009, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_style_radius(img007_jump_img009, 8, LV_PART_MAIN);
	lv_obj_set_style_bg_color(img007_jump_img009, lv_color_make(0xff, 0x63, 0x00), LV_PART_MAIN);
	lv_obj_set_style_border_width(img007_jump_img009, 0, LV_PART_MAIN);
	lv_obj_set_pos(img007_jump_img009, 1180, 36);
	img007_jump_img009->user_data = img009_scr;

	label = lv_label_create(img007_scr);
    lv_obj_set_style_text_font(label, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_width(label, 40);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "开始烹饪 >");
	lv_obj_set_pos(label, 1200, 100);
}

void img009_scr_create(void)
{
	lv_obj_t *obj = img009_scr;//lv_obj_create(img009_scr, 0);
	//lv_obj_set_size(obj, 1280, 480);
	lv_obj_set_style_bg_opa(obj, 255, 0);
	lv_obj_set_style_bg_color(obj, lv_color_make(0xff, 0x63, 0x00), 0);
	lv_obj_set_style_bg_grad_color(obj, lv_color_black(), 0);
	lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, 0);
	lv_obj_set_style_bg_main_stop(obj, 200 - 100, 0);
	lv_obj_set_style_bg_grad_stop(obj, 255 - 100, 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

    
	obj = lv_obj_create(img009_scr);
	lv_obj_set_size(obj, 1280, 70);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_style_bg_opa(obj, 50, 0);
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

	lv_obj_t *label1 = lv_label_create(obj);
    lv_obj_set_style_text_font(label1, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label1, "烹饪  >");
	lv_obj_align(label1, LV_ALIGN_LEFT_MID, 80, 20);

	lv_obj_t *label2 = lv_label_create(obj);
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_40, LV_PART_MAIN);
	lv_obj_set_style_text_color(label2, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label2, LV_SYMBOL_OK);
	lv_obj_align_to(label2, label1, LV_ALIGN_OUT_LEFT_MID, -15	, 0);

	obj = lv_obj_create(img009_scr);
	lv_obj_set_size(obj, 1280, 480 - 70);
	lv_obj_set_pos(obj, 0, 70);
	lv_obj_set_style_bg_opa(obj, 50, 0);
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_set_style_border_width(obj, 0, 0);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
#if 1
	label1 = lv_label_create(obj);
    lv_obj_set_style_text_font(label1, &ali_font_80_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label1, "180");
	lv_obj_align(label1, LV_ALIGN_CENTER, -150, -70);
	label2 = lv_label_create(obj);
    lv_obj_set_style_text_font(label2, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label2, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label2, "温度");
	lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);

	img009_time_label = lv_label_create(obj);
    lv_obj_set_style_text_font(img009_time_label, &ali_font_80_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(img009_time_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(img009_time_label, "28");
	lv_obj_align(img009_time_label, LV_ALIGN_CENTER, 150, -70);
	label2 = lv_label_create(obj);
    lv_obj_set_style_text_font(label2, &ali_font_40_m, LV_PART_MAIN);
	lv_obj_set_style_text_color(label2, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label2, "时间");
	lv_obj_align_to(label2, img009_time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);

	label1 = lv_label_create(obj);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_40, LV_PART_MAIN);
	lv_obj_set_style_text_color(label1, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label1, LV_SYMBOL_PAUSE);
	lv_obj_align(label1, LV_ALIGN_CENTER, 400, -70);
#endif

}

void img010_scr_create(void)
{
	lv_obj_t *img = lv_img_create(img010_scr);
	lv_img_set_src(img, SRC2_PATH_HEAD"image010.png");

	lv_obj_t *img010_jump_img001 = lv_obj_create(img010_scr);
	lv_obj_set_size(img010_jump_img001, 95, 95);
	lv_obj_add_event_cb(img010_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img010_jump_img001, 22, 35);
	img010_jump_img001->user_data = img001_scr;
	lv_obj_set_style_bg_opa(img010_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img010_jump_img001, 0, 0);
	lv_obj_set_scroll_dir(img010_jump_img001, LV_DIR_NONE);

	img010_jump_img001 = lv_obj_create(img010_scr);
	lv_obj_set_size(img010_jump_img001, 475, 104);
	lv_obj_add_event_cb(img010_jump_img001, jump_scr_event_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_set_pos(img010_jump_img001, 415, 297);
	img010_jump_img001->user_data = img001_scr;
	lv_obj_set_style_bg_opa(img010_jump_img001, 0, 0);
	lv_obj_set_style_border_width(img010_jump_img001, 0, 0);
	lv_obj_set_scroll_dir(img010_jump_img001, LV_DIR_NONE);
}

void user_task(struct _lv_timer_t *t)
{
	lv_obj_t *obj1;
	lv_obj_t *obj2;
	switch (user_task_case)
	{
		case 0:
			user_task_case = 1;
			lv_scr_load(img001_scr);
			lv_obj_set_style_anim_time(img001_page, 1500, 0);
			lv_obj_scroll_to_x(img001_page, 800, LV_ANIM_ON);
			break;
		case 1:
			user_task_case = 2;
			lv_scr_load(img007_scr);
			obj1 = lv_obj_get_child(img007_scr, 6);
			obj2 = lv_obj_get_child(img007_scr, 7);
			// lv_obj_set_style_anim_time(obj1, 2000, 0);
			// lv_obj_set_style_anim_time(obj2, 2000, 0);
			// lv_roller_set_selected(obj1, 5, LV_ANIM_ON);
			// lv_roller_set_selected(obj2, 2, LV_ANIM_ON);
			break;
		case 2:
			user_task_case = 3;
			lv_scr_load(img009_scr);
			if(!lv_anim_count_running())
			{
				lv_anim_t a;
				lv_anim_init(&a);
				lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_process_roll_forward);
				lv_anim_set_ready_cb(&a, (lv_anim_ready_cb_t)_process_roll_ready);
				lv_anim_set_var(&a, img009_scr);
				lv_anim_set_time(&a, 3000);
				lv_anim_set_values(&a, 0, 255);
				lv_anim_start(&a);
			}
			break;
		case 3:
			user_task_case = 4;
			lv_scr_load(img010_scr);
			break;
#if 0
		case 4:
			user_task_case = 5;
			lv_scr_load(img002_scr);
			break;
		case 5:
			user_task_case = 6;
			lv_scr_load(img003_scr);
			break;
		case 6:
			user_task_case = 7;
			lv_scr_load(img004_scr);
			break;
		case 7:
			user_task_case = 0;
			lv_scr_load(img005_scr);
			break;
#else
		case 4:
			user_task_case = 0;
			lv_scr_load(img001_scr);
			obj1 = lv_obj_get_child(img007_scr, 6);
			obj2 = lv_obj_get_child(img007_scr, 7);
			lv_obj_scroll_to_x(img001_page, 0, LV_ANIM_OFF);
			lv_roller_set_selected(obj1, 0, LV_ANIM_OFF);
			lv_roller_set_selected(obj2, 0, LV_ANIM_OFF);
			break;
#endif
		default:
			// lv_timer_del(t);
			break;
	}
}

void zx_ui_entry(void)
{
	img007_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(img007_scr, lv_color_black(), 0);
	lv_obj_set_style_pad_top(img007_scr, _480to600_y_offset, 0);

	img001_scr = lv_obj_create(NULL);
	lv_obj_set_style_pad_top(img001_scr, _480to600_y_offset, 0);
	lv_obj_set_style_bg_color(img001_scr, lv_color_black(), 0);
	lv_obj_set_scroll_dir(img001_scr, LV_DIR_NONE);

	img009_scr = lv_obj_create(NULL);
	lv_obj_set_style_pad_top(img009_scr, _480to600_y_offset, 0);
	lv_obj_set_style_bg_color(img009_scr, lv_color_black(), 0);
	lv_obj_set_scroll_dir(img009_scr, LV_DIR_NONE);

	img010_scr = lv_obj_create(NULL);
	lv_obj_set_style_pad_top(img010_scr, _480to600_y_offset, 0);
	lv_obj_set_style_bg_color(img010_scr, lv_color_make(0xff, 0x63, 0x00), 0);
	lv_obj_set_scroll_dir(img010_scr, LV_DIR_NONE);

	img007_scr_create();
	img001_scr_create();	// PASS
    img009_scr_create();
    img010_scr_create();
#ifdef ZX_TOUCH_NONE
	lv_timer_create(user_task, 3000, NULL);
#endif
	lv_scr_load(img001_scr);
	// lv_scr_load(img007_scr);
}
