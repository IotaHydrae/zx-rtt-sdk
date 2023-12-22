#ifndef __HAIER_PAGE_LEFT_H__
#define __HAIER_PAGE_LEFT_H__

#include "lvgl.h"

typedef struct 
{
    lv_obj_t * img1;
    lv_obj_t * label1;      //专属空间
    lv_obj_t * label2;      //适宜存放

    lv_obj_t * btn1;        //母乳 
    lv_obj_t * btn2;        //增加母乳
    lv_obj_t * img2;        //+
    lv_obj_t * img3;        //母乳图片
    lv_obj_t * label3;      //母乳

    lv_obj_t * btn3;        //肉泥
    lv_obj_t * btn4;        //增加肉泥
    lv_obj_t * img4;        //+
    lv_obj_t * img5;        //肉泥图片
    lv_obj_t * label4;      //肉泥

    lv_obj_t * btn5;        //滴眼液
    lv_obj_t * btn6;        //增加滴眼液
    lv_obj_t * img6;        //+
    lv_obj_t * img7;        //滴眼液图片
    lv_obj_t * label5;      //滴眼液

    lv_obj_t * btn7;        //胰岛素
    lv_obj_t * btn8;        //增加胰岛素
    lv_obj_t * img8;        //+
    lv_obj_t * img9;        //胰岛素图片
    lv_obj_t * label6;      //胰岛素


    lv_obj_t * obj_temp;      //对象（用来存放存放常识)
    lv_obj_t * label7;      //存放常识
    lv_obj_t * label8;      //4         改动
    lv_obj_t * label9;      //℃
    lv_obj_t * label10;     //适宜温度

    lv_obj_t * label11;     //1         改动
    lv_obj_t * label12;     //天
    lv_obj_t * label13;     //存放天数
    

}haier_page_left_obj;

extern void haier_page_left(void);


#endif
