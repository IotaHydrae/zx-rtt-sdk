// 240 * 284
#include "ui_common.h"

LV_IMG_DECLARE(mask01);
LV_IMG_DECLARE(mask02);
LV_IMG_DECLARE(mask03);
LV_IMG_DECLARE(mask04);

static lv_obj_t *item4_scr;
static lv_obj_t *item4_cont1;
static lv_obj_t *item4_cont2;
static lv_obj_t *item4_cont3;
static lv_obj_t *item4_cont4;
static lv_obj_t *item4_cont5;


static lv_timer_t *item4_task1;
static int refr_lock = 1;
static int obj_sel = 0;

static void item4_scr_obj_create(void);
static void item4_scr_obj_delete(void);

static void load_next_scr(void)
{
    switch (obj_sel)
    {
    case 0:
        scr_load_func(E_ITEM4_INFO_SCR, NULL);
        break;
    case 1:
        scr_load_func(E_ITEM4_LCD_BRIGHTNESS_SCR, NULL);
        break;
    case 2:
        scr_load_func(E_ITEM4_SOUND_SCR, NULL);
        break;
    case 3:
        scr_load_func(E_ITEM4_WIFI_SCR, NULL);
        break;
    case 4:
        scr_load_func(E_ITEM4_RESET_SCR, NULL);
        break;
    default:
        break;
    }
}

static void refr_cont_disp(void)
{
    lv_obj_clear_state(item4_cont1, LV_STATE_CHECKED);
    lv_obj_clear_state(item4_cont2, LV_STATE_CHECKED);
    lv_obj_clear_state(item4_cont3, LV_STATE_CHECKED);
    lv_obj_clear_state(item4_cont4, LV_STATE_CHECKED);
    lv_obj_clear_state(item4_cont5, LV_STATE_CHECKED);
    if(obj_sel == 0)
        lv_obj_add_state(item4_cont1, LV_STATE_CHECKED);
    else if(obj_sel == 1)
        lv_obj_add_state(item4_cont2, LV_STATE_CHECKED);
    else if(obj_sel == 2)
        lv_obj_add_state(item4_cont3, LV_STATE_CHECKED);
    else if(obj_sel == 3)
        lv_obj_add_state(item4_cont4, LV_STATE_CHECKED);
    else if(obj_sel == 4)
        lv_obj_add_state(item4_cont5, LV_STATE_CHECKED);

    // lv_timer_reset(item4_task1);
    // lv_timer_resume(item4_task1);
}

static void auto_sel_task(lv_timer_t *t)
{
    load_next_scr();
}

static void item4_scr_load_func(void *arg)
{
    // ... ui_init
    // obj_sel = 0;
    item4_scr_obj_create();
    refr_cont_disp();
    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
            scr_load_func(E_HOME_SCR, NULL);
        break;
    case E_K1:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            obj_sel = obj_sel + 1 > 4 ? 0 : obj_sel + 1;
            refr_cont_disp();
        }
        break;

    case E_K2:
        if (status == E_S_BUTTON_SINGLE_CLICK)
            load_next_scr();
        else
            ; 
    case E_K3:
        break;
    default:
        break;
    }
}

static void item4_scr_refr_func(Event_Data_t *arg)
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

static void item4_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4_scr_obj_delete();
    lv_timer_pause(item4_task1);
    obj_sel = 0;
}


static void item4_scr_obj_create(void)
{
    lv_obj_t *label1 = lv_label_create(item4_scr);
	lv_label_set_text(label1, "Set Up");
	lv_obj_set_style_text_font(label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 10);

    item4_cont1 = new_scr_base_cont1_create(item4_scr,  "Device information");
    lv_obj_add_state(item4_cont1, LV_STATE_CHECKED);
    lv_obj_align(item4_cont1, LV_ALIGN_TOP_MID, 0, 52);

    item4_cont2 = new_scr_base_cont2_create(item4_scr,  "LCD Brightness");
    lv_obj_clear_state(item4_cont2, LV_STATE_CHECKED);
    lv_obj_align(item4_cont2, LV_ALIGN_TOP_LEFT, 32, 110);

    item4_cont3 = new_scr_base_cont2_create(item4_scr,  "Sound");
    lv_obj_clear_state(item4_cont3, LV_STATE_CHECKED);
    lv_obj_align(item4_cont3, LV_ALIGN_TOP_LEFT, 166, 110);

    item4_cont4 = new_scr_base_cont2_create(item4_scr,  "WiFi");
    lv_obj_clear_state(item4_cont4, LV_STATE_CHECKED);
    lv_obj_align(item4_cont4, LV_ALIGN_TOP_LEFT, 32, 168);

    item4_cont5 = new_scr_base_cont2_create(item4_scr,  "Factory Reset");
    lv_obj_clear_state(item4_cont5, LV_STATE_CHECKED);
    lv_obj_align(item4_cont5, LV_ALIGN_TOP_LEFT, 166, 168);

}

static void item4_scr_obj_delete(void)
{
    lv_obj_clean(item4_scr);
}

void item4_scr_create(void)
{
    item4_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(item4_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(item4_scr, LV_OBJ_FLAG_SCROLLABLE);

    item4_task1 = lv_timer_create(auto_sel_task, 2000, NULL);
    lv_timer_pause(item4_task1);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4_scr_load_func;
    _user_data->refr_func = item4_scr_refr_func;
    _user_data->quit_func = item4_scr_quit_func;

    scr_add_user_data(item4_scr, E_ITEM4_SCR, _user_data);
}
