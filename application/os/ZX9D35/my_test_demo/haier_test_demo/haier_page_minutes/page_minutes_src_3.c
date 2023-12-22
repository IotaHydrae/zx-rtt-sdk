#include "haier_test.h"
#include <math.h>

extern void __user_update_layout_page_minutes_pos(void);              //更新对象
extern void __user_update_layout_page_minutes_obj(void);

extern haier_test_obj haier_obj;
extern global_data user_data;
extern haier_src user_src_obj;       //屏幕对象
extern top_src_obj user_top_src_obj; //顶层对象

extern CUR_CONTROL_MODE cur_control_mode;           //控制模式 冷冻/冷藏
extern CUR_DRAWER_MODE cur_drawer_mode;             //变温抽屉  珍品 母婴 0°C 冷藏
extern CUR_SCENE_MODE cur_scene_mode;               //日常运行 外出节能 大量储存 智能储存
extern haier_page_minutes_obj page_minutes_obj;


page_minutes_src_3_obj minutes_src_3_obj;   


static void __user_update_layout_pos(void);              //更新布局
static void __user_update_layout_obj(void);              //更新对象

static void minutes_src_3_btn_6_cb(lv_event_t * e);  //退出回调
static void minutes_src_2_img_2_cb(lv_event_t * e);  //日常运行回调
static void minutes_src_2_img_3_cb(lv_event_t * e);  //外出节能回调
static void minutes_src_2_img_4_cb(lv_event_t * e);  //大量存储回调
static void minutes_src_2_img_5_cb(lv_event_t * e);  //智能存储回调



void page_minutes_src_3_menu(void)
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

    minutes_src_3_obj.line_1 = lv_line_create(user_src_obj.page_minutes_src_3);
    lv_line_set_points(minutes_src_3_obj.line_1, line_points, 2);
    lv_obj_add_style(minutes_src_3_obj.line_1, &style_line, LV_PART_MAIN);

    minutes_src_3_obj.label_1 = lv_label_create(user_src_obj.page_minutes_src_3);
    lv_obj_remove_style_all(minutes_src_3_obj.label_1);
    lv_obj_set_style_text_font(minutes_src_3_obj.label_1, &my_font_35, 0);
    lv_obj_set_style_text_color(minutes_src_3_obj.label_1, lv_color_make(211, 211, 211), 0);
    lv_label_set_text(minutes_src_3_obj.label_1, "场景模式");

    minutes_src_3_obj.img_1 = lv_img_create(user_src_obj.page_minutes_src_3);
    lv_img_set_src(minutes_src_3_obj.img_1, SCENE_IMG5);       //MID_SRC_3_IMG_1
    

    


    //日常运行
    minutes_src_3_obj.img_2 = lv_img_create(user_src_obj.page_minutes_src_3);
    lv_obj_add_flag(minutes_src_3_obj.img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minutes_src_3_obj.img_2, minutes_src_2_img_2_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_3_obj.label_2 = lv_label_create(user_src_obj.page_minutes_src_3);


    minutes_src_3_obj.line_2 = lv_line_create(user_src_obj.page_minutes_src_3);
    lv_line_set_points(minutes_src_3_obj.line_2, line_points, 2);
    lv_obj_add_style(minutes_src_3_obj.line_2, &style_line, LV_PART_MAIN);




    //外出节能
    minutes_src_3_obj.img_3 = lv_img_create(user_src_obj.page_minutes_src_3);
    lv_obj_add_flag(minutes_src_3_obj.img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minutes_src_3_obj.img_3, minutes_src_2_img_3_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_3_obj.label_3 = lv_label_create(user_src_obj.page_minutes_src_3);


    minutes_src_3_obj.line_3 = lv_line_create(user_src_obj.page_minutes_src_3);
    lv_line_set_points(minutes_src_3_obj.line_3, line_points, 2);
    lv_obj_add_style(minutes_src_3_obj.line_3, &style_line, LV_PART_MAIN);


    

    //大量存储
    minutes_src_3_obj.img_4 = lv_img_create(user_src_obj.page_minutes_src_3);
    lv_obj_add_flag(minutes_src_3_obj.img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minutes_src_3_obj.img_4, minutes_src_2_img_4_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_3_obj.label_4 = lv_label_create(user_src_obj.page_minutes_src_3);


    minutes_src_3_obj.line_4 = lv_line_create(user_src_obj.page_minutes_src_3);
    lv_line_set_points(minutes_src_3_obj.line_4, line_points, 2);
    lv_obj_add_style(minutes_src_3_obj.line_4, &style_line, LV_PART_MAIN);


    

    //智能存储
    minutes_src_3_obj.img_5 = lv_img_create(user_src_obj.page_minutes_src_3);
    lv_obj_add_flag(minutes_src_3_obj.img_5, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minutes_src_3_obj.img_5, minutes_src_2_img_5_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_3_obj.label_5 = lv_label_create(user_src_obj.page_minutes_src_3);


    minutes_src_3_obj.line_5 = lv_line_create(user_src_obj.page_minutes_src_3);
    lv_line_set_points(minutes_src_3_obj.line_5, line_points, 2);
    lv_obj_add_style(minutes_src_3_obj.line_5, &style_line, LV_PART_MAIN);
    

    minutes_src_3_obj.btn_6 = lv_btn_create(user_src_obj.page_minutes_src_3);
    lv_obj_remove_style_all(minutes_src_3_obj.btn_6);
    lv_obj_set_style_bg_img_opa(minutes_src_3_obj.btn_6 , LV_OPA_0, 0);
    lv_obj_set_size(minutes_src_3_obj.btn_6, 100, 100);
    lv_obj_add_event_cb(minutes_src_3_obj.btn_6, minutes_src_3_btn_6_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    minutes_src_3_obj.img_6 = lv_img_create(minutes_src_3_obj.btn_6);
    lv_img_set_src(minutes_src_3_obj.img_6, MID_SRC_1_IMG10);       //MID_SRC_3_IMG_10

    __user_update_layout_obj();
    __user_update_layout_pos();
}
static void __user_update_layout_pos(void)
{
    //场景模式
    lv_obj_align(minutes_src_3_obj.line_1, LV_ALIGN_TOP_MID, 0, 260);
    lv_obj_align_to(minutes_src_3_obj.label_1, minutes_src_3_obj.line_1, LV_ALIGN_OUT_TOP_MID, 0, -15);
    lv_obj_align_to(minutes_src_3_obj.img_1, minutes_src_3_obj.label_1, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    //日常运行
    lv_obj_align_to(minutes_src_3_obj.img_2, minutes_src_3_obj.line_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 55);         //日常运行图片
    lv_obj_align_to(minutes_src_3_obj.label_2, minutes_src_3_obj.img_2, LV_ALIGN_TOP_MID, 0, -35);               //文字
    lv_obj_align_to(minutes_src_3_obj.line_2, minutes_src_3_obj.img_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

    //外出节能
    lv_obj_align_to(minutes_src_3_obj.img_3, minutes_src_3_obj.line_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_align_to(minutes_src_3_obj.label_3, minutes_src_3_obj.img_3, LV_ALIGN_TOP_MID, 0, -25);
    lv_obj_align_to(minutes_src_3_obj.line_3, minutes_src_3_obj.img_3, LV_ALIGN_OUT_BOTTOM_MID, 0, 25);

    //大量存储
    lv_obj_align_to(minutes_src_3_obj.img_4, minutes_src_3_obj.line_3, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_align_to(minutes_src_3_obj.label_4, minutes_src_3_obj.img_4, LV_ALIGN_TOP_MID, 0, -20);
    lv_obj_align_to(minutes_src_3_obj.line_4, minutes_src_3_obj.img_4, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    //智能存储
    lv_obj_align_to(minutes_src_3_obj.img_5, minutes_src_3_obj.line_4, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_obj_align_to(minutes_src_3_obj.label_5, minutes_src_3_obj.img_5, LV_ALIGN_TOP_MID, 0, -15);
    lv_obj_align_to(minutes_src_3_obj.line_5, minutes_src_3_obj.img_5, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    //退出
    lv_obj_align_to(minutes_src_3_obj.btn_6, minutes_src_3_obj.line_5, LV_ALIGN_OUT_BOTTOM_MID, 0, 90);
    lv_obj_align(minutes_src_3_obj.img_6,  LV_ALIGN_CENTER, 0, 0);

}

static void __user_update_layout_obj(void)              //更新对象
{
    lv_img_set_src(minutes_src_3_obj.img_2, MID_SRC_3_IMG_2);
    lv_img_set_src(minutes_src_3_obj.img_3, MID_SRC_3_IMG_3);
    lv_img_set_src(minutes_src_3_obj.img_4, MID_SRC_3_IMG_4);
    lv_img_set_src(minutes_src_3_obj.img_5, MID_SRC_3_IMG_5);

    lv_obj_remove_style_all(minutes_src_3_obj.label_2);
    lv_obj_set_style_text_font(minutes_src_3_obj.label_2, &my_font_30, 0);             //25号字体
    lv_obj_set_style_text_color(minutes_src_3_obj.label_2, lv_color_make(164, 163, 163), 0);
    lv_label_set_text(minutes_src_3_obj.label_2, "日常运行");

    lv_obj_remove_style_all(minutes_src_3_obj.label_3);
    lv_obj_set_style_text_font(minutes_src_3_obj.label_3, &my_font_30, 0);             //25号字体
    lv_obj_set_style_text_color(minutes_src_3_obj.label_3, lv_color_make(164, 163, 163), 0);
    lv_label_set_text(minutes_src_3_obj.label_3, "外出节能");

    lv_obj_remove_style_all(minutes_src_3_obj.label_4);
    lv_obj_set_style_text_font(minutes_src_3_obj.label_4, &my_font_30, 0);             //25号字体
    lv_obj_set_style_text_color(minutes_src_3_obj.label_4, lv_color_make(164, 163, 163), 0);
    lv_label_set_text(minutes_src_3_obj.label_4, "大量存储");

    lv_obj_remove_style_all(minutes_src_3_obj.label_5);
    lv_obj_set_style_text_font(minutes_src_3_obj.label_5, &my_font_30, 0);             //25号字体
    lv_obj_set_style_text_color(minutes_src_3_obj.label_5, lv_color_make(164, 163, 163), 0);
    lv_label_set_text(minutes_src_3_obj.label_5, "智能储存");

    if(cur_scene_mode == SCENE_MODE1)
    {
        lv_img_set_src(minutes_src_3_obj.img_2, SCENE_IMG1);        //MID_SRC_3_IMG_6
        lv_obj_remove_style_all(minutes_src_3_obj.label_2);
        lv_obj_set_style_text_font(minutes_src_3_obj.label_2, &my_font_40, 0);             //25号字体
        lv_obj_set_style_text_color(minutes_src_3_obj.label_2, lv_color_make(226, 184, 131), 0);
        lv_label_set_text(minutes_src_3_obj.label_2, "· 日常运行 ·");
    }else if(cur_scene_mode == SCENE_MODE2)
    {
        lv_img_set_src(minutes_src_3_obj.img_3, SCENE_IMG2);    //MID_SRC_3_IMG_7
        lv_obj_remove_style_all(minutes_src_3_obj.label_3);
        lv_obj_set_style_text_font(minutes_src_3_obj.label_3, &my_font_40, 0);             //25号字体
        lv_obj_set_style_text_color(minutes_src_3_obj.label_3, lv_color_make(226, 184, 131), 0);
        lv_label_set_text(minutes_src_3_obj.label_3, "· 外出节能 ·");
    }else if(cur_scene_mode == SCENE_MODE3)
    {
        lv_img_set_src(minutes_src_3_obj.img_4, SCENE_IMG3);        //MID_SRC_3_IMG_8
        lv_obj_remove_style_all(minutes_src_3_obj.label_4);
        lv_obj_set_style_text_font(minutes_src_3_obj.label_4, &my_font_40, 0);             //25号字体
        lv_obj_set_style_text_color(minutes_src_3_obj.label_4, lv_color_make(226, 184, 131), 0);
        lv_label_set_text(minutes_src_3_obj.label_4, "· 大量存储 ·");
    }else if(cur_scene_mode == SCENE_MODE4)
    {
        lv_img_set_src(minutes_src_3_obj.img_5, SCENE_IMG4);        //MID_SRC_3_IMG_9
        lv_obj_remove_style_all(minutes_src_3_obj.label_5);
        lv_obj_set_style_text_font(minutes_src_3_obj.label_5, &my_font_40, 0);             //25号字体
        lv_obj_set_style_text_color(minutes_src_3_obj.label_5, lv_color_make(226, 184, 131), 0);
        lv_label_set_text(minutes_src_3_obj.label_5, "· 智能储存 ·");
    }
}

static void minutes_src_2_img_2_cb(lv_event_t * e)  //"日常运行"回调
{
    cur_scene_mode = SCENE_MODE1;

    __user_update_layout_obj();
    __user_update_layout_pos();
}
static void minutes_src_2_img_3_cb(lv_event_t * e)  //"外出节能"回调
{
    cur_scene_mode = SCENE_MODE2;

    __user_update_layout_obj();
    __user_update_layout_pos();
}
static void minutes_src_2_img_4_cb(lv_event_t * e)  //"大量存储"回调
{
    cur_scene_mode = SCENE_MODE3;

    __user_update_layout_obj();
    __user_update_layout_pos();
}
static void minutes_src_2_img_5_cb(lv_event_t * e)  //"智能储存"回调
{
    cur_scene_mode = SCENE_MODE4;

    __user_update_layout_obj();
    __user_update_layout_pos();
}


static void minutes_src_3_btn_6_cb(lv_event_t * e)  //退出回调
{
    lv_obj_clean(user_src_obj.page_minutes_src_3);
    __user_update_layout_page_minutes_obj();
    __user_update_layout_page_minutes_pos();
    lv_scr_load(user_src_obj.maininterface_src);
    lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_middle, LV_ANIM_OFF);

}
