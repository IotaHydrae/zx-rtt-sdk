#include "events_handle.h"
#include "gui_guider.h"
#include <stdlib.h>

#include <time.h>

void screen_event_handler(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
# if 0
    if(code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT){
        lv_indev_wait_release(lv_indev_get_act());/*Do nothing until the next release 在下一次发布之前不执行任何操作*/
        setup_scr_screen_check(bee);
        lv_scr_load_anim(bee->screen_check, LV_SCR_LOAD_ANIM_FADE_IN, 100, 100, false);
    }
#endif

    if(code == LV_EVENT_SCREEN_UNLOAD_START){
    }

    if(code == LV_EVENT_SCREEN_LOAD_START){
    }

    if(code == LV_EVENT_DRAW_POST_END){
        
    }
}

void screen_speed_timer_cb(lv_timer_t* t){
    static int timer_index = 0;
    lv_ui* bee = t->user_data;

    if(timer_index < 3 && timer_index >= 0){
        lv_obj_clear_flag(bee->screen_img_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(bee->screen_img_4, LV_OBJ_FLAG_HIDDEN);
    } else if(timer_index < 6 && timer_index >= 3){
        lv_obj_add_flag(bee->screen_img_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(bee->screen_img_4, LV_OBJ_FLAG_HIDDEN);
    } else if(timer_index < 9 && timer_index >= 6){
        lv_obj_add_flag(bee->screen_img_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(bee->screen_img_4, LV_OBJ_FLAG_HIDDEN);
    } else if(timer_index == 9){
        timer_index = -1;
    }
    timer_index ++;
}

void screen_rand_timer_cb(lv_timer_t* t){
    lv_ui* bee = t->user_data;

    srand((unsigned int)time(0));//初始化种子为随机值
    lv_label_set_text_fmt(bee->screen_label_9, "%d.%dKWH / 100KM", (rand()%8+32), (rand()%9));
}

void switch_gif_show(int index, lv_ui* ui){
    char path[128];
    lv_snprintf(path, 128, SRC2_PATH_HEAD"661_00%03d.png", index);
    lv_img_set_src(ui->screen_img_18,path);
}

void screen_gif_timer_cb(lv_timer_t* t){
    lv_ui* bee = t->user_data;
    static int time_index = 0;

    switch_gif_show(time_index, bee);

    time_index++;
    if(time_index > 51){
        time_index = 0;
    }
}

