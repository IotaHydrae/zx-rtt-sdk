#ifndef __PAGE_MINUTES_SRC_2_H__
#define __PAGE_MINUTES_SRC_2_H__
#include "lvgl.h"

typedef struct 
{
    lv_obj_t * line_1;
    lv_obj_t * label_1;     //变温抽屉
    lv_obj_t * img_1;
    lv_obj_t * img_2;


    lv_obj_t * label_2;     //珍品

    lv_obj_t * line_2;
    lv_obj_t * img_3;
    lv_obj_t * label_3;     //母婴


    lv_obj_t * line_3;
    lv_obj_t * img_4;
    lv_obj_t * label_4;     //0℃保鲜

    lv_obj_t * line_4;
    lv_obj_t * img_5;
    lv_obj_t * label_5;     //冷藏

    lv_obj_t * line_5;
    lv_obj_t * btn_6;       //退出回调
    lv_obj_t * img_6;       //退出回调

}page_minutes_src_2_obj;

extern void page_minutes_src_2_menu(void);

#endif
