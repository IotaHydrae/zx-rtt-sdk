// 240 * 284
#include "ui_common.h"


static lv_obj_t *itemt1_reset_scr;
static lv_obj_t *itemt1_reset_label1;
static lv_obj_t *itemt1_reset_label2;
static lv_obj_t *itemt1_reset_img;



static int refr_lock = 1;

static void item4t1_reset_scr_obj_create(void);
static void item4t1_reset_scr_obj_delete(void);

static void refr_cont_disp(void)
{
    
}

static void item4t1_reset_scr_load_func(void *arg)
{
    // ... ui_init
    item4t1_reset_scr_obj_create();
    refr_cont_disp();
    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    
    case E_K1:
        refr_cont_disp();
        break;
    case E_K2:
        refr_cont_disp();
        break;
    case E_K3:
        break;
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
            scr_load_func(E_ITEM4_SCR, NULL);
        break;
    default:
        break;
    }
}

static void item4t1_reset_scr_refr_func(Event_Data_t *arg)
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

static void item4t1_reset_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4t1_reset_scr_obj_delete();
}


static void item4t1_reset_scr_obj_create(void)
{
    itemt1_reset_label1 = lv_label_create(itemt1_reset_scr);
	lv_label_set_text(itemt1_reset_label1, "Factory Reset");
	lv_obj_set_style_text_font(itemt1_reset_label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(itemt1_reset_label1, lv_color_white(), 0);
    lv_obj_align(itemt1_reset_label1, LV_ALIGN_TOP_MID, 0, 10);

    itemt1_reset_label2 = lv_label_create(itemt1_reset_scr);
	lv_label_set_text(itemt1_reset_label2, "Reset To Factory Setup...");
	lv_obj_set_style_text_font(itemt1_reset_label2, &montserrat_el_17, 0);
	lv_obj_set_style_text_color(itemt1_reset_label2, lv_color_white(), 0);
    lv_obj_set_pos(itemt1_reset_label2, 65, 160);

    itemt1_reset_img = lv_img_create(itemt1_reset_scr);
    lv_img_set_src(itemt1_reset_img, LVGL_DIR"new_mask24.png");
    lv_obj_clear_flag(itemt1_reset_img, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align(itemt1_reset_img, LV_ALIGN_CENTER, 0, 0);
}   

static void item4t1_reset_scr_obj_delete(void)
{
    lv_obj_clean(itemt1_reset_scr);
}

void item4t1_reset_scr_create(void)
{
    itemt1_reset_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(itemt1_reset_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(itemt1_reset_scr, LV_OBJ_FLAG_SCROLLABLE);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4t1_reset_scr_load_func;
    _user_data->refr_func = item4t1_reset_scr_refr_func;
    _user_data->quit_func = item4t1_reset_scr_quit_func;

    scr_add_user_data(itemt1_reset_scr, E_ITEM4_RESET_SCR, _user_data);
}
