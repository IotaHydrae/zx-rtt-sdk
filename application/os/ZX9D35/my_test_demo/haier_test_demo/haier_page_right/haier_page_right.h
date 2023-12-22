#ifndef __HAIER_PAGE_RIGHT_H__
#define __HAIER_PAGE_RIGHT_H__

#include "lvgl.h"


typedef struct 
{
    lv_obj_t * timer_line_1;
    lv_obj_t * timer_img_1;
    lv_obj_t * timer_label_1;
    lv_obj_t * timer_img_2;

    lv_obj_t * phone_line_1;
    lv_obj_t * phone_img_1;
    lv_obj_t * phone_label_1;
    lv_obj_t * phone_img_2;

    lv_obj_t * setup_line_1;
    lv_obj_t * setup_img_1;
    lv_obj_t * setup_label_1;
    lv_obj_t * setup_img_2;

}haier_page_right_obj;

extern void haier_page_right(void);



#endif
