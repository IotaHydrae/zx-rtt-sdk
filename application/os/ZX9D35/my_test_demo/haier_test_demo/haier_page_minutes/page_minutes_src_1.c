#include "haier_test.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <math.h>

extern haier_test_obj haier_obj;
extern global_data user_data;
extern haier_src user_src_obj;       //屏幕对象
extern top_src_obj user_top_src_obj; //顶层对象

extern CUR_CONTROL_MODE cur_control_mode;      //控制模式 冷冻/冷藏
extern CUR_DRAWER_MODE cur_drawer_mode;         //变温抽屉  珍品 母婴 0°C 冷藏
extern CUR_SCENE_MODE cur_scene_mode;            //日常运行 外出节能 大量储存 智能储存
extern haier_page_minutes_obj page_minutes_obj;

page_minutes_src_1_obj minutes_src_1_obj;   


static void minutes_src_1_obj_1_cb(lv_event_t * e);
static void minutes_src_1_obj_3_cb(lv_event_t * e);
static void minutes_src_1_img_9_cb(lv_event_t * e);
static void minutes_src_1_btn_10_cb(lv_event_t * e);


void user_set_page_minutes_src_1_label_2(int value);     //速冷/杀菌
void user_set_page_minutes_src_1_label_8(int value);     //速冻

extern void set_backlight_pulse(int pulse);
extern int get_g_pulse(void);

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);    // 获取当前部件(对象)触发的事件代码
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(minutes_src_1_obj.slider_label, "%d%%", (int)lv_slider_get_value(slider));
        lv_obj_align_to(minutes_src_1_obj.slider_label, minutes_src_1_obj.slider_brightness, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

        set_backlight_pulse((int)lv_slider_get_value(slider));
    }
}

void page_minutes_src_1_menu(void)
{
    static lv_style_t style_line;
    static bool style_line_flag = false;
    static lv_point_t line_points[] = { {0, 0}, {380, 0} };
    if(!style_line_flag)
    {
        style_line_flag = true;
        lv_style_init(&style_line);
        lv_style_set_line_width(&style_line, 3);
        lv_style_set_arc_color(&style_line, lv_color_make(77, 77, 77));
    }
    //滑块
    //LV_PART_MAIN 背景     LV_PART_INDICATOR   LV_PART_KNOB 滑块
    minutes_src_1_obj.slider_brightness = lv_slider_create(lv_scr_act());
    lv_slider_set_range(minutes_src_1_obj.slider_brightness, 25, 100);
    lv_obj_set_size(minutes_src_1_obj.slider_brightness, 400, 30);
    lv_obj_align(minutes_src_1_obj.slider_brightness, LV_ALIGN_BOTTOM_MID, 0, -300);
    lv_obj_set_style_bg_opa(minutes_src_1_obj.slider_brightness, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(minutes_src_1_obj.slider_brightness, lv_color_make(204, 226, 244), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(minutes_src_1_obj.slider_brightness, lv_color_make(255, 255, 255), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(minutes_src_1_obj.slider_brightness, lv_color_hex(0xbdddba), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(minutes_src_1_obj.slider_brightness, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    minutes_src_1_obj.slider_label = lv_label_create(lv_scr_act());
    lv_slider_set_value(minutes_src_1_obj.slider_brightness, get_g_pulse()*100/5000, LV_ANIM_OFF);
    lv_label_set_text_fmt(minutes_src_1_obj.slider_label, "%d%%", get_g_pulse()*100/5000);
    lv_obj_set_style_text_color(minutes_src_1_obj.slider_label, lv_color_white(), 0);
    lv_obj_align_to(minutes_src_1_obj.slider_label, minutes_src_1_obj.slider_brightness, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);




    minutes_src_1_obj.line_1 = lv_line_create(user_src_obj.page_minutes_src_1);
    lv_line_set_points(minutes_src_1_obj.line_1, line_points, 2);
    lv_obj_add_style(minutes_src_1_obj.line_1, &style_line, LV_PART_MAIN);
    lv_obj_align(minutes_src_1_obj.line_1, LV_ALIGN_TOP_MID, 0, 200);

    minutes_src_1_obj.label_1 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_1);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_1, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_1, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_1, "冷藏");
    lv_obj_align(minutes_src_1_obj.label_1, LV_ALIGN_TOP_MID, 0, 260);

    minutes_src_1_obj.img_1 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_1, MID_SRC_1_IMG1);
    lv_obj_align_to(minutes_src_1_obj.img_1, minutes_src_1_obj.label_1, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    minutes_src_1_obj.img_2 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_2, MID_SRC_1_IMG2);
    lv_obj_align_to(minutes_src_1_obj.img_2, minutes_src_1_obj.label_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

    //

    minutes_src_1_obj.label_2 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_2);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_2, &my_font_150, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_2, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_2, "%d", 5);
    lv_obj_align(minutes_src_1_obj.label_2, LV_ALIGN_TOP_MID, 0, 340);

    minutes_src_1_obj.label_C = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_C);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_C, &my_font_30, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_C, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_C, "℃");
    lv_obj_align_to(minutes_src_1_obj.label_C, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_TOP, 1, 10);

    minutes_src_1_obj.label_3 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_3);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_3, &my_font_100, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_3, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_3, "%d", 4);
    lv_obj_align_to(minutes_src_1_obj.label_3, minutes_src_1_obj.label_2, LV_ALIGN_OUT_LEFT_MID, -50, 0);

    minutes_src_1_obj.label_temp_1 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_temp_1);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_temp_1, &my_font_40, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_temp_1, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_temp_1, "");
    lv_obj_align_to(minutes_src_1_obj.label_temp_1, minutes_src_1_obj.label_3, LV_ALIGN_OUT_LEFT_MID, 10, 0);

    
    minutes_src_1_obj.img_3 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_3, MID_SRC_1_BACKGROUD_LEFT);
    lv_obj_align(minutes_src_1_obj.img_3, LV_ALIGN_TOP_LEFT, 20, 350);

    minutes_src_1_obj.label_4 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_4);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_4, &my_font_100, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_4, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_4, "%d", 6);
    lv_obj_align_to(minutes_src_1_obj.label_4, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_MID, 50, 0);

    minutes_src_1_obj.img_3 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_3, MID_SRC_1_BACKGROUD_RIGHT);
    lv_obj_align(minutes_src_1_obj.img_3, LV_ALIGN_TOP_RIGHT, -20, 360);



    //速冷
    minutes_src_1_obj.obj_1 = lv_obj_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.obj_1);
    lv_obj_set_size(minutes_src_1_obj.obj_1, 120, 60);
    lv_obj_set_style_bg_opa(minutes_src_1_obj.obj_1, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(minutes_src_1_obj.obj_1, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(minutes_src_1_obj.obj_1, LV_OPA_100, 0);
    lv_obj_set_style_border_color(minutes_src_1_obj.obj_1, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_width(minutes_src_1_obj.obj_1, 2, 0);
    lv_obj_align(minutes_src_1_obj.obj_1, LV_ALIGN_TOP_LEFT, 70, 550);
    lv_obj_add_flag(minutes_src_1_obj.obj_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(minutes_src_1_obj.obj_1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(minutes_src_1_obj.obj_1, minutes_src_1_obj_1_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_1_obj.label_5 = lv_label_create(minutes_src_1_obj.obj_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_5);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_5, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_5, lv_color_make(228, 185, 132), 0);
    lv_label_set_text(minutes_src_1_obj.label_5, "速冷");
    lv_obj_align(minutes_src_1_obj.label_5, LV_ALIGN_CENTER, 0, 0);

    //杀菌
    minutes_src_1_obj.obj_2 = lv_obj_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.obj_2);
    lv_obj_set_size(minutes_src_1_obj.obj_2, 120, 60);
    lv_obj_set_style_bg_opa(minutes_src_1_obj.obj_2, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(minutes_src_1_obj.obj_2, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(minutes_src_1_obj.obj_2, LV_OPA_100, 0);
    lv_obj_set_style_border_color(minutes_src_1_obj.obj_2, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_width(minutes_src_1_obj.obj_2, 2, 0);
    lv_obj_align(minutes_src_1_obj.obj_2, LV_ALIGN_TOP_RIGHT, -70, 550);
    lv_obj_add_flag(minutes_src_1_obj.obj_2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(minutes_src_1_obj.obj_2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(minutes_src_1_obj.obj_2, minutes_src_1_obj_1_cb, LV_EVENT_CLICKED, NULL);   //添加事件
    
    minutes_src_1_obj.label_6 = lv_label_create(minutes_src_1_obj.obj_2);
    lv_obj_remove_style_all(minutes_src_1_obj.label_6);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_6, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_6, lv_color_make(228, 185, 132), 0);
    lv_label_set_text(minutes_src_1_obj.label_6, "杀菌");
    lv_obj_align(minutes_src_1_obj.label_6, LV_ALIGN_CENTER, 0, 0);



    minutes_src_1_obj.line_2 = lv_line_create(user_src_obj.page_minutes_src_1);
    lv_line_set_points(minutes_src_1_obj.line_2, line_points, 2);
    lv_obj_add_style(minutes_src_1_obj.line_2, &style_line, LV_PART_MAIN);
    lv_obj_align(minutes_src_1_obj.line_2, LV_ALIGN_TOP_MID, 0, 680);



    //冷冻
    minutes_src_1_obj.label_7 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_7);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_7, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_7, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_7, "冷冻");
    lv_obj_align_to(minutes_src_1_obj.label_7, minutes_src_1_obj.line_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 60);

    minutes_src_1_obj.img_5 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_5, MID_SRC_1_IMG5);
    lv_obj_align_to(minutes_src_1_obj.img_5, minutes_src_1_obj.label_7, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    //250, 190 img size
    minutes_src_1_obj.img_6 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_6, CONTROL_IMG1);      //MID_SRC_1_IMG6
    lv_obj_align_to(minutes_src_1_obj.img_6, minutes_src_1_obj.label_7, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);


    minutes_src_1_obj.label_8 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_8);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_8, &my_font_150, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_8, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_8, "%d", 18);
    lv_obj_align_to(minutes_src_1_obj.label_8, minutes_src_1_obj.line_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 130);

    minutes_src_1_obj.label_temp_8 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_temp_8);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_temp_8, &my_font_40, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_temp_8, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_temp_8, "-");
    lv_obj_align_to(minutes_src_1_obj.label_temp_8, minutes_src_1_obj.label_8, LV_ALIGN_OUT_LEFT_MID, 10, 0);


    minutes_src_1_obj.label_C_2 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_C_2);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_C_2, &my_font_30, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_C_2, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_C_2, "℃");
    lv_obj_align_to(minutes_src_1_obj.label_C_2, minutes_src_1_obj.label_8, LV_ALIGN_OUT_RIGHT_TOP, 1, 10);

    minutes_src_1_obj.label_9 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_9);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_9, &my_font_100, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_9, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_9, "%d", 17);
    lv_obj_align_to(minutes_src_1_obj.label_9, minutes_src_1_obj.label_8, LV_ALIGN_OUT_LEFT_MID, -50, 0);

    minutes_src_1_obj.label_temp_9 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_temp_9);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_temp_9, &my_font_40, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_temp_9, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_temp_9, "-");
    lv_obj_align_to(minutes_src_1_obj.label_temp_9, minutes_src_1_obj.label_9, LV_ALIGN_OUT_LEFT_MID, 10, 0);

    minutes_src_1_obj.img_7 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_7, MID_SRC_1_BACKGROUD_LEFT);
    lv_obj_align_to(minutes_src_1_obj.img_7, minutes_src_1_obj.label_8, LV_ALIGN_LEFT_MID, -170, 0);

    minutes_src_1_obj.label_10 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_10);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_10, &my_font_100, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_10, lv_color_make(255, 255, 255), 0);
    lv_label_set_text_fmt(minutes_src_1_obj.label_10, "%d", 19);
    lv_obj_align_to(minutes_src_1_obj.label_10, minutes_src_1_obj.label_8, LV_ALIGN_OUT_RIGHT_MID, 50, 0);

    minutes_src_1_obj.label_temp_10 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_temp_10);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_temp_10, &my_font_40, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_temp_10, lv_color_make(255, 255, 255), 0);
    lv_label_set_text(minutes_src_1_obj.label_temp_10, "-");
    lv_obj_align_to(minutes_src_1_obj.label_temp_10, minutes_src_1_obj.label_10, LV_ALIGN_OUT_LEFT_MID, 10, 0);

    minutes_src_1_obj.img_8 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_8, MID_SRC_1_BACKGROUD_RIGHT);
    lv_obj_align_to(minutes_src_1_obj.img_8, minutes_src_1_obj.label_8, LV_ALIGN_RIGHT_MID, 170, 0);


    //速冻
    minutes_src_1_obj.obj_3 = lv_obj_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.obj_3);
    lv_obj_set_size(minutes_src_1_obj.obj_3, 120, 60);
    lv_obj_set_style_bg_opa(minutes_src_1_obj.obj_3, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(minutes_src_1_obj.obj_3, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(minutes_src_1_obj.obj_3, LV_OPA_100, 0);
    lv_obj_set_style_border_color(minutes_src_1_obj.obj_3, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_width(minutes_src_1_obj.obj_3, 2, 0);
    lv_obj_align_to(minutes_src_1_obj.obj_3, minutes_src_1_obj.label_8, LV_ALIGN_OUT_BOTTOM_MID, 0, 60);
    lv_obj_add_flag(minutes_src_1_obj.obj_3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(minutes_src_1_obj.obj_3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(minutes_src_1_obj.obj_3, minutes_src_1_obj_3_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_1_obj.label_11 = lv_label_create(minutes_src_1_obj.obj_3);
    lv_obj_remove_style_all(minutes_src_1_obj.label_11);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_11, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_11, lv_color_make(228, 185, 132), 0);
    lv_label_set_text(minutes_src_1_obj.label_11, "速冻");
    lv_obj_align(minutes_src_1_obj.label_11, LV_ALIGN_CENTER, 0, 0);


    minutes_src_1_obj.line_3 = lv_line_create(user_src_obj.page_minutes_src_1);
    lv_line_set_points(minutes_src_1_obj.line_3, line_points, 2);
    lv_obj_add_style(minutes_src_1_obj.line_3, &style_line, LV_PART_MAIN);
    lv_obj_align(minutes_src_1_obj.line_3, LV_ALIGN_TOP_MID, 0, 1160);

    minutes_src_1_obj.label_12 = lv_label_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.label_12);
    lv_obj_set_style_text_font(minutes_src_1_obj.label_12, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_1_obj.label_12, lv_color_make(205, 205, 205), 0);
    lv_label_set_text(minutes_src_1_obj.label_12, "MSA");
    lv_obj_align_to(minutes_src_1_obj.label_12, minutes_src_1_obj.line_3, LV_ALIGN_OUT_BOTTOM_MID, 10, 30);

    minutes_src_1_obj.img_9 = lv_img_create(user_src_obj.page_minutes_src_1);
    lv_img_set_src(minutes_src_1_obj.img_9, MID_SRC_1_IMG9);
    lv_obj_align_to(minutes_src_1_obj.img_9, minutes_src_1_obj.label_12, LV_ALIGN_OUT_LEFT_MID, -3, 0);
    lv_obj_add_flag(minutes_src_1_obj.img_9, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minutes_src_1_obj.img_9, minutes_src_1_img_9_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_1_obj.line_4 = lv_line_create(user_src_obj.page_minutes_src_1);
    lv_line_set_points(minutes_src_1_obj.line_4, line_points, 2);
    lv_obj_add_style(minutes_src_1_obj.line_4, &style_line, LV_PART_MAIN);
    lv_obj_align(minutes_src_1_obj.line_4, LV_ALIGN_TOP_MID, 0, 1260);

    minutes_src_1_obj.btn_10 = lv_btn_create(user_src_obj.page_minutes_src_1);
    lv_obj_remove_style_all(minutes_src_1_obj.btn_10);
    lv_obj_set_style_bg_img_opa(minutes_src_1_obj.btn_10 , LV_OPA_0, 0);
    lv_obj_set_size(minutes_src_1_obj.btn_10, 100, 100);
    lv_obj_align_to(minutes_src_1_obj.btn_10, minutes_src_1_obj.line_4, LV_ALIGN_OUT_BOTTOM_MID, 0, 80);
    lv_obj_add_event_cb(minutes_src_1_obj.btn_10, minutes_src_1_btn_10_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_1_obj.img_10 = lv_img_create(minutes_src_1_obj.btn_10);
    lv_img_set_src(minutes_src_1_obj.img_10, MID_SRC_1_IMG10);      //
    lv_obj_align(minutes_src_1_obj.img_10, LV_ALIGN_CENTER, 0, 0);
}


static bool obj_1_flag = false;
static bool obj_2_flag = false;
static bool obj_3_flag = false;

static void minutes_src_1_obj_1_cb(lv_event_t * e)  //速冷/杀菌
{   
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *child = lv_obj_get_child(target, 0);      //label

    cur_control_mode = CONTROL_MODE1;

    if(target == minutes_src_1_obj.obj_1)       //速冷
    {
        if(obj_1_flag)
        {
            lv_obj_set_style_bg_color(target, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(child, lv_color_make(228, 185, 132), 0);
            obj_1_flag = false;
            
        }else{
            lv_obj_set_style_bg_color(target, lv_color_make(228, 185, 132), LV_PART_MAIN);
            lv_obj_set_style_text_color(child, lv_color_black(), 0);
            obj_1_flag = true;

            user_set_page_minutes_src_1_label_2(5);
            lv_obj_align_to(minutes_src_1_obj.label_C, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_TOP, 1, 10);
            lv_obj_align_to(minutes_src_1_obj.label_3, minutes_src_1_obj.label_2, LV_ALIGN_OUT_LEFT_MID, -50, 0);
            lv_obj_align_to(minutes_src_1_obj.label_4, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
            user_data.cur_temperature = 5;      //冰箱温度
            cur_control_mode = CONTROL_MODE1;   //冷藏模式

            lv_obj_set_style_bg_color(minutes_src_1_obj.obj_2, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_2, 0), lv_color_make(228, 185, 132), 0);
            obj_2_flag = false;
            lv_obj_set_style_bg_color(minutes_src_1_obj.obj_3, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_3, 0), lv_color_make(228, 185, 132), 0);
            obj_3_flag = false;

            //速冷已开启
            lv_obj_set_size(user_top_src_obj.obj_temp_1, 350, 50);
            lv_obj_set_style_bg_opa(user_top_src_obj.obj_temp_1, LV_OPA_70, 0);
            lv_obj_set_style_bg_color(user_top_src_obj.obj_temp_1, lv_color_make(228, 185, 132), LV_PART_MAIN);
            lv_obj_align(user_top_src_obj.obj_temp_1, LV_ALIGN_TOP_LEFT, 80, 100);
            lv_obj_set_style_text_color(user_top_src_obj.label_temp_1, lv_color_black(), 0);
            lv_label_set_text(user_top_src_obj.label_temp_1, "速冷已开启");
            lv_obj_center(user_top_src_obj.label_temp_1);
            lv_obj_clear_flag(user_top_src_obj.obj_temp_1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(user_top_src_obj.label_temp_1, LV_OBJ_FLAG_HIDDEN);

            lv_timer_resume(user_top_src_obj.timer);
            lv_timer_reset(user_top_src_obj.timer);
        }
        
    }
    else if(target == minutes_src_1_obj.obj_2) //杀菌
    {
        if(obj_2_flag)
        {
            lv_obj_set_style_bg_color(target, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(child, lv_color_make(228, 185, 132), 0);
            obj_2_flag = false;
        }else{
            lv_obj_set_style_bg_color(target, lv_color_make(228, 185, 132), LV_PART_MAIN);
            lv_obj_set_style_text_color(child, lv_color_black(), 0);
            obj_2_flag = true;

            user_set_page_minutes_src_1_label_2(0);
            lv_obj_align_to(minutes_src_1_obj.label_C, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_TOP, 1, 10);
            lv_obj_align_to(minutes_src_1_obj.label_3, minutes_src_1_obj.label_2, LV_ALIGN_OUT_LEFT_MID, -50, 0);
            lv_obj_align_to(minutes_src_1_obj.label_4, minutes_src_1_obj.label_2, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
            user_data.cur_temperature = 0;      //冰箱温度
            cur_control_mode = CONTROL_MODE1;   //冷藏模式
            lv_obj_set_style_bg_color(minutes_src_1_obj.obj_1, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_1, 0), lv_color_make(228, 185, 132), 0);
            obj_1_flag = false;
            lv_obj_set_style_bg_color(minutes_src_1_obj.obj_3, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_3, 0), lv_color_make(228, 185, 132), 0);
            obj_3_flag = false;

            //杀菌已开启
            lv_obj_set_size(user_top_src_obj.obj_temp_1, 350, 50);
            lv_obj_set_style_bg_opa(user_top_src_obj.obj_temp_1, LV_OPA_70, 0);
            lv_obj_set_style_bg_color(user_top_src_obj.obj_temp_1, lv_color_make(228, 185, 132), LV_PART_MAIN);
            lv_obj_align(user_top_src_obj.obj_temp_1, LV_ALIGN_TOP_LEFT, 80, 100);
            lv_obj_set_style_text_color(user_top_src_obj.label_temp_1, lv_color_black(), 0);
            lv_label_set_text(user_top_src_obj.label_temp_1, "杀菌已开启");
            lv_obj_center(user_top_src_obj.label_temp_1);
            lv_obj_clear_flag(user_top_src_obj.obj_temp_1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(user_top_src_obj.label_temp_1, LV_OBJ_FLAG_HIDDEN);

            lv_timer_resume(user_top_src_obj.timer);
            lv_timer_reset(user_top_src_obj.timer);
        }
    }
}

static void minutes_src_1_obj_3_cb(lv_event_t * e)      //速冻
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *child = lv_obj_get_child(target, 0);      //label
    cur_control_mode = CONTROL_MODE2;
    if(obj_3_flag)
    {
        lv_obj_set_style_bg_color(target, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_text_color(child, lv_color_make(228, 185, 132), 0);
        obj_3_flag = false;
        
    }else{
        lv_obj_set_style_bg_color(target, lv_color_make(228, 185, 132), LV_PART_MAIN);
        lv_obj_set_style_text_color(child, lv_color_black(), 0);
        obj_3_flag = true;

        user_set_page_minutes_src_1_label_8(-18);
        lv_obj_align_to(minutes_src_1_obj.label_C_2, minutes_src_1_obj.label_8, LV_ALIGN_OUT_RIGHT_TOP, 1, 10);
        lv_obj_align_to(minutes_src_1_obj.label_9, minutes_src_1_obj.label_8, LV_ALIGN_OUT_LEFT_MID, -50, 0);
        lv_obj_align_to(minutes_src_1_obj.label_10, minutes_src_1_obj.label_8, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
        user_data.cur_temperature = -18;
        cur_control_mode = CONTROL_MODE2;   //冷冻模式


        lv_obj_set_style_bg_color(minutes_src_1_obj.obj_2, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_2, 0), lv_color_make(228, 185, 132), 0);
        obj_2_flag = false;
        lv_obj_set_style_bg_color(minutes_src_1_obj.obj_1, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_text_color(lv_obj_get_child(minutes_src_1_obj.obj_1, 0), lv_color_make(228, 185, 132), 0);
        obj_1_flag = false;

        //速冻已开启
        lv_obj_set_size(user_top_src_obj.obj_temp_1, 350, 50);
        lv_obj_set_style_bg_opa(user_top_src_obj.obj_temp_1, LV_OPA_70, 0);
        lv_obj_set_style_bg_color(user_top_src_obj.obj_temp_1, lv_color_make(228, 185, 132), LV_PART_MAIN);
        lv_obj_align(user_top_src_obj.obj_temp_1, LV_ALIGN_TOP_LEFT, 80, 100);
        lv_obj_set_style_text_color(user_top_src_obj.label_temp_1, lv_color_black(), 0);
        lv_label_set_text(user_top_src_obj.label_temp_1, "速冻已开启");
        lv_obj_center(user_top_src_obj.label_temp_1);
        lv_obj_clear_flag(user_top_src_obj.obj_temp_1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(user_top_src_obj.label_temp_1, LV_OBJ_FLAG_HIDDEN);

        lv_timer_resume(user_top_src_obj.timer);
        lv_timer_reset(user_top_src_obj.timer);
    }
}

void user_set_page_minutes_src_1_label_2(int value)     //速冷/杀菌
{
    if(value < 0)
        lv_label_set_text_fmt(minutes_src_1_obj.label_2, "-%d", abs(value));    
    else
        lv_label_set_text_fmt(minutes_src_1_obj.label_2, "%d", value);    

    if(value - 1 < 0)
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_3, "%d", abs(value - 1));
        lv_label_set_text(minutes_src_1_obj.label_temp_1, "-");
        lv_obj_align_to(minutes_src_1_obj.label_temp_1, minutes_src_1_obj.label_3, LV_ALIGN_OUT_LEFT_MID, 10, 0);
    }
    else
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_3, "%d", value - 1);
        lv_label_set_text(minutes_src_1_obj.label_temp_1, "");
    }

    if(value + 1 < 0)
        lv_label_set_text_fmt(minutes_src_1_obj.label_4, "-%d", abs(value + 1));
    else
        lv_label_set_text_fmt(minutes_src_1_obj.label_4, "%d", value + 1);
}

void user_set_page_minutes_src_1_label_8(int value)     //速冻
{
    if(value < 0)
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_8, "%d", abs(value));    
        lv_label_set_text(minutes_src_1_obj.label_temp_8, "-");
        lv_obj_align_to(minutes_src_1_obj.label_temp_8, minutes_src_1_obj.label_8, LV_ALIGN_OUT_LEFT_MID, 10, 0);
    }
    else
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_8, "%d", value);    
        lv_label_set_text(minutes_src_1_obj.label_temp_8, "");
    }

    if(value + 1 < 0)
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_9, "%d", abs(value + 1));
        lv_label_set_text(minutes_src_1_obj.label_temp_9, "-");
        lv_obj_align_to(minutes_src_1_obj.label_temp_9, minutes_src_1_obj.label_9, LV_ALIGN_OUT_LEFT_MID, 10, 0);
    }
    else
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_9, "%d", value + 1);
        lv_label_set_text(minutes_src_1_obj.label_temp_9, "");
    }

    if(value - 1 < 0)
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_10, "%d", abs(value - 1));
        lv_label_set_text(minutes_src_1_obj.label_temp_10, "-");
        lv_obj_align_to(minutes_src_1_obj.label_temp_10, minutes_src_1_obj.label_10, LV_ALIGN_OUT_LEFT_MID, 10, 0);
    }
    else
    {
        lv_label_set_text_fmt(minutes_src_1_obj.label_10, "%d", value - 1);
        lv_label_set_text(minutes_src_1_obj.label_temp_10, "");
    }
}

static void minutes_src_1_img_9_cb(lv_event_t * e)
{
    // lv_obj_clean(user_src_obj.page_minutes_src_1);

    // lv_scr_load(user_src_obj.page_minutes_src_4);
}

static void minutes_src_1_btn_10_cb(lv_event_t * e)
{
    lv_obj_clean(user_src_obj.page_minutes_src_1);
    __user_update_layout_page_minutes_obj();
    __user_update_layout_page_minutes_pos();

    lv_scr_load(user_src_obj.maininterface_src);
    lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_middle, LV_ANIM_OFF);
}
