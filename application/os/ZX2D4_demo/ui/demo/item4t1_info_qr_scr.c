// 240 * 284
#include "ui_common.h"

static lv_obj_t *itemt1_info_qr_scr;
static lv_obj_t *itemt1_info_qr_label1;

static int refr_lock = 1;

static void item4t1_info_qr_scr_obj_create(void);
static void item4t1_info_qr_scr_obj_delete(void);

static void item4t1_info_qr_scr_load_func(void *arg)
{
    // ... ui_init
    item4t1_info_qr_scr_obj_create();
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
        break;
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
            scr_load_func(E_ITEM4_INFO_SCR, NULL);
        break;
    default:
        break;
    }
}

static void item4t1_info_qr_scr_refr_func(Event_Data_t *arg)
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

static void item4t1_info_qr_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4t1_info_qr_scr_obj_delete();
}


static void item4t1_info_qr_scr_obj_create(void)
{
    itemt1_info_qr_label1 = lv_label_create(itemt1_info_qr_scr);
	lv_label_set_text(itemt1_info_qr_label1, "Link For Help");
	lv_obj_set_style_text_font(itemt1_info_qr_label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(itemt1_info_qr_label1, lv_color_white(), 0);
    lv_obj_align(itemt1_info_qr_label1, LV_ALIGN_TOP_MID, 0, 10);


    lv_obj_t *img1 = lv_img_create(itemt1_info_qr_scr);
    lv_img_set_src(img1, LVGL_DIR"new_mask26.png");
    lv_obj_clear_flag(img1, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align(img1, LV_ALIGN_CENTER, 0, 10);

}

static void item4t1_info_qr_scr_obj_delete(void)
{
    lv_obj_clean(itemt1_info_qr_scr);
}

void item4t1_info_qr_scr_create(void)
{
    itemt1_info_qr_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(itemt1_info_qr_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(itemt1_info_qr_scr, LV_OBJ_FLAG_SCROLLABLE);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4t1_info_qr_scr_load_func;
    _user_data->refr_func = item4t1_info_qr_scr_refr_func;
    _user_data->quit_func = item4t1_info_qr_scr_quit_func;

    scr_add_user_data(itemt1_info_qr_scr, E_ITEM4_INFO_QR_SCR, _user_data);
}
