#include "haier_test.h"

char *user_get_week(int week);

haier_page_time_obj page_time_obj = {0};
extern user_time_data user_cur_time;

static void haier_page_time_obj_timer_cb(lv_timer_t * timer)
{
   static bool flag = false;
    if(flag)
    {
        flag = false;
        lv_label_set_text(page_time_obj.label_time_point, " "); //18:23
    }
    else
    {
        flag = true;
        lv_label_set_text(page_time_obj.label_time_point, ":"); //18:23
    }
}

//时间页面
void haier_page_time(void)
{
    memset(&page_time_obj, 0, sizeof(page_time_obj));

    //设置背景图片
    page_time_obj.img_background = lv_img_create(haier_obj.page_time);
    lv_img_set_src(page_time_obj.img_background, PAGE_TIME_BACKGROUND);
    lv_obj_align(page_time_obj.img_background, LV_ALIGN_LEFT_MID, 0, 0);

    //label_time
    page_time_obj.label_time_point = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_time_point, &my_font_70, 0);
    lv_obj_set_style_text_color(page_time_obj.label_time_point, lv_color_white(), 0);
    lv_label_set_text(page_time_obj.label_time_point, ":"); //18:23
    lv_obj_clear_flag(page_time_obj.label_time_point, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(page_time_obj.label_time_point, LV_ALIGN_CENTER, -10, -230);

    page_time_obj.label_time_hours = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_time_hours, &my_font_70, 0);
    lv_obj_set_style_text_color(page_time_obj.label_time_hours, lv_color_white(), 0);
    lv_label_set_text_fmt(page_time_obj.label_time_hours, "%d", user_cur_time.cur_hour); //18:23
    lv_obj_clear_flag(page_time_obj.label_time_hours, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(page_time_obj.label_time_hours, page_time_obj.label_time_point, LV_ALIGN_OUT_LEFT_TOP, -10, 0);
    
    page_time_obj.label_time_minutes = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_time_minutes, &my_font_70, 0);
    lv_obj_set_style_text_color(page_time_obj.label_time_minutes, lv_color_white(), 0);
    lv_label_set_text_fmt(page_time_obj.label_time_minutes, "%d", user_cur_time.cur_minute); //18:23
    lv_obj_clear_flag(page_time_obj.label_time_minutes, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(page_time_obj.label_time_minutes, page_time_obj.label_time_point, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

    page_time_obj.label_time_period = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_time_period, &my_font_25, 0);
    lv_obj_set_style_text_color(page_time_obj.label_time_period, lv_color_white(), 0);
    if(user_cur_time.cur_hour >= 0 && user_cur_time.cur_hour <= 12)     //24小时制
        lv_label_set_text(page_time_obj.label_time_period, "上午"); 
    else if(user_cur_time.cur_hour >= 12 && user_cur_time.cur_hour <= 23)     //24小时制
        lv_label_set_text(page_time_obj.label_time_period, "下午");
    lv_obj_clear_flag(page_time_obj.label_time_period, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(page_time_obj.label_time_period, page_time_obj.label_time_minutes, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, -10);

    page_time_obj.label_Mon_Day = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_Mon_Day, &my_font_25, 0);
    lv_obj_set_style_text_color(page_time_obj.label_Mon_Day, lv_color_white(), 0);
    lv_label_set_text_fmt(page_time_obj.label_Mon_Day, "%d月%d日", user_cur_time.cur_mon, user_cur_time.cur_day);
    lv_obj_align_to(page_time_obj.label_Mon_Day, page_time_obj.label_time_hours, LV_ALIGN_OUT_BOTTOM_LEFT, 2, 0);

    page_time_obj.label_week = lv_label_create(haier_obj.page_time);
    lv_obj_set_style_text_font(page_time_obj.label_week, &my_font_25, 0);
    lv_obj_set_style_text_color(page_time_obj.label_week, lv_color_white(), 0);
    lv_label_set_text(page_time_obj.label_week, user_get_week(1));
    lv_obj_align_to(page_time_obj.label_week, page_time_obj.label_Mon_Day, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    if(page_time_obj.timer == NULL)
    {
        page_time_obj.timer = lv_timer_create(haier_page_time_obj_timer_cb, 500, NULL);
    }
}

char *user_get_week(int week)
{
    switch (week)
    {
    case 1:
        return "星期一";
        break;
    case 2:
        return "星期二";
        break;
    case 3:
        return "星期三";
        break;
    case 4:
        return "星期四";
        break;
    case 5:
        return "星期五";
        break;
    case 6:
        return "星期六";
        break;
    case 7:
        return "星期日";
        break;
    default:
        return NULL;
        break;
    }
}
