#ifndef _BEE_ANIM_H_
#define _BEE_ANIM_H_

#include "lvgl.h"
#include "gui_guider.h"

// void screen_change_anim(lv_obj_t* var_obj, int start_data, int end_data, void* cb_function);
// void obj_anim_del(lv_obj_t* obj);

void screen_obj_anim(lv_obj_t* var_obj,int time, int start_data, int end_data, void* cb_function);
void screen_obj_circulate_anim(lv_obj_t* var_obj,int time, int start_data, int end_data, void* cb_function);
void screen_speed_text_anim_cb(lv_obj_t* obj, int32_t values);
void screen_speed_battery_text_anim_cb(lv_obj_t* obj, int32_t values);
// void screen_speed_bar_anim_cb(lv_obj_t* obj, int32_t values);
void screen_speed_bar_1_anim_cb(lv_obj_t* obj, int32_t values);
void screen_speed_bar_2_anim_cb(lv_obj_t* obj, int32_t values);

#endif
