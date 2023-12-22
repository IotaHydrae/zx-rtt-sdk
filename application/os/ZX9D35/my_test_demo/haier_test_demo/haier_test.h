#ifndef __HAIER_TEST_H__
#define __HAIER_TEST_H__

#include "lvgl.h"
#include <stdio.h>

#include "haier_page_time.h"
#include "haier_page_minutes.h"
#include "haier_page_right.h"
#include "haier_page_left.h"
#include "user_png_path.h"

//屏幕
typedef struct 
{
    lv_obj_t *maininterface_src;                //主界面
    lv_obj_t *page_minutes_src_1;               //冰箱控制界面
    lv_obj_t *page_minutes_src_2;               //变温抽屉界面
    lv_obj_t *page_minutes_src_3;               //日常运行界面
    lv_obj_t *page_minutes_src_4;               //MSA界面

    lv_obj_t *page_right_src_1;                 //计时器界面
    lv_obj_t *page_right_src_2;                 //绑定手机界面
    lv_obj_t *page_right_src_3;                 //设置中心界面

    lv_obj_t *page_left_src_1;                  //使用提醒
}haier_src;

typedef struct      //顶层对象
{
    lv_obj_t *label_time;
    lv_obj_t * img_wifi;

    lv_obj_t * img_left_bottom;
    lv_obj_t * img_middle_bottom;
    lv_obj_t * img_right_bottom;

    lv_obj_t * obj_temp_1;      //字幕 速冷已开启
    lv_obj_t * label_temp_1;    //字幕 速冷已开启

    lv_timer_t *timer;                    //定时器
    lv_timer_t *timer_2;                    //定时器

}top_src_obj;

typedef struct 
{
    int cur_temperature;   //当前冰箱温度
    //当前时间
    //当前模式

}global_data;

typedef struct 
{
    lv_obj_t *haier_tv;
    lv_obj_t * page_background;
    lv_obj_t * page_time;
    lv_obj_t * page_left;
    lv_obj_t * page_middle;
    lv_obj_t * page_right;
}haier_test_obj;


extern global_data user_data;        //用户数据
extern haier_src user_src_obj;
extern haier_test_obj haier_obj;

LV_FONT_DECLARE(my_font_25);
LV_FONT_DECLARE(my_font_28);
LV_FONT_DECLARE(my_font_30);
LV_FONT_DECLARE(my_font_35);
LV_FONT_DECLARE(my_font_40);
LV_FONT_DECLARE(my_font_70);
LV_FONT_DECLARE(my_font_80);
LV_FONT_DECLARE(my_font_100);
LV_FONT_DECLARE(my_font_150);


#endif


