// 240 * 284
#include "ui_common.h"


static lv_obj_t *itemt1_sound_scr;
static lv_obj_t *itemt1_sound_label1;
static lv_obj_t *itemt1_sound_label2;
static lv_obj_t *itemt1_sound_label3;
static lv_obj_t *itemt1_sound_img;
static lv_obj_t *itemt1_sound_swich;


static int refr_lock = 1;
static bool sound_flag = true;

static void item4t1_sound_scr_obj_create(void);
static void item4t1_sound_scr_obj_delete(void);

static void refr_cont_disp(void)
{
    if(sound_flag)
    {
        lv_img_set_src(itemt1_sound_img, LVGL_DIR"new_mask21.png");
        lv_obj_add_state(itemt1_sound_swich, LV_STATE_CHECKED);     //OFF
    }
    else
    {
        lv_img_set_src(itemt1_sound_img, LVGL_DIR"new_mask22.png");
        lv_obj_clear_state(itemt1_sound_swich, LV_STATE_CHECKED);   //ON
    }
}

static void item4t1_sound_scr_load_func(void *arg)
{
    // ... ui_init
    item4t1_sound_scr_obj_create();
    refr_cont_disp();
    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    
    case E_K1:  
        break;
    case E_K2:  
        break;
    case E_K3:
        if(sound_flag)
        {
            sound_flag = false;//开声音
            refr_cont_disp();
        }else{
            sound_flag = true;//关声音
            refr_cont_disp();
        }
        break;
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
            scr_load_func(E_ITEM4_SCR, NULL);
        break;
    default:
        break;
    }
}

static void item4t1_sound_scr_refr_func(Event_Data_t *arg)
{
    if(refr_lock)
        return;
    if(arg == NULL)
    {
        
    }
    else
    {
        if(arg->eEventID == E_KEY_EVENT)
            btn_event_handle(arg->lDataArray[0], arg->lDataArray[1]);
    }
}

static void item4t1_sound_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4t1_sound_scr_obj_delete();
}


static void switch_event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        sound_flag = lv_obj_has_state(obj, LV_STATE_CHECKED);
    }
}
static void item4t1_sound_scr_obj_create(void)
{
    itemt1_sound_label1 = lv_label_create(itemt1_sound_scr);
	lv_label_set_text(itemt1_sound_label1, "Sound");
	lv_obj_set_style_text_font(itemt1_sound_label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(itemt1_sound_label1, lv_color_white(), 0);
    lv_obj_align(itemt1_sound_label1, LV_ALIGN_TOP_MID, 0, 10);

    itemt1_sound_swich = lv_switch_create(itemt1_sound_scr);
    lv_obj_remove_style_all(itemt1_sound_swich);
    lv_obj_set_size(itemt1_sound_swich, 83, 32);
    lv_obj_set_style_radius(itemt1_sound_swich, 45, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(itemt1_sound_swich, 45, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(itemt1_sound_swich, LV_OPA_100, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(itemt1_sound_swich, -6, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(itemt1_sound_swich, lv_color_make(243, 133, 40), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(itemt1_sound_swich, lv_color_make(0, 0, 0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(itemt1_sound_swich, LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(itemt1_sound_swich, lv_color_make(255, 255, 255), LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(itemt1_sound_swich, 3, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(itemt1_sound_swich, LV_OPA_100, LV_STATE_DEFAULT);
    lv_obj_align(itemt1_sound_swich, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(itemt1_sound_swich, switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    itemt1_sound_label2 = lv_label_create(itemt1_sound_scr);
	lv_label_set_text(itemt1_sound_label2, "ON");
	lv_obj_set_style_text_font(itemt1_sound_label2, &montserrat_el_22, 0);
	lv_obj_set_style_text_color(itemt1_sound_label2, lv_color_white(), 0);
    lv_obj_align_to(itemt1_sound_label2, itemt1_sound_swich, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    itemt1_sound_label3 = lv_label_create(itemt1_sound_scr);
	lv_label_set_text(itemt1_sound_label3, "OFF");
	lv_obj_set_style_text_font(itemt1_sound_label3, &montserrat_el_22, 0);
	lv_obj_set_style_text_color(itemt1_sound_label3, lv_color_white(), 0);
    lv_obj_align_to(itemt1_sound_label3, itemt1_sound_swich, LV_ALIGN_OUT_RIGHT_MID, 10, 0);


    itemt1_sound_img = lv_img_create(itemt1_sound_scr);
    lv_img_set_src(itemt1_sound_img, LVGL_DIR"new_mask22.png");
    lv_obj_clear_flag(itemt1_sound_img, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align_to(itemt1_sound_img, itemt1_sound_swich, LV_ALIGN_OUT_TOP_MID, 0, -10);

    if(sound_flag)
    {
        lv_img_set_src(itemt1_sound_img, LVGL_DIR"new_mask21.png");
        lv_obj_add_state(itemt1_sound_swich, LV_STATE_CHECKED);     //OFF
    }
    else
    {
        lv_img_set_src(itemt1_sound_img, LVGL_DIR"new_mask22.png");
        lv_obj_clear_state(itemt1_sound_swich, LV_STATE_CHECKED);   //ON
    }
}

static void item4t1_sound_scr_obj_delete(void)
{
    lv_obj_clean(itemt1_sound_scr);
}

void item4t1_sound_scr_create(void)
{
    itemt1_sound_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(itemt1_sound_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(itemt1_sound_scr, LV_OBJ_FLAG_SCROLLABLE);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4t1_sound_scr_load_func;
    _user_data->refr_func = item4t1_sound_scr_refr_func;
    _user_data->quit_func = item4t1_sound_scr_quit_func;

    scr_add_user_data(itemt1_sound_scr, E_ITEM4_SOUND_SCR, _user_data);
}
