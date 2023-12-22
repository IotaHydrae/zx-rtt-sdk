#ifndef __HAIER_PAGE_TIME_H__
#define __HAIER_PAGE_TIME_H__

#include "lvgl.h"

typedef struct 
{
    lv_obj_t *img_background;

    lv_obj_t *label_time_point;
    lv_obj_t * label_time_hours;
    lv_obj_t * label_time_minutes;
    lv_obj_t * label_time_period;
    lv_obj_t * label_Mon_Day;
    lv_obj_t * label_week;
    lv_timer_t *timer;                    //定时器
}haier_page_time_obj;

typedef struct 
{
    int cur_hour;
    int cur_minute;
    int cur_second;

    int cur_mon;
    int cur_day;
    int cur_week;
}user_time_data;

extern haier_page_time_obj page_time_obj;
extern void haier_page_time(void);

#endif
