// 240 * 284
#include "ui_common.h"

static lv_obj_t *home_scr;
static lv_obj_t *home_img[3];
static lv_obj_t *home_label1;
static lv_timer_t *home_task1;
static int refr_lock = 1;
static int obj_sel = 0; // 0 - 5, 0 -> 未选择，取消2s计时任务


static void home_scr_obj_create(void);
static void home_scr_obj_delete(void);

static void load_next_scr(void)
{
    switch (obj_sel)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        scr_load_func(E_ITEM0N5_SCR, &obj_sel);
        break;
    case 5:
        scr_load_func(E_ITEM4_SCR, &obj_sel);
        break;
    default:
        break;
    }
}

static const void *img_desc[][3] = {
    {LVGL_DIR"new_mask1.png", LVGL_DIR"new_mask0.png", LVGL_DIR"new_mask2.png"},       //Light Therapy
    {LVGL_DIR"new_mask3.png", LVGL_DIR"new_mask4.png", LVGL_DIR"new_mask5.png"},       //Beauty&Skincare
    {LVGL_DIR"new_mask2.png", LVGL_DIR"new_mask6.png", LVGL_DIR"new_mask7.png"},       //Pain Relief
    {LVGL_DIR"new_mask5.png", LVGL_DIR"new_mask8.png", LVGL_DIR"new_mask9.png"},       //Hair generating
    {LVGL_DIR"new_mask7.png", LVGL_DIR"new_mask10.png", LVGL_DIR"new_mask1.png"},      //Wound Healing
    {LVGL_DIR"new_mask9.png", LVGL_DIR"new_mask11.png", LVGL_DIR"new_mask3.png"},      //Setup
};

static const char *opt_text[] = {
    "Light Therapy",
    "Beauty&Skincare",
    "Pain Relief",
    "Wound Healing",
    "Hair regenerating",
    "Setup",
};

static void refr_cont_disp(void)
{
#if 0
    if(obj_sel == 0)
        lv_timer_pause(home_task1);
    else
    {
        lv_timer_reset(home_task1);
        lv_timer_resume(home_task1);
    }
#endif
    if(obj_sel > 5)
        return;
    lv_img_set_src(home_img[0], img_desc[obj_sel][0]);
    lv_img_set_src(home_img[1], img_desc[obj_sel][1]);
    lv_img_set_src(home_img[2], img_desc[obj_sel][2]);
    lv_label_set_text(home_label1, opt_text[obj_sel]);
}

static void auto_sel_task(lv_timer_t *t)
{
    load_next_scr();
}

static void home_scr_load_func(void *arg)
{
    // ... ui_init
    // obj_sel = 1;
    home_scr_obj_create();
    refr_cont_disp();
    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    switch (which)
    {
    case E_K4:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            obj_sel = 0;
            refr_cont_disp();
        }
        break;
    case E_K1:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            obj_sel = obj_sel + 1 > 5 ? 0 : obj_sel + 1;
            refr_cont_disp();
            // obj_sel = obj_sel - 1 < 0 ? 5 : obj_sel - 1;
            // refr_cont_disp();
        }
        break;
    case E_K2:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            // obj_sel = obj_sel + 1 > 5 ? 0 : obj_sel + 1;
            // refr_cont_disp();
            load_next_scr();
        }
        break;
    case E_K3:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {

        }
        else
            ;   // 关机
        break;
    default:
        break;
    }
}

static void home_scr_refr_func(Event_Data_t *arg)
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

static void home_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    home_scr_obj_delete();
    lv_timer_pause(home_task1);
}

static void home_scr_obj_create(void)
{
    home_img[0] = lv_img_create(home_scr);
    lv_obj_align(home_img[0], LV_ALIGN_LEFT_MID, -80, 20);

    home_img[1] = lv_img_create(home_scr);
    lv_obj_align(home_img[1], LV_ALIGN_CENTER, 0, 20);

    home_img[2] = lv_img_create(home_scr);
    lv_obj_align(home_img[2], LV_ALIGN_RIGHT_MID, 80, 20);

    home_label1 = lv_label_create(home_scr);
	lv_obj_set_style_text_font(home_label1, &montserrat_x18m, 0);
	lv_obj_set_style_text_color(home_label1, lv_color_white(), 0);
    lv_obj_align(home_label1, LV_ALIGN_BOTTOM_MID, 0, -30);
}

static void home_scr_obj_delete(void)
{
    lv_obj_clean(home_scr);
}

void home_scr_create(void)
{
    home_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(home_scr, lv_color_black(), 0);
    lv_obj_clear_flag(home_scr, LV_OBJ_FLAG_SCROLLABLE);

    home_task1 = lv_timer_create(auto_sel_task, 3000, NULL);
    lv_timer_pause(home_task1);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = home_scr_load_func;
    _user_data->refr_func = home_scr_refr_func;
    _user_data->quit_func = home_scr_quit_func;

    scr_add_user_data(home_scr, E_HOME_SCR, _user_data);
}
