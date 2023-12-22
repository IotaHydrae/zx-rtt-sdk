#include "haier_test.h"


extern haier_test_obj haier_obj;
extern global_data user_data;
extern haier_src user_src_obj;       //屏幕对象

haier_page_minutes_obj page_minutes_obj = {0};
CUR_CONTROL_MODE cur_control_mode = CONTROL_MODE1;      //控制模式 冷冻/冷藏
CUR_DRAWER_MODE cur_drawer_mode = DRAWER_MODE3;         //变温抽屉  珍品 母婴 0°C 冷藏
CUR_SCENE_MODE cur_scene_mode = SCENE_MODE1;            //日常运行 外出节能 大量储存 智能储存


static void lv_control_img_1_cb(lv_event_t * e);        //冰箱控制图片回调
static void lv_drawer_img_1_cb(lv_event_t * e);        //变温抽屉图片回调
static void lv_scene_img_1_cb(lv_event_t * e);        //场景模式图片回调


void __user_update_layout_page_minutes_pos(void);              //更新对象
void __user_update_layout_page_minutes_obj(void); 

static void lv_user_obj_set_x(void *obj, int32_t x)
{
	lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)x);
    lv_obj_align_to(page_minutes_obj.control_label_1, page_minutes_obj.control_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_align_to(page_minutes_obj.control_img_2, page_minutes_obj.control_img_1, LV_ALIGN_OUT_RIGHT_MID, 240, 0);
    lv_obj_align_to(page_minutes_obj.control_label_2, page_minutes_obj.control_img_2, LV_ALIGN_OUT_TOP_MID, 0, -10);
}
//中屏
void haier_page_minutes(void)
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

    memset(&page_minutes_obj, 0, sizeof(page_minutes_obj));

    page_minutes_obj.control_obj_1 = lv_obj_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.control_obj_1);
    lv_obj_set_size(page_minutes_obj.control_obj_1, 400, 300);
    lv_obj_center(page_minutes_obj.control_obj_1);
    lv_obj_set_scrollbar_mode(page_minutes_obj.control_obj_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(page_minutes_obj.control_obj_1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(page_minutes_obj.control_obj_1, true, 0); //儿子超出部分隐藏   
    lv_obj_add_flag(page_minutes_obj.control_obj_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(page_minutes_obj.control_obj_1, lv_control_img_1_cb, LV_EVENT_CLICKED, NULL);   //添加事件
    
    page_minutes_obj.control_img_1 = lv_img_create(page_minutes_obj.control_obj_1);
    lv_img_set_src(page_minutes_obj.control_img_1, CONTROL_IMG1);
    page_minutes_obj.control_img_2 = lv_img_create(page_minutes_obj.control_obj_1);
    lv_img_set_src(page_minutes_obj.control_img_2, CONTROL_IMG2);

    lv_obj_align(page_minutes_obj.control_img_1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(page_minutes_obj.control_img_2, page_minutes_obj.control_img_1, LV_ALIGN_OUT_RIGHT_MID, 240, 0);

    page_minutes_obj.control_label_1 = lv_label_create(page_minutes_obj.control_obj_1);
    lv_obj_remove_style_all(page_minutes_obj.control_label_1);
    lv_obj_set_style_text_color(page_minutes_obj.control_label_1, lv_color_make(228, 184, 128), 0);
    lv_obj_set_style_text_font(page_minutes_obj.control_label_1, &my_font_40, 0);
    lv_label_set_text_fmt(page_minutes_obj.control_label_1, "冷藏%d℃", user_data.cur_temperature);

    
    page_minutes_obj.control_label_2 = lv_label_create(page_minutes_obj.control_obj_1);
    lv_obj_remove_style_all(page_minutes_obj.control_label_2);
    lv_obj_set_style_text_color(page_minutes_obj.control_label_2, lv_color_make(228, 184, 128), 0);
    lv_obj_set_style_text_font(page_minutes_obj.control_label_2, &my_font_40, 0);
    lv_label_set_text_fmt(page_minutes_obj.control_label_2, "冷冻-18℃", user_data.cur_temperature);

    lv_anim_t anim = {0};//创建动画对象anim
    lv_anim_init(&anim);//初始化anim(必须)
    anim.var = page_minutes_obj.control_img_1;//选择动画控制的对象
    anim.exec_cb = lv_user_obj_set_x;//设置动画执行的动作为在y轴运动（横移：lv_obj_set_x）
    anim.start_value = 0;//设置动画的初始值
    anim.end_value = -480;//设置动画的结束值
    anim.path_cb = lv_anim_path_linear;//设置动画效果（详见动画解析）
    anim.time = 2000;//动画所需总时间，单位：ms
    anim.act_time = 0;//设置起始时间点，动画已经执行的时间，单位：ms（动画从0时刻开始运行，负数会相当于动画前等待，可以读取）
    anim.repeat_cnt = LV_ANIM_REPEAT_INFINITE;//设置动画执行次数
    anim.playback_delay = 500;
    anim.playback_time = 2000;
    anim.repeat_delay = 500;
    lv_anim_start(&anim);//启动动画


    page_minutes_obj.control_label_3 = lv_label_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.control_label_3);
    lv_obj_set_style_text_color(page_minutes_obj.control_label_3, lv_color_make(211, 211, 211), 0);
    lv_obj_set_style_text_font(page_minutes_obj.control_label_3, &my_font_30, 0);
    lv_label_set_text(page_minutes_obj.control_label_3, "冰箱控制");

    page_minutes_obj.control_img_3 = lv_img_create(haier_obj.page_middle);
    lv_img_set_src(page_minutes_obj.control_img_3, CONTROL_IMG3);

    // lv_line_create
    page_minutes_obj.control_line_1 = lv_line_create(haier_obj.page_middle);
    lv_line_set_points(page_minutes_obj.control_line_1, line_points, 2);
    lv_obj_add_style(page_minutes_obj.control_line_1, &style_line, LV_PART_MAIN);

    

    page_minutes_obj.drawer_img_1 = lv_img_create(haier_obj.page_middle);
    lv_obj_add_flag(page_minutes_obj.drawer_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(page_minutes_obj.drawer_img_1, lv_drawer_img_1_cb, LV_EVENT_CLICKED, NULL);   //添加事件


    page_minutes_obj.drawer_label_1 = lv_label_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.drawer_label_1);
    lv_obj_set_style_text_color(page_minutes_obj.drawer_label_1, lv_color_make(228, 184, 128), 0);
    lv_obj_set_style_text_font(page_minutes_obj.drawer_label_1, &my_font_40, 0);

    page_minutes_obj.drawer_label_2 = lv_label_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.drawer_label_2);
    lv_obj_set_style_text_color(page_minutes_obj.drawer_label_2, lv_color_make(211, 211, 211), 0);
    lv_obj_set_style_text_font(page_minutes_obj.drawer_label_2, &my_font_30, 0);
    lv_label_set_text(page_minutes_obj.drawer_label_2, "变温抽屉");

    page_minutes_obj.drawer_img_2 = lv_img_create(haier_obj.page_middle);
    lv_img_set_src(page_minutes_obj.drawer_img_2, DRAWER_IMG5);

    page_minutes_obj.drawer_line_1 = lv_line_create(haier_obj.page_middle);
    lv_line_set_points(page_minutes_obj.drawer_line_1, line_points, 2);
    lv_obj_add_style(page_minutes_obj.drawer_line_1, &style_line, LV_PART_MAIN);




    page_minutes_obj.scene_img_1 = lv_img_create(haier_obj.page_middle);
    lv_obj_add_flag(page_minutes_obj.scene_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(page_minutes_obj.scene_img_1, lv_scene_img_1_cb, LV_EVENT_CLICKED, NULL);   //添加事件

    page_minutes_obj.scene_label_1 = lv_label_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.scene_label_1);
    lv_obj_set_style_text_color(page_minutes_obj.scene_label_1, lv_color_make(228, 184, 128), 0);
    lv_obj_set_style_text_font(page_minutes_obj.scene_label_1, &my_font_40, 0);


    page_minutes_obj.scene_label_2 = lv_label_create(haier_obj.page_middle);
    lv_obj_remove_style_all(page_minutes_obj.scene_label_2);
    lv_obj_set_style_text_color(page_minutes_obj.scene_label_2, lv_color_make(211, 211, 211), 0);
    lv_obj_set_style_text_font(page_minutes_obj.scene_label_2, &my_font_30, 0);
    lv_label_set_text(page_minutes_obj.scene_label_2, "场景模式");

    page_minutes_obj.scene_img_2 = lv_img_create(haier_obj.page_middle);
    lv_img_set_src(page_minutes_obj.scene_img_2, SCENE_IMG5);

    page_minutes_obj.scene_line_1 = lv_line_create(haier_obj.page_middle);
    lv_line_set_points(page_minutes_obj.scene_line_1, line_points, 2);
    lv_obj_add_style(page_minutes_obj.scene_line_1, &style_line, LV_PART_MAIN);

    __user_update_layout_page_minutes_obj();
    __user_update_layout_page_minutes_pos();
}


void __user_update_layout_page_minutes_pos(void)
{
    lv_obj_align(page_minutes_obj.control_obj_1, LV_ALIGN_TOP_MID, 0, 400);
    lv_obj_align_to(page_minutes_obj.control_label_1, page_minutes_obj.control_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_align_to(page_minutes_obj.control_label_2, page_minutes_obj.control_img_2, LV_ALIGN_OUT_TOP_MID, 0, -10);

    lv_obj_align_to(page_minutes_obj.control_label_3, page_minutes_obj.control_obj_1, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_obj_align_to(page_minutes_obj.control_img_3, page_minutes_obj.control_label_3, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_align_to(page_minutes_obj.control_line_1, page_minutes_obj.control_obj_1,LV_ALIGN_OUT_BOTTOM_MID, 0, 50);



    lv_obj_align_to(page_minutes_obj.drawer_img_1, page_minutes_obj.control_line_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 165);
    lv_obj_align_to(page_minutes_obj.drawer_label_1, page_minutes_obj.drawer_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_align_to(page_minutes_obj.drawer_label_2, page_minutes_obj.drawer_label_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_align_to(page_minutes_obj.drawer_img_2, page_minutes_obj.drawer_label_2, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_align_to(page_minutes_obj.drawer_line_1, page_minutes_obj.drawer_img_1,LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    lv_obj_align_to(page_minutes_obj.scene_img_1, page_minutes_obj.drawer_line_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 165);
    lv_obj_align_to(page_minutes_obj.scene_label_1, page_minutes_obj.scene_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_obj_align_to(page_minutes_obj.scene_label_2, page_minutes_obj.scene_label_1, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_obj_align_to(page_minutes_obj.scene_img_2, page_minutes_obj.scene_label_2, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_align_to(page_minutes_obj.scene_line_1, page_minutes_obj.scene_img_1,LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
}
void __user_update_layout_page_minutes_obj(void)              //更新对象
{
    if(cur_control_mode == CONTROL_MODE1)
    {
        lv_img_set_src(page_minutes_obj.control_img_1, CONTROL_IMG2);
        lv_label_set_text_fmt(page_minutes_obj.control_label_1, "冷藏%d℃", user_data.cur_temperature);
    }
    else if(cur_control_mode == CONTROL_MODE2)
    {
        lv_img_set_src(page_minutes_obj.control_img_1, CONTROL_IMG1);
        lv_label_set_text_fmt(page_minutes_obj.control_label_1, "冷冻-%d℃", abs(user_data.cur_temperature)); 
    }

    if(cur_drawer_mode == DRAWER_MODE1)
    {
        lv_img_set_src(page_minutes_obj.drawer_img_1, DRAWER_IMG1);
        lv_label_set_text(page_minutes_obj.drawer_label_1, "珍品");
    }
    else if(cur_drawer_mode == DRAWER_MODE2)
    {
        lv_img_set_src(page_minutes_obj.drawer_img_1, DRAWER_IMG2);
        lv_label_set_text(page_minutes_obj.drawer_label_1, "母婴");

    }
    else if(cur_drawer_mode == DRAWER_MODE3)
    {
        lv_img_set_src(page_minutes_obj.drawer_img_1, DRAWER_IMG3);
        lv_label_set_text(page_minutes_obj.drawer_label_1, "0℃保鲜");

    }
    else if(cur_drawer_mode == DRAWER_MODE4)
    {
        lv_img_set_src(page_minutes_obj.drawer_img_1, DRAWER_IMG4);
        lv_label_set_text(page_minutes_obj.drawer_label_1, "冷藏");

    }

    if(cur_scene_mode == SCENE_MODE1)
    {
        lv_img_set_src(page_minutes_obj.scene_img_1, SCENE_IMG1);
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "日常运行", user_data.cur_temperature);
    }
    else if(cur_scene_mode == SCENE_MODE2)
    {
        lv_img_set_src(page_minutes_obj.scene_img_1, SCENE_IMG2);
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "外出节能", user_data.cur_temperature);
    }
    else if(cur_scene_mode == SCENE_MODE3)
    {
        lv_img_set_src(page_minutes_obj.scene_img_1, SCENE_IMG3);
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "大量储存", user_data.cur_temperature);
    }
    else if(cur_scene_mode == SCENE_MODE4)
    {
        lv_img_set_src(page_minutes_obj.scene_img_1, SCENE_IMG4);
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "智能储存", user_data.cur_temperature);
    }
}


void user_set_control_mode(CUR_CONTROL_MODE mode, char *src)
{
    if(src)
    {
        lv_img_set_src(page_minutes_obj.control_img_1, src);
        lv_obj_align(page_minutes_obj.control_img_1, LV_ALIGN_TOP_MID, 0, 280);
    }
    if(mode == CONTROL_MODE1)
        lv_label_set_text_fmt(page_minutes_obj.control_label_1, "冷藏%d℃", user_data.cur_temperature);
    else if(mode == CONTROL_MODE2)
        lv_label_set_text_fmt(page_minutes_obj.control_label_1, "冷冻-%d℃", abs(user_data.cur_temperature));
    lv_obj_align_to(page_minutes_obj.control_label_1, page_minutes_obj.control_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

void user_set_drawer_mode(CUR_DRAWER_MODE mode, char *src)
{
    if(src)
    {
        lv_img_set_src(page_minutes_obj.drawer_img_1, src);
        lv_obj_align_to(page_minutes_obj.drawer_img_1, page_minutes_obj.control_img_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 230);
    }
    if(mode == DRAWER_MODE1)
        lv_label_set_text(page_minutes_obj.drawer_label_1, "珍品");
    else if(mode == DRAWER_MODE2)
        lv_label_set_text(page_minutes_obj.drawer_label_1, "母婴");
    else if(mode == DRAWER_MODE3)
        lv_label_set_text_fmt(page_minutes_obj.drawer_label_1, "%d℃保鲜", user_data.cur_temperature);
    else if(mode == DRAWER_MODE4)
        lv_label_set_text(page_minutes_obj.drawer_label_1, "冷藏"); 
    lv_obj_align_to(page_minutes_obj.drawer_label_1, page_minutes_obj.drawer_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

void user_set_scene_mode(CUR_SCENE_MODE mode, char *src)
{
    if(src)
    {
        lv_img_set_src(page_minutes_obj.scene_img_1, src);
        lv_obj_align_to(page_minutes_obj.scene_img_1, page_minutes_obj.drawer_img_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 230);
    }
    if(cur_scene_mode == SCENE_MODE1)
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "日常运行", user_data.cur_temperature);
    else if(cur_scene_mode == SCENE_MODE2)
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "外出节能", user_data.cur_temperature);
    else if(cur_scene_mode == SCENE_MODE3)
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "大量储存", user_data.cur_temperature);
    else if(cur_scene_mode == SCENE_MODE4)
        lv_label_set_text_fmt(page_minutes_obj.scene_label_1, "智能储存", user_data.cur_temperature);
    lv_obj_align_to(page_minutes_obj.scene_label_1, page_minutes_obj.scene_img_1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

static void lv_control_img_1_cb(lv_event_t * e)
{
    lv_scr_load(user_src_obj.page_minutes_src_1);
    page_minutes_src_1_menu();
}

static void lv_drawer_img_1_cb(lv_event_t * e)        //变温抽屉图片回调
{
    lv_scr_load(user_src_obj.page_minutes_src_2);
    page_minutes_src_2_menu();
}

static void lv_scene_img_1_cb(lv_event_t * e)        //场景模式图片回调
{
    lv_scr_load(user_src_obj.page_minutes_src_3);
    page_minutes_src_3_menu();
}
