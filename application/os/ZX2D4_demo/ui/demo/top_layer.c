// 240 * 284
#include "ui_common.h"


static lv_obj_t *top_layer = NULL;;

static lv_obj_t *top_icon_wifi = NULL;
static lv_obj_t *top_icon_blue = NULL;;
static lv_obj_t *top_label_hour = NULL;;
static lv_obj_t *top_label_point = NULL;;
static lv_obj_t *top_label_sec = NULL;;
static bool point_flag = false;
static lv_timer_t *top_task1 = NULL;;





static void top_layer_load_func(void *arg)
{
    if(top_task1->paused == true){
        lv_timer_resume(top_task1);
    }
    lv_obj_clear_flag(top_icon_wifi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(top_icon_blue, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(top_label_hour, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(top_label_sec, LV_OBJ_FLAG_HIDDEN);

}

static void top_layer_refr_func(Event_Data_t *arg)
{
    if(arg == NULL)
    {
        // if(sys_parm.charge_level < 8)
        //     lv_img_set_src(top_icon1, iconB_list[sys_parm.charge_level]);
        // else
        //     lv_img_set_src(top_icon1, iconB_list[1]);
    }
    else
    {
        if(arg->eEventID == E_KEY_EVENT)
            ;
    }
}

static void top_layer_quit_func(void *arg)
{

}

static void work_top_task(struct _lv_timer_t *timer)
{
    if(top_label_point)
    {
        if(point_flag){
            point_flag = false;
            lv_obj_add_flag(top_label_point, LV_OBJ_FLAG_HIDDEN);
        }
        else{
            point_flag = true;
            lv_obj_clear_flag(top_label_point, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void top_layer_create(void)
{
    top_layer = lv_layer_top();

    top_icon_wifi = lv_img_create(top_layer);
    lv_img_set_src(top_icon_wifi, LVGL_DIR"mask_top01.png");
    lv_obj_clear_flag(top_icon_wifi, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align(top_icon_wifi, LV_ALIGN_TOP_LEFT, 20, 15);
    lv_obj_add_flag(top_icon_wifi, LV_OBJ_FLAG_HIDDEN);
    
    top_icon_blue = lv_img_create(top_layer);
    lv_img_set_src(top_icon_blue, LVGL_DIR"mask_top00.png");
    lv_obj_clear_flag(top_icon_blue, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align_to(top_icon_blue, top_icon_wifi, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_flag(top_icon_blue, LV_OBJ_FLAG_HIDDEN);

    top_label_hour = lv_label_create(top_layer);
    lv_obj_set_style_text_font(top_label_hour, &montserrat_15, 0);
	lv_obj_set_style_text_color(top_label_hour, lv_color_white(), 0);
    lv_label_set_text(top_label_hour, "23");
    lv_obj_align(top_label_hour, LV_ALIGN_TOP_RIGHT, -45, 13);
    lv_obj_add_flag(top_label_hour, LV_OBJ_FLAG_HIDDEN);

    top_label_point = lv_label_create(top_layer);
    lv_obj_set_style_text_font(top_label_point, &montserrat_15, 0);
	lv_obj_set_style_text_color(top_label_point, lv_color_white(), 0);
    lv_label_set_text(top_label_point, ":");
    lv_obj_align_to(top_label_point, top_label_hour, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_add_flag(top_label_point, LV_OBJ_FLAG_HIDDEN);

    top_label_sec = lv_label_create(top_layer);
    lv_obj_set_style_text_font(top_label_sec, &montserrat_15, 0);
	lv_obj_set_style_text_color(top_label_sec, lv_color_white(), 0);
    lv_label_set_text(top_label_sec, "59");
    lv_obj_align_to(top_label_sec, top_label_hour, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_add_flag(top_label_sec, LV_OBJ_FLAG_HIDDEN);

    top_task1 = lv_timer_create(work_top_task, 500, NULL);
    lv_timer_pause(top_task1);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = top_layer_load_func;
    _user_data->refr_func = top_layer_refr_func;
    _user_data->quit_func = top_layer_quit_func;

    scr_add_user_data(top_layer, E_TOP_LAYER, _user_data);
}

