#ifndef __HAIER_PAGE_MINUTES_H__
#define __HAIER_PAGE_MINUTES_H__
#include "lvgl.h"

#include "page_minutes_src_1.h"     //冰箱控制
#include "page_minutes_src_2.h"     //变温抽屉
#include "page_minutes_src_3.h"     //场景模式

typedef struct 
{
    lv_obj_t * control_line_1;
    lv_obj_t * control_obj_1;
    lv_obj_t * control_img_1;
    lv_obj_t * control_label_1;         // 冷藏/冷冻
    lv_obj_t * control_img_2;
    lv_obj_t * control_label_2;         // 冷藏/冷冻

    lv_obj_t * control_label_3;
    lv_obj_t * control_img_3;

    lv_obj_t * drawer_line_1;
    lv_obj_t * drawer_img_1;
    lv_obj_t * drawer_label_1;          //变温抽屉：珍品 母婴 0°C保鲜 冷藏
    lv_obj_t * drawer_label_2;
    lv_obj_t * drawer_img_2;

    lv_obj_t * scene_line_1;
    lv_obj_t * scene_img_1;
    lv_obj_t * scene_label_1;           //场景模式:日常运行 外出节能 大量储存 智能储存
    lv_obj_t * scene_label_2;
    lv_obj_t * scene_img_2;


}haier_page_minutes_obj;

typedef enum
{
	CONTROL_MODE1,          //冷藏
    CONTROL_MODE2,          //冷冻

    CONTROL_OTHER,                  //其它
}CUR_CONTROL_MODE;

typedef enum
{
	DRAWER_MODE1,          //珍品
    DRAWER_MODE2,          //母婴
    DRAWER_MODE3,          //0°C保险
    DRAWER_MODE4,          //冷藏

    DRAWER_OTHER,                  //其它
}CUR_DRAWER_MODE;

typedef enum
{
	SCENE_MODE1,          //日常运行
    SCENE_MODE2,          //外出节能
    SCENE_MODE3,          //大量储存
    SCENE_MODE4,          //智能储存

    SCENE_OTHER,                  //其它
}CUR_SCENE_MODE;

extern void haier_page_minutes(void);

extern void user_set_control_mode(CUR_CONTROL_MODE mode, char *src);
extern void user_set_drawer_mode(CUR_DRAWER_MODE mode, char *src);
extern void user_set_scene_mode(CUR_SCENE_MODE mode, char *src);

extern void __user_update_layout_page_minutes_pos(void);              //更新对象
extern void __user_update_layout_page_minutes_obj(void);

#endif
