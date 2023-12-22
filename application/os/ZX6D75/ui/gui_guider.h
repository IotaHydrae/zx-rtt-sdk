/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#define SRC2_PATH_HEAD "L:/ram/ramfs/"

LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_26)
LV_FONT_DECLARE(lv_font_simsun_16)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_30)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_36)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_22)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_170)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_28)
LV_FONT_DECLARE(lv_font_ShangShouJianHeiZhongXiTi_16)




typedef struct
{
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_img_17;
	lv_obj_t *screen_img_15;
	lv_obj_t *screen_img_16;
	lv_obj_t *screen_img_10;
	lv_obj_t *screen_img_12;
	lv_obj_t *screen_img_13;
	lv_obj_t *screen_img_11;
	lv_obj_t *screen_img_14;
	lv_obj_t *screen_img_1;
	lv_obj_t *screen_img_2;
	lv_obj_t *screen_img_3;
	lv_obj_t *screen_img_4;
	lv_obj_t *screen_img_5;
	lv_obj_t *screen_img_6;
	lv_obj_t *screen_img_7;
	lv_obj_t *screen_img_8;
	lv_obj_t *screen_img_9;
	lv_obj_t *screen_canvas_1;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_label_2;
	lv_obj_t *screen_label_3;
	lv_obj_t *screen_label_4;
	lv_obj_t *screen_label_5;
	lv_obj_t *screen_label_6;
	lv_obj_t *screen_label_7;
	lv_obj_t *screen_label_8;
	lv_obj_t *screen_label_9;
	lv_obj_t *screen_label_10;
	lv_obj_t *screen_label_11;
	lv_obj_t *screen_bar_1;
	lv_obj_t *screen_bar_2;
	lv_obj_t *screen_img_18;
	lv_obj_t *screen_img_19;
	lv_obj_t *screen_btn_1;
	lv_obj_t *screen_btn_1_label;
	lv_obj_t *screen_1;
	bool screen_1_del;
	lv_obj_t *screen_1_img_1;
	lv_obj_t *screen_1_img_2;
	lv_obj_t *screen_1_img_3;
	lv_obj_t *screen_1_img_4;
	lv_obj_t *screen_1_img_5;
	lv_obj_t *screen_1_img_6;
	lv_obj_t *screen_1_img_7;
	lv_obj_t *screen_1_img_8;
	lv_obj_t *screen_1_img_9;
	lv_obj_t *screen_1_img_10;
	lv_obj_t *screen_1_img_11;
	lv_obj_t *screen_1_img_12;
	lv_obj_t *screen_1_img_13;
	lv_obj_t *screen_1_img_14;
	lv_obj_t *screen_1_img_15;
	lv_obj_t *screen_1_img_16;
	lv_obj_t *screen_1_img_17;
	lv_obj_t *screen_1_img_18;
	lv_obj_t *screen_1_img_19;
	lv_obj_t *screen_1_img_20;
	lv_obj_t *screen_1_img_21;
	lv_obj_t *screen_1_img_22;
	lv_obj_t *screen_1_img_23;
	lv_obj_t *screen_1_img_24;
	lv_obj_t *screen_1_img_25;
	lv_obj_t *screen_1_img_26;
	lv_obj_t *screen_1_img_27;
	lv_obj_t *screen_1_img_28;
	lv_obj_t *screen_1_img_29;
	lv_obj_t *screen_1_img_30;
	lv_obj_t *screen_1_img_31;
	lv_obj_t *screen_1_img_32;
	lv_obj_t *screen_1_img_33;
	lv_obj_t *screen_1_img_34;
	lv_obj_t *screen_1_img_35;
	lv_obj_t *screen_1_img_36;
	lv_obj_t *screen_1_img_37;
	lv_obj_t *screen_1_img_38;
	lv_obj_t *screen_1_img_39;
	lv_obj_t *screen_1_img_40;
	lv_obj_t *screen_1_img_41;
	lv_obj_t *screen_1_img_42;
	lv_obj_t *screen_1_img_43;
	lv_obj_t *screen_1_img_44;
	lv_obj_t *screen_1_img_45;
	lv_obj_t *screen_1_img_46;
	lv_obj_t *screen_1_img_47;
	lv_obj_t *screen_1_img_48;
	lv_obj_t *screen_1_img_49;
	lv_obj_t *screen_1_img_50;
	lv_obj_t *screen_1_img_51;
	lv_obj_t *screen_1_img_52;
}lv_ui;

void init_scr_del_flag(lv_ui *ui);
void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_screen(lv_ui *ui);
void setup_scr_screen_1(lv_ui *ui);
// LV_IMG_DECLARE(_661_00009_480x480);
// LV_IMG_DECLARE(_661_00039_480x480);
// LV_IMG_DECLARE(_icon_bottom_1280x480); //---------mark
LV_IMG_DECLARE(_icon_speed_40_42x42);
// LV_IMG_DECLARE(_661_00022_480x480);
// LV_IMG_DECLARE(_661_00007_480x480);
LV_IMG_DECLARE(_tips_20x20);
// LV_IMG_DECLARE(_661_00050_480x480);
// LV_IMG_DECLARE(_661_00024_480x480);
// LV_IMG_DECLARE(_661_00041_480x480);
// LV_IMG_DECLARE(_661_00028_480x480);
LV_IMG_DECLARE(_left_line_62x353);
// LV_IMG_DECLARE(_661_00043_480x480);
// LV_IMG_DECLARE(_661_00011_480x480);
// LV_IMG_DECLARE(_661_00026_480x480);
// LV_IMG_DECLARE(_661_00013_480x480);
// LV_IMG_DECLARE(_661_00000_480x480);
// LV_IMG_DECLARE(_661_00016_480x480);
LV_IMG_DECLARE(_icon_car_240x149);
// LV_IMG_DECLARE(_661_00045_480x480);
LV_IMG_DECLARE(_icon_speed_120_40x40);
// LV_IMG_DECLARE(_661_00031_480x480);
LV_IMG_DECLARE(_right_load_96x298);
// LV_IMG_DECLARE(_661_00033_480x480);
// LV_IMG_DECLARE(_661_00002_480x480);
// LV_IMG_DECLARE(_661_00047_480x480);
LV_IMG_DECLARE(_icon_top_bottom_506x83);
// LV_IMG_DECLARE(_661_00035_480x480);
LV_IMG_DECLARE(_icon_car1_74x88);
LV_IMG_DECLARE(_icon_car2_92x84);
// LV_IMG_DECLARE(_661_00006_480x480);
// LV_IMG_DECLARE(_661_00037_480x480);
// LV_IMG_DECLARE(_661_00021_480x480);
// LV_IMG_DECLARE(_661_00049_480x480);
// LV_IMG_DECLARE(_661_00018_480x480);
// LV_IMG_DECLARE(_661_00004_480x480);
// LV_IMG_DECLARE(_661_00023_480x480);
// LV_IMG_DECLARE(_661_00025_480x480);
// LV_IMG_DECLARE(_661_00040_480x480);
// LV_IMG_DECLARE(_661_00008_480x480);
// LV_IMG_DECLARE(_661_00038_480x480);
// LV_IMG_DECLARE(_661_00010_480x480);
// LV_IMG_DECLARE(_661_00042_480x480);
// LV_IMG_DECLARE(_661_00012_480x480);
// LV_IMG_DECLARE(_661_00029_480x480);
LV_IMG_DECLARE(_icon_stop_38x32);
LV_IMG_DECLARE(_turn_right_40x32);
LV_IMG_DECLARE(_song_tips_60x20);
// LV_IMG_DECLARE(_661_00014_480x480);
// LV_IMG_DECLARE(_661_00027_480x480);
// LV_IMG_DECLARE(_661_00044_480x480);
// LV_IMG_DECLARE(_661_00030_480x480);
// LV_IMG_DECLARE(_661_00046_480x480);
// LV_IMG_DECLARE(_661_00048_480x480);
// LV_IMG_DECLARE(_661_00017_480x480);
// LV_IMG_DECLARE(_661_00032_480x480);
LV_IMG_DECLARE(_turn_left_40x32);
// LV_IMG_DECLARE(_661_00015_480x480);
// LV_IMG_DECLARE(_661_00001_480x480);
LV_IMG_DECLARE(_song_327x176);
LV_IMG_DECLARE(_right_line_62x353);
// LV_IMG_DECLARE(_661_00005_480x480);
// LV_IMG_DECLARE(_661_00020_480x480);
// LV_IMG_DECLARE(_661_00003_480x480);
// LV_IMG_DECLARE(_661_00034_480x480);
// LV_IMG_DECLARE(_661_00051_480x480);
// LV_IMG_DECLARE(_661_00019_480x480);
// LV_IMG_DECLARE(_661_00036_480x480);
LV_IMG_DECLARE(_icon_battery_44x20);
LV_IMG_DECLARE(_left_load_96x298);

#ifdef __cplusplus
}
#endif
#endif