// 240 * 284
#include "ui_common.h"

static lv_obj_t *item4_info_scr;
static lv_obj_t *item4_info_label1;
static lv_obj_t *item4_info_label2;
static lv_obj_t *item4_info_label3;
static lv_obj_t *item4_info_label4;
static lv_obj_t *item4_info_cont1;

static int refr_lock = 1;


static void item4t1_info_scr_obj_create(void);
static void item4t1_info_scr_obj_delete(void);

static void item4t1_info_scr_load_func(void *arg)
{
    // ... ui_init
    item4t1_info_scr_obj_create();
    refr_lock = 0;
}

static void refr_sel_disp(int which)    //which: 0-633 1-830 2-时间
{

}

static void refr_mode_disp(void)
{

}



static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    
    case E_K1:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            
        }
  
        break;
    case E_K2:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            scr_load_func(E_ITEM4_INFO_QR_SCR, NULL);
        }

        break;
    case E_K3:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
        }

        break;
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            
            scr_load_func(E_ITEM4_SCR, NULL);
        }
        break;
    default:
        break;
    }
}

static void item4t1_info_scr_refr_func(Event_Data_t *arg)
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

static void item4t1_info_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4t1_info_scr_obj_delete();
}


static void item4t1_info_scr_obj_create(void)
{
    item4_info_label1 = lv_label_create(item4_info_scr);
	lv_label_set_text(item4_info_label1, "Device info");
	lv_obj_set_style_text_font(item4_info_label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(item4_info_label1, lv_color_white(), 0);
    lv_obj_align(item4_info_label1, LV_ALIGN_TOP_MID, 0, 10);

    item4_info_cont1 = lv_obj_create(item4_info_scr);
    lv_obj_set_size(item4_info_cont1, 288, 48);
    lv_obj_set_style_bg_color(item4_info_cont1, lv_color_make(62, 62, 62), LV_PART_MAIN);
    lv_obj_set_style_bg_color(item4_info_cont1, lv_color_make(243, 133, 40), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(item4_info_cont1, 0, 0);
    lv_obj_set_style_radius(item4_info_cont1, 12, 0);
    lv_obj_clear_flag(item4_info_cont1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_state(item4_info_cont1, LV_STATE_CHECKED);
    lv_obj_align(item4_info_cont1, LV_ALIGN_TOP_MID, 0, 144);

    lv_obj_t *img1 = lv_img_create(item4_info_cont1);
    lv_img_set_src(img1, LVGL_DIR"new_mask20.png");
    lv_obj_clear_flag(img1, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align(img1, LV_ALIGN_LEFT_MID, 30, 3);

    item4_info_label2 = lv_label_create(item4_info_cont1);
	lv_label_set_text(item4_info_label2, "QR for help link");
	lv_obj_set_style_text_font(item4_info_label2, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(item4_info_label2, lv_color_white(), 0);
    lv_obj_align(item4_info_label2, LV_ALIGN_CENTER, 20, 0);

    item4_info_label3 = lv_label_create(item4_info_scr);
	lv_label_set_text(item4_info_label3, "Hardware:V1.0");
	lv_obj_set_style_text_font(item4_info_label3, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(item4_info_label3, lv_color_white(), 0);
    lv_obj_align(item4_info_label3, LV_ALIGN_TOP_LEFT, 97, 75);

    item4_info_label4 = lv_label_create(item4_info_scr);
	lv_label_set_text(item4_info_label4, "Firmware:V1.0");
	lv_obj_set_style_text_font(item4_info_label4, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(item4_info_label4, lv_color_white(), 0);
    lv_obj_align(item4_info_label4, LV_ALIGN_TOP_LEFT, 97, 100);
}

static void item4t1_info_scr_obj_delete(void)
{
    lv_obj_clean(item4_info_scr);
}

void item4t1_info_scr_create(void)
{
    item4_info_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(item4_info_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(item4_info_scr, LV_OBJ_FLAG_SCROLLABLE);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4t1_info_scr_load_func;
    _user_data->refr_func = item4t1_info_scr_refr_func;
    _user_data->quit_func = item4t1_info_scr_quit_func;

    scr_add_user_data(item4_info_scr, E_ITEM4_INFO_SCR, _user_data);
}
