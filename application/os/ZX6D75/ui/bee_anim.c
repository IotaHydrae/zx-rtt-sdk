#include "bee_anim.h"

extern lv_ui bee_ui;

void screen_obj_anim(lv_obj_t* var_obj,int time, int start_data, int end_data, void* cb_function){
    lv_anim_t ANIM_obj;
    lv_anim_init(&ANIM_obj);
    lv_anim_set_time(&ANIM_obj, time);
    lv_anim_set_values(&ANIM_obj, start_data, end_data);
    lv_anim_set_path_cb(&ANIM_obj, lv_anim_path_linear);
    lv_anim_set_delay(&ANIM_obj, 10);/*开启动画前的延时时间*/
    lv_anim_set_playback_time(&ANIM_obj, 0);     /*动画返回时时间*/
    lv_anim_set_playback_delay(&ANIM_obj, 0);    /*动画执行返回时延时时间*/
    lv_anim_set_repeat_count(&ANIM_obj, LV_ANIM_REPEAT_INFINITE);      /*动画重复次数*/
    lv_anim_set_repeat_delay(&ANIM_obj, 0);      /*在重复动画之前设置延迟。*/
    lv_anim_set_early_apply(&ANIM_obj, true);    /*设置是应立即应用动画还是仅在延迟过期时应用动画。*/
    
    lv_anim_set_var(&ANIM_obj, var_obj);
    lv_anim_set_exec_cb(&ANIM_obj, (lv_anim_exec_xcb_t)cb_function);
    lv_anim_start(&ANIM_obj);
}

void screen_obj_circulate_anim(lv_obj_t* var_obj,int time, int start_data, int end_data, void* cb_function){
    lv_anim_t ANIM_obj;
    lv_anim_init(&ANIM_obj);
    lv_anim_set_time(&ANIM_obj, time);
    lv_anim_set_values(&ANIM_obj, start_data, end_data);
    lv_anim_set_path_cb(&ANIM_obj, lv_anim_path_linear);
    lv_anim_set_delay(&ANIM_obj, 10);/*开启动画前的延时时间*/
    lv_anim_set_playback_time(&ANIM_obj, time);     /*动画返回时时间*/
    lv_anim_set_playback_delay(&ANIM_obj, 0);    /*动画执行返回时延时时间*/
    lv_anim_set_repeat_count(&ANIM_obj, LV_ANIM_REPEAT_INFINITE);      /*动画重复次数*/
    lv_anim_set_repeat_delay(&ANIM_obj, 0);      /*在重复动画之前设置延迟。*/
    lv_anim_set_early_apply(&ANIM_obj, true);    /*设置是应立即应用动画还是仅在延迟过期时应用动画。*/
    
    lv_anim_set_var(&ANIM_obj, var_obj);
    lv_anim_set_exec_cb(&ANIM_obj, (lv_anim_exec_xcb_t)cb_function);
    lv_anim_start(&ANIM_obj);
}

void screen_speed_text_anim_cb(lv_obj_t* obj, int32_t values){
    lv_label_set_text_fmt(obj, "%d", values);
    if(values >= 100){
        lv_obj_set_x(bee_ui.screen_label_8, 335);
    }else{
        lv_obj_set_x(bee_ui.screen_label_8, 305); 
    }
}

void screen_speed_battery_text_anim_cb(lv_obj_t* obj, int32_t values){
    lv_label_set_text_fmt(obj, "%.3d", values);
}

void screen_speed_bar_2_anim_cb(lv_obj_t* obj, int32_t values){
    lv_bar_set_value(obj, values, LV_ANIM_OFF);
    if(values <= 20 && values > 10){
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xf6d91e), LV_PART_INDICATOR);
    }else if(values <= 10 && values > 0){
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xf60e0e), LV_PART_INDICATOR);
    }else{
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x00E120), LV_PART_INDICATOR);
    }
}

void screen_speed_bar_1_anim_cb(lv_obj_t* obj, int32_t values){
    lv_bar_set_value(obj, values, LV_ANIM_OFF);
}
