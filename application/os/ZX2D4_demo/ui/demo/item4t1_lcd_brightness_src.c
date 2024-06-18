// 240 * 284
#include "ui_common.h"

static uint8_t g_lcd_brightness = 50;
static lv_obj_t *itemt1_lcd_brightness_scr;
static lv_obj_t *itemt1_lcd_brightness_label1;
static lv_obj_t *itemt1_lcd_brightness_img;
static lv_obj_t *itemt1_lcd_brightness_bar;

static int refr_lock = 1;

static void item4t1_lcd_brightness_scr_obj_create(void);
static void item4t1_lcd_brightness_scr_obj_delete(void);
#include "pwm.h"

static void refr_cont_disp(void)
{
    lv_bar_set_value(itemt1_lcd_brightness_bar, g_lcd_brightness, LV_ANIM_OFF);
    pwm_set_duty(PWM_CH_1, PWN_SIG_B, g_lcd_brightness);   //背光
}

static void item4t1_lcd_brightness_scr_load_func(void *arg)
{
    // ... ui_init
    item4t1_lcd_brightness_scr_obj_create();
    refr_cont_disp();
    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    
    case E_K1:
        g_lcd_brightness = g_lcd_brightness + 10 > 100 ? 100 : g_lcd_brightness + 10;
        refr_cont_disp();
        break;
    case E_K2:
        g_lcd_brightness = g_lcd_brightness - 10 < 0 ? 0 : g_lcd_brightness - 10;
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

static void item4t1_lcd_brightness_scr_refr_func(Event_Data_t *arg)
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

static void item4t1_lcd_brightness_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item4t1_lcd_brightness_scr_obj_delete();
}


static void item4t1_lcd_brightness_scr_obj_create(void)
{
    itemt1_lcd_brightness_label1 = lv_label_create(itemt1_lcd_brightness_scr);
	lv_label_set_text(itemt1_lcd_brightness_label1, "LCD Brightness");
	lv_obj_set_style_text_font(itemt1_lcd_brightness_label1, &montserrat_el_19, 0);
	lv_obj_set_style_text_color(itemt1_lcd_brightness_label1, lv_color_white(), 0);
    lv_obj_align(itemt1_lcd_brightness_label1, LV_ALIGN_TOP_MID, 0, 10);


    itemt1_lcd_brightness_bar = lv_bar_create(itemt1_lcd_brightness_scr);
    lv_bar_set_range(itemt1_lcd_brightness_bar, 0, 100);
    lv_obj_set_size(itemt1_lcd_brightness_bar, 240, 18);
    lv_obj_set_style_radius(itemt1_lcd_brightness_bar, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(itemt1_lcd_brightness_bar, 9, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(itemt1_lcd_brightness_bar, lv_color_make(243, 133, 40), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(itemt1_lcd_brightness_bar, lv_color_make(62, 62, 62), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(itemt1_lcd_brightness_bar, LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(itemt1_lcd_brightness_bar, LV_ALIGN_CENTER, 15, 0);
    lv_bar_set_value(itemt1_lcd_brightness_bar, g_lcd_brightness, LV_ANIM_OFF);

    itemt1_lcd_brightness_img = lv_img_create(itemt1_lcd_brightness_scr);
    lv_img_set_src(itemt1_lcd_brightness_img, LVGL_DIR"new_mask23.png");
    lv_obj_clear_flag(itemt1_lcd_brightness_img, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_align_to(itemt1_lcd_brightness_img, itemt1_lcd_brightness_bar, LV_ALIGN_OUT_LEFT_MID, -10, 0);
}

static void item4t1_lcd_brightness_scr_obj_delete(void)
{
    lv_obj_clean(itemt1_lcd_brightness_scr);
}

void item4t1_lcd_brightness_scr_create(void)
{
    itemt1_lcd_brightness_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(itemt1_lcd_brightness_scr, lv_color_make(0, 0, 0), 0);
    lv_obj_clear_flag(itemt1_lcd_brightness_scr, LV_OBJ_FLAG_SCROLLABLE);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item4t1_lcd_brightness_scr_load_func;
    _user_data->refr_func = item4t1_lcd_brightness_scr_refr_func;
    _user_data->quit_func = item4t1_lcd_brightness_scr_quit_func;

    scr_add_user_data(itemt1_lcd_brightness_scr, E_ITEM4_LCD_BRIGHTNESS_SCR, _user_data);
}
