#ifndef __PAGE_MINUTES_SRC_H__
#define __PAGE_MINUTES_SRC_H__
#include "lvgl.h"


typedef struct 
{
    lv_obj_t * line_1;
    lv_obj_t * label_1;     //冷藏
    lv_obj_t * img_1;
    lv_obj_t * img_2;

    //冷藏度数
    lv_obj_t * label_2;     //中间度数
    lv_obj_t * label_C;
    lv_obj_t * label_3;     //左度数
    lv_obj_t * img_3;       //左阴影
    lv_obj_t * label_4;     //右度数
    lv_obj_t * img_4;       //右阴影
    lv_obj_t * label_temp_1;     //-


    lv_obj_t * obj_1;     //速冷
    lv_obj_t * label_5;     //速冷

    lv_obj_t * obj_2;     //杀菌
    lv_obj_t * label_6;     //杀菌

    lv_obj_t * line_2;



    lv_obj_t * label_7;     //冷冻
    lv_obj_t * img_5;
    lv_obj_t * img_6;

    //冷冻度数
    lv_obj_t * label_temp_8;     //-
    lv_obj_t * label_temp_9;     //-
    lv_obj_t * label_temp_10;     //-
    lv_obj_t * label_8;     //中间度数
    lv_obj_t * label_C_2;
    lv_obj_t * label_9;     //左度数
    lv_obj_t * img_7;       //左阴影
    lv_obj_t * label_10;     //右度数
    lv_obj_t * img_8;       //右阴影

    lv_obj_t * obj_3;     //速冻
    lv_obj_t * label_11;     //速冻



    lv_obj_t * line_3;
    lv_obj_t * img_9;       //
    lv_obj_t * label_12;     //MSA
    lv_obj_t * line_4;

    lv_obj_t *btn_10;
    lv_obj_t * img_10;       //

    lv_obj_t * slider_brightness;
    lv_obj_t * slider_label;

    
}page_minutes_src_1_obj;

extern void page_minutes_src_1_menu(void);

#endif
