/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"

lv_style_t style;

void setup_scr_screen(lv_ui *ui){

	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_main_main_default
	static lv_style_t style_screen_main_main_default;
	if (style_screen_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_main_main_default);
	else
		lv_style_init(&style_screen_main_main_default);
	lv_style_set_bg_color(&style_screen_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_opa(&style_screen_main_main_default, 255);
	lv_obj_add_style(ui->screen, &style_screen_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_img_17
	ui->screen_img_17 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_17, 694, 238);
	lv_obj_set_size(ui->screen_img_17, 92, 84);
	lv_obj_set_scrollbar_mode(ui->screen_img_17, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_17_main_main_default
	static lv_style_t style_screen_img_17_main_main_default;
	if (style_screen_img_17_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_17_main_main_default);
	else
		lv_style_init(&style_screen_img_17_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_17_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_17_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_17_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_17, &style_screen_img_17_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_17, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_17,&_icon_car2_92x84);
	lv_img_set_pivot(ui->screen_img_17, 50,50);
	lv_img_set_angle(ui->screen_img_17, 0);

	//Write codes screen_img_15
	ui->screen_img_15 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_15, 517, 192);
	lv_obj_set_size(ui->screen_img_15, 74, 88);
	lv_obj_set_scrollbar_mode(ui->screen_img_15, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_15_main_main_default
	static lv_style_t style_screen_img_15_main_main_default;
	if (style_screen_img_15_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_15_main_main_default);
	else
		lv_style_init(&style_screen_img_15_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_15_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_15_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_15_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_15, &style_screen_img_15_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_15, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_15,&_icon_car1_74x88);
	lv_img_set_pivot(ui->screen_img_15, 50,50);
	lv_img_set_angle(ui->screen_img_15, 0);

	//Write codes screen_img_16
	ui->screen_img_16 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_16, 520, 309);
	lv_obj_set_size(ui->screen_img_16, 240, 149);
	lv_obj_set_scrollbar_mode(ui->screen_img_16, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_16_main_main_default
	static lv_style_t style_screen_img_16_main_main_default;
	if (style_screen_img_16_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_16_main_main_default);
	else
		lv_style_init(&style_screen_img_16_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_16_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_16_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_16_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_16, &style_screen_img_16_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_16, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_16,&_icon_car_240x149);
	lv_img_set_pivot(ui->screen_img_16, 50,50);
	lv_img_set_angle(ui->screen_img_16, 0);

	//Write codes screen_img_10
	ui->screen_img_10 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_10, 442, 178);
	lv_obj_set_size(ui->screen_img_10, 96, 298);
	lv_obj_set_scrollbar_mode(ui->screen_img_10, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_10_main_main_default
	static lv_style_t style_screen_img_10_main_main_default;
	if (style_screen_img_10_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_10_main_main_default);
	else
		lv_style_init(&style_screen_img_10_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_10_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_10_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_10_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_10, &style_screen_img_10_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_10, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_10,&_left_load_96x298);
	lv_img_set_pivot(ui->screen_img_10, 50,50);
	lv_img_set_angle(ui->screen_img_10, 0);

	//Write codes screen_img_12
	ui->screen_img_12 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_12, 552, 126);
	lv_obj_set_size(ui->screen_img_12, 62, 353);
	lv_obj_set_scrollbar_mode(ui->screen_img_12, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_12_main_main_default
	static lv_style_t style_screen_img_12_main_main_default;
	if (style_screen_img_12_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_12_main_main_default);
	else
		lv_style_init(&style_screen_img_12_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_12_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_12_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_12_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_12, &style_screen_img_12_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_12, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_12,&_left_line_62x353);
	lv_img_set_pivot(ui->screen_img_12, 50,50);
	lv_img_set_angle(ui->screen_img_12, 0);

	//Write codes screen_img_13
	ui->screen_img_13 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_13, 666, 126);
	lv_obj_set_size(ui->screen_img_13, 62, 353);
	lv_obj_set_scrollbar_mode(ui->screen_img_13, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_13_main_main_default
	static lv_style_t style_screen_img_13_main_main_default;
	if (style_screen_img_13_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_13_main_main_default);
	else
		lv_style_init(&style_screen_img_13_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_13_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_13_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_13_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_13, &style_screen_img_13_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_13, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_13,&_right_line_62x353);
	lv_img_set_pivot(ui->screen_img_13, 50,50);
	lv_img_set_angle(ui->screen_img_13, 0);

	//Write codes screen_img_11
	ui->screen_img_11 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_11, 744, 178);
	lv_obj_set_size(ui->screen_img_11, 96, 298);
	lv_obj_set_scrollbar_mode(ui->screen_img_11, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_11_main_main_default
	static lv_style_t style_screen_img_11_main_main_default;
	if (style_screen_img_11_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_11_main_main_default);
	else
		lv_style_init(&style_screen_img_11_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_11_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_11_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_11_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_11, &style_screen_img_11_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_11, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_11,&_right_load_96x298);
	lv_img_set_pivot(ui->screen_img_11, 50,50);
	lv_img_set_angle(ui->screen_img_11, 0);

	//Write codes screen_img_14
	ui->screen_img_14 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_14, 0, 1);
	lv_obj_set_size(ui->screen_img_14, 1280, 480);
	lv_obj_set_scrollbar_mode(ui->screen_img_14, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_14_main_main_default
	static lv_style_t style_screen_img_14_main_main_default;
	if (style_screen_img_14_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_14_main_main_default);
	else
		lv_style_init(&style_screen_img_14_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_14_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_14_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_14_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_14, &style_screen_img_14_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_14, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_14, SRC2_PATH_HEAD"_icon_bottom_1280x480.png");
	lv_img_set_pivot(ui->screen_img_14, 50,50);
	lv_img_set_angle(ui->screen_img_14, 0);

	//Write codes screen_img_1
	ui->screen_img_1 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_1, 223, 28);
	lv_obj_set_size(ui->screen_img_1, 38, 32);
	lv_obj_set_scrollbar_mode(ui->screen_img_1, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_1_main_main_default
	static lv_style_t style_screen_img_1_main_main_default;
	if (style_screen_img_1_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_1_main_main_default);
	else
		lv_style_init(&style_screen_img_1_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_1_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_1_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_1, &style_screen_img_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_1,&_icon_stop_38x32);
	lv_img_set_pivot(ui->screen_img_1, 50,50);
	lv_img_set_angle(ui->screen_img_1, 0);

	//Write codes screen_img_2
	ui->screen_img_2 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_2, 300, 28);
	lv_obj_set_size(ui->screen_img_2, 40, 32);
	lv_obj_set_scrollbar_mode(ui->screen_img_2, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_2_main_main_default
	static lv_style_t style_screen_img_2_main_main_default;
	if (style_screen_img_2_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_2_main_main_default);
	else
		lv_style_init(&style_screen_img_2_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_2_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_2_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_2_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_2, &style_screen_img_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_2,&_turn_left_40x32);
	lv_img_set_pivot(ui->screen_img_2, 50,50);
	lv_img_set_angle(ui->screen_img_2, 0);

	//Write codes screen_img_3
	ui->screen_img_3 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_3, 387, 0);
	lv_obj_set_size(ui->screen_img_3, 506, 83);
	lv_obj_set_scrollbar_mode(ui->screen_img_3, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_3_main_main_default
	static lv_style_t style_screen_img_3_main_main_default;
	if (style_screen_img_3_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_3_main_main_default);
	else
		lv_style_init(&style_screen_img_3_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_3_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_3_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_3_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_3, &style_screen_img_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_3, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_3,&_icon_top_bottom_506x83);
	lv_img_set_pivot(ui->screen_img_3, 50,50);
	lv_img_set_angle(ui->screen_img_3, 0);

	//Write codes screen_img_4
	ui->screen_img_4 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_4, 926, 28);
	lv_obj_set_size(ui->screen_img_4, 40, 32);
	lv_obj_set_scrollbar_mode(ui->screen_img_4, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_4_main_main_default
	static lv_style_t style_screen_img_4_main_main_default;
	if (style_screen_img_4_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_4_main_main_default);
	else
		lv_style_init(&style_screen_img_4_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_4_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_4_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_4_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_4, &style_screen_img_4_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_4, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_4,&_turn_right_40x32);
	lv_img_set_pivot(ui->screen_img_4, 50,50);
	lv_img_set_angle(ui->screen_img_4, 0);

	//Write codes screen_img_5
	ui->screen_img_5 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_5, 1114, 31);
	lv_obj_set_size(ui->screen_img_5, 44, 20);
	lv_obj_set_scrollbar_mode(ui->screen_img_5, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_5_main_main_default
	static lv_style_t style_screen_img_5_main_main_default;
	if (style_screen_img_5_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_5_main_main_default);
	else
		lv_style_init(&style_screen_img_5_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_5_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_5_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_5_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_5, &style_screen_img_5_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_5, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_5,&_icon_battery_44x20);
	lv_img_set_pivot(ui->screen_img_5, 50,50);
	lv_img_set_angle(ui->screen_img_5, 0);

	//Write codes screen_img_6
	ui->screen_img_6 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_6, 352, 202);
	lv_obj_set_size(ui->screen_img_6, 40, 40);
	lv_obj_set_scrollbar_mode(ui->screen_img_6, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_6_main_main_default
	static lv_style_t style_screen_img_6_main_main_default;
	if (style_screen_img_6_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_6_main_main_default);
	else
		lv_style_init(&style_screen_img_6_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_6_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_6_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_6_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_6, &style_screen_img_6_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_6, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_6,&_icon_speed_120_40x40);
	lv_img_set_pivot(ui->screen_img_6, 50,50);
	lv_img_set_angle(ui->screen_img_6, 0);

	//Write codes screen_img_7
	ui->screen_img_7 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_7, 352, 247);
	lv_obj_set_size(ui->screen_img_7, 42, 42);
	lv_obj_set_scrollbar_mode(ui->screen_img_7, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_7_main_main_default
	static lv_style_t style_screen_img_7_main_main_default;
	if (style_screen_img_7_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_7_main_main_default);
	else
		lv_style_init(&style_screen_img_7_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_7_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_7_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_7_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_7, &style_screen_img_7_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_7, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_7,&_icon_speed_40_42x42);
	lv_img_set_pivot(ui->screen_img_7, 50,50);
	lv_img_set_angle(ui->screen_img_7, 0);

	//Write codes screen_img_8
	ui->screen_img_8 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_8, 925, 171);
	lv_obj_set_size(ui->screen_img_8, 327, 176);
	lv_obj_set_scrollbar_mode(ui->screen_img_8, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_8_main_main_default
	static lv_style_t style_screen_img_8_main_main_default;
	if (style_screen_img_8_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_8_main_main_default);
	else
		lv_style_init(&style_screen_img_8_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_8_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_8_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_8_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_8, &style_screen_img_8_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_8, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_8,&_song_327x176);
	lv_img_set_pivot(ui->screen_img_8, 50,50);
	lv_img_set_angle(ui->screen_img_8, 0);

	//Write codes screen_img_9
	ui->screen_img_9 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_9, 1161, 346);
	lv_obj_set_size(ui->screen_img_9, 60, 20);
	lv_obj_set_scrollbar_mode(ui->screen_img_9, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_9_main_main_default
	static lv_style_t style_screen_img_9_main_main_default;
	if (style_screen_img_9_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_9_main_main_default);
	else
		lv_style_init(&style_screen_img_9_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_9_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_9_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_9_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_9, &style_screen_img_9_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_9, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_9,&_song_tips_60x20);
	lv_img_set_pivot(ui->screen_img_9, 50,50);
	lv_img_set_angle(ui->screen_img_9, 0);

	//Write codes screen_canvas_1
	ui->screen_canvas_1 = lv_canvas_create(ui->screen);
	lv_obj_set_pos(ui->screen_canvas_1, 11, 430);
	lv_obj_set_size(ui->screen_canvas_1, 100, 40);
	lv_obj_set_scrollbar_mode(ui->screen_canvas_1, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_canvas_1_main_main_default
	static lv_style_t style_screen_canvas_1_main_main_default;
	if (style_screen_canvas_1_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_canvas_1_main_main_default);
	else
		lv_style_init(&style_screen_canvas_1_main_main_default);
	lv_style_set_img_recolor(&style_screen_canvas_1_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_img_recolor_opa(&style_screen_canvas_1_main_main_default, 3);
	lv_obj_add_style(ui->screen_canvas_1, &style_screen_canvas_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	static lv_color_t buf_screen_canvas_1[100*40*4];
	lv_canvas_set_buffer(ui->screen_canvas_1, buf_screen_canvas_1, 100, 40, LV_IMG_CF_TRUE_COLOR_ALPHA);
	lv_canvas_fill_bg(ui->screen_canvas_1, lv_color_make(0x00, 0x00, 0x00), 3);

	//Write codes screen_label_1
	ui->screen_label_1 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_1, 38, 19);
	lv_obj_set_size(ui->screen_label_1, 86, 40);
	lv_obj_set_scrollbar_mode(ui->screen_label_1, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_1, "11:24");
	lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_1_main_main_default
	static lv_style_t style_screen_label_1_main_main_default;
	if (style_screen_label_1_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_1_main_main_default);
	else
		lv_style_init(&style_screen_label_1_main_main_default);
	lv_style_set_radius(&style_screen_label_1_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_1_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_1_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_1_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_26);
	lv_style_set_text_letter_space(&style_screen_label_1_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_1_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_1_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_1_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_1_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_1_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_1_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_1, &style_screen_label_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_2
	ui->screen_label_2 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_2, 546, 18);
	lv_obj_set_size(ui->screen_label_2, 26, 43);
	lv_obj_set_scrollbar_mode(ui->screen_label_2, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_2, "R");
	lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_2_main_main_default
	static lv_style_t style_screen_label_2_main_main_default;
	if (style_screen_label_2_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_2_main_main_default);
	else
		lv_style_init(&style_screen_label_2_main_main_default);
	lv_style_set_radius(&style_screen_label_2_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_2_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_2_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_2_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_2_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_30);
	lv_style_set_text_letter_space(&style_screen_label_2_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_2_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_2_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_2_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_2_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_2_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_2_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_2, &style_screen_label_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_3
	ui->screen_label_3 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_3, 713, 18);
	lv_obj_set_size(ui->screen_label_3, 26, 43);
	lv_obj_set_scrollbar_mode(ui->screen_label_3, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_3, "D");
	lv_label_set_long_mode(ui->screen_label_3, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_3_main_main_default
	static lv_style_t style_screen_label_3_main_main_default;
	if (style_screen_label_3_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_3_main_main_default);
	else
		lv_style_init(&style_screen_label_3_main_main_default);
	lv_style_set_radius(&style_screen_label_3_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_3_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_3_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_3_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_3_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_30);
	lv_style_set_text_letter_space(&style_screen_label_3_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_3_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_3_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_3_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_3_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_3_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_3_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_3, &style_screen_label_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_4
	ui->screen_label_4 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_4, 626, 14);
	lv_obj_set_size(ui->screen_label_4, 29, 46);
	lv_obj_set_scrollbar_mode(ui->screen_label_4, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_4, "N");
	lv_label_set_long_mode(ui->screen_label_4, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_4_main_main_default
	static lv_style_t style_screen_label_4_main_main_default;
	if (style_screen_label_4_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_4_main_main_default);
	else
		lv_style_init(&style_screen_label_4_main_main_default);
	lv_style_set_radius(&style_screen_label_4_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_4_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_4_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_4_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_4_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_4_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_4_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_36);
	lv_style_set_text_letter_space(&style_screen_label_4_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_4_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_4_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_4_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_4_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_4_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_4_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_4, &style_screen_label_4_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_5
	ui->screen_label_5 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_5, 1167, 19);
	lv_obj_set_size(ui->screen_label_5, 52, 40);
	lv_obj_set_scrollbar_mode(ui->screen_label_5, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_5, "426");
	lv_label_set_long_mode(ui->screen_label_5, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_5_main_main_default
	static lv_style_t style_screen_label_5_main_main_default;
	if (style_screen_label_5_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_5_main_main_default);
	else
		lv_style_init(&style_screen_label_5_main_main_default);
	lv_style_set_radius(&style_screen_label_5_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_5_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_5_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_5_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_5_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_5_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_5_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_26);
	lv_style_set_text_letter_space(&style_screen_label_5_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_5_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_5_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_5_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_5_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_5_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_5_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_5, &style_screen_label_5_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_6
	ui->screen_label_6 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_6, 1215, 22);
	lv_obj_set_size(ui->screen_label_6, 33, 35);
	lv_obj_set_scrollbar_mode(ui->screen_label_6, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_6, "KM");
	lv_label_set_long_mode(ui->screen_label_6, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_6_main_main_default
	static lv_style_t style_screen_label_6_main_main_default;
	if (style_screen_label_6_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_6_main_main_default);
	else
		lv_style_init(&style_screen_label_6_main_main_default);
	lv_style_set_radius(&style_screen_label_6_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_6_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_6_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_6_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_6_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_6_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_6_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_22);
	lv_style_set_text_letter_space(&style_screen_label_6_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_6_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_6_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_6_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_6_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_6_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_6_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_6, &style_screen_label_6_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_7
	ui->screen_label_7 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_7, 45, 165);
	lv_obj_set_size(ui->screen_label_7, 320, 180);
	lv_obj_set_scrollbar_mode(ui->screen_label_7, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_7, "120");
	lv_label_set_long_mode(ui->screen_label_7, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_7_main_main_default
	static lv_style_t style_screen_label_7_main_main_default;
	if (style_screen_label_7_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_7_main_main_default);
	else
		lv_style_init(&style_screen_label_7_main_main_default);
	lv_style_set_radius(&style_screen_label_7_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_7_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_7_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_7_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_7_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_7_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	// lv_style_set_text_font(&style_screen_label_7_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_170);
	lv_style_set_text_letter_space(&style_screen_label_7_main_main_default, 0);
	lv_style_set_text_line_space(&style_screen_label_7_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_7_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_7_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_7_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_7_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_7_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_7, &style_screen_label_7_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_8
	ui->screen_label_8 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_8, 340, 286);
	lv_obj_set_size(ui->screen_label_8, 104, 40);
	lv_obj_set_scrollbar_mode(ui->screen_label_8, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_8, "KM/H");
	lv_label_set_long_mode(ui->screen_label_8, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_8_main_main_default
	static lv_style_t style_screen_label_8_main_main_default;
	if (style_screen_label_8_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_8_main_main_default);
	else
		lv_style_init(&style_screen_label_8_main_main_default);
	lv_style_set_radius(&style_screen_label_8_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_8_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_8_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_8_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_8_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_8_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_8_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_30);
	lv_style_set_text_letter_space(&style_screen_label_8_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_8_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_8_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_8_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_8_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_8_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_8_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_8, &style_screen_label_8_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_9
	ui->screen_label_9 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_9, 142, 364);
	lv_obj_set_size(ui->screen_label_9, 205, 31);
	lv_obj_set_scrollbar_mode(ui->screen_label_9, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_9, "32.4KWH / 100KM");
	lv_label_set_long_mode(ui->screen_label_9, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_9_main_main_default
	static lv_style_t style_screen_label_9_main_main_default;
	if (style_screen_label_9_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_9_main_main_default);
	else
		lv_style_init(&style_screen_label_9_main_main_default);
	lv_style_set_radius(&style_screen_label_9_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_9_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_9_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_9_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_9_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_9_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_9_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_22);
	lv_style_set_text_letter_space(&style_screen_label_9_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_9_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_9_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_9_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_9_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_9_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_9_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_9, &style_screen_label_9_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_10
	ui->screen_label_10 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_10, 956, 335);
	lv_obj_set_size(ui->screen_label_10, 207, 35);
	lv_obj_set_scrollbar_mode(ui->screen_label_10, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_10, "ALWAYS GOLD");
	lv_label_set_long_mode(ui->screen_label_10, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_10_main_main_default
	static lv_style_t style_screen_label_10_main_main_default;
	if (style_screen_label_10_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_10_main_main_default);
	else
		lv_style_init(&style_screen_label_10_main_main_default);
	lv_style_set_radius(&style_screen_label_10_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_10_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_10_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_10_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_10_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_10_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_10_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_28);
	lv_style_set_text_letter_space(&style_screen_label_10_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_10_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_10_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_10_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_10_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_10_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_10_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_10, &style_screen_label_10_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_11
	ui->screen_label_11 = lv_label_create(ui->screen);
	lv_obj_set_pos(ui->screen_label_11, 999, 368);
	lv_obj_set_size(ui->screen_label_11, 131, 33);
	lv_obj_set_scrollbar_mode(ui->screen_label_11, LV_SCROLLBAR_MODE_OFF);
	lv_label_set_text(ui->screen_label_11, "RADICAL FACE");
	lv_label_set_long_mode(ui->screen_label_11, LV_LABEL_LONG_WRAP);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_11_main_main_default
	static lv_style_t style_screen_label_11_main_main_default;
	if (style_screen_label_11_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_label_11_main_main_default);
	else
		lv_style_init(&style_screen_label_11_main_main_default);
	lv_style_set_radius(&style_screen_label_11_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_11_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_11_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_11_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_label_11_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_11_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_font(&style_screen_label_11_main_main_default, &lv_font_ShangShouJianHeiZhongXiTi_16);
	lv_style_set_text_letter_space(&style_screen_label_11_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_label_11_main_main_default, 0);
	lv_style_set_text_align(&style_screen_label_11_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_style_set_pad_left(&style_screen_label_11_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_11_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_11_main_main_default, 8);
	lv_style_set_pad_bottom(&style_screen_label_11_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_11, &style_screen_label_11_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_bar_1
	ui->screen_bar_1 = lv_bar_create(ui->screen);
	lv_obj_set_pos(ui->screen_bar_1, 90, 356);
	lv_obj_set_size(ui->screen_bar_1, 298, 6);
	lv_obj_set_scrollbar_mode(ui->screen_bar_1, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_bar_1_main_main_default
	static lv_style_t style_screen_bar_1_main_main_default;
	if (style_screen_bar_1_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_bar_1_main_main_default);
	else
		lv_style_init(&style_screen_bar_1_main_main_default);
	lv_style_set_radius(&style_screen_bar_1_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_bar_1_main_main_default, lv_color_make(0x39, 0x39, 0x39));
	lv_style_set_bg_grad_color(&style_screen_bar_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_bar_1_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_bar_1_main_main_default, 255);
	lv_obj_add_style(ui->screen_bar_1, &style_screen_bar_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_bar_1_main_indicator_default
	static lv_style_t style_screen_bar_1_main_indicator_default;
	if (style_screen_bar_1_main_indicator_default.prop_cnt > 1)
		lv_style_reset(&style_screen_bar_1_main_indicator_default);
	else
		lv_style_init(&style_screen_bar_1_main_indicator_default);
	lv_style_set_radius(&style_screen_bar_1_main_indicator_default, 0);
	lv_style_set_bg_color(&style_screen_bar_1_main_indicator_default, lv_color_make(0x00, 0xFF, 0x12));
	lv_style_set_bg_grad_color(&style_screen_bar_1_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_bar_1_main_indicator_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_bar_1_main_indicator_default, 255);
	lv_obj_add_style(ui->screen_bar_1, &style_screen_bar_1_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_anim_time(ui->screen_bar_1, 100, 0);
	lv_bar_set_mode(ui->screen_bar_1, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->screen_bar_1, 100, LV_ANIM_OFF);

	//Write codes screen_bar_2
	ui->screen_bar_2 = lv_bar_create(ui->screen);
	lv_obj_set_pos(ui->screen_bar_2, 1117, 34);
	lv_obj_set_size(ui->screen_bar_2, 35, 14);
	lv_obj_set_scrollbar_mode(ui->screen_bar_2, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_bar_2_main_main_default
	static lv_style_t style_screen_bar_2_main_main_default;
	if (style_screen_bar_2_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_bar_2_main_main_default);
	else
		lv_style_init(&style_screen_bar_2_main_main_default);
	lv_style_set_radius(&style_screen_bar_2_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_bar_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_bar_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_bar_2_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_bar_2_main_main_default, 0);
	lv_obj_add_style(ui->screen_bar_2, &style_screen_bar_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_bar_2_main_indicator_default
	static lv_style_t style_screen_bar_2_main_indicator_default;
	if (style_screen_bar_2_main_indicator_default.prop_cnt > 1)
		lv_style_reset(&style_screen_bar_2_main_indicator_default);
	else
		lv_style_init(&style_screen_bar_2_main_indicator_default);
	lv_style_set_radius(&style_screen_bar_2_main_indicator_default, 0);
	lv_style_set_bg_color(&style_screen_bar_2_main_indicator_default, lv_color_make(0x00, 0xE1, 0x20));
	lv_style_set_bg_grad_color(&style_screen_bar_2_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_bar_2_main_indicator_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_bar_2_main_indicator_default, 255);
	lv_obj_add_style(ui->screen_bar_2, &style_screen_bar_2_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_anim_time(ui->screen_bar_2, 100, 0);
	lv_bar_set_mode(ui->screen_bar_2, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->screen_bar_2, 100, LV_ANIM_OFF);

	//Write codes screen_img_18
	ui->screen_img_18 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_18, 398, 0);
	lv_obj_set_size(ui->screen_img_18, 480, 480);
	lv_obj_set_scrollbar_mode(ui->screen_img_18, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_18_main_main_default
	static lv_style_t style_screen_img_18_main_main_default;
	if (style_screen_img_18_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_18_main_main_default);
	else
		lv_style_init(&style_screen_img_18_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_18_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_18_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_18_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_18, &style_screen_img_18_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_18, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_18, NULL);
	lv_img_set_pivot(ui->screen_img_18, 50,50);
	lv_img_set_angle(ui->screen_img_18, 0);

	//Write codes screen_img_19
	ui->screen_img_19 = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_19, 112, 374);
	lv_obj_set_size(ui->screen_img_19, 20, 20);
	lv_obj_set_scrollbar_mode(ui->screen_img_19, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_19_main_main_default
	static lv_style_t style_screen_img_19_main_main_default;
	if (style_screen_img_19_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_img_19_main_main_default);
	else
		lv_style_init(&style_screen_img_19_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_19_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_19_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_19_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_19, &style_screen_img_19_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_19, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_19,&_tips_20x20);
	lv_img_set_pivot(ui->screen_img_19, 50,50);
	lv_img_set_angle(ui->screen_img_19, 0);

#if 0
	//Write codes screen_btn_1
	ui->screen_btn_1 = lv_btn_create(ui->screen);
	lv_obj_set_pos(ui->screen_btn_1, 0, 0);
	lv_obj_set_size(ui->screen_btn_1, 1280, 1);
	lv_obj_set_scrollbar_mode(ui->screen_btn_1, LV_SCROLLBAR_MODE_OFF);

	//Write style state: LV_STATE_DEFAULT for style_screen_btn_1_main_main_default
	static lv_style_t style_screen_btn_1_main_main_default;
	if (style_screen_btn_1_main_main_default.prop_cnt > 1)
		lv_style_reset(&style_screen_btn_1_main_main_default);
	else
		lv_style_init(&style_screen_btn_1_main_main_default);
	lv_style_set_radius(&style_screen_btn_1_main_main_default, 5);
	lv_style_set_bg_color(&style_screen_btn_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_btn_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_btn_1_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_bg_opa(&style_screen_btn_1_main_main_default, 255);
	lv_style_set_border_color(&style_screen_btn_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_btn_1_main_main_default, 0);
	lv_style_set_border_opa(&style_screen_btn_1_main_main_default, 0);
	lv_style_set_text_color(&style_screen_btn_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_text_align(&style_screen_btn_1_main_main_default, LV_TEXT_ALIGN_CENTER);
	lv_obj_add_style(ui->screen_btn_1, &style_screen_btn_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	ui->screen_btn_1_label = lv_label_create(ui->screen_btn_1);
	lv_label_set_text(ui->screen_btn_1_label, "");
	lv_obj_set_style_pad_all(ui->screen_btn_1, 0, LV_STATE_DEFAULT);
	lv_obj_align(ui->screen_btn_1_label, LV_ALIGN_CENTER, 0, 0);
#endif

	lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_ShangShouJianHeiZhongXiTi_170);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
	lv_obj_add_style(ui->screen_label_7, &style, 0);


	screen_event_handle_add(ui);
}