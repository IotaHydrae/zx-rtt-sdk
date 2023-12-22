#include "haier_test.h"

extern void haier_page_time(void);


void user_src_init(void);           //屏幕初始化


haier_test_obj haier_obj = {0};     //页面对象
haier_src user_src_obj = {0};       //屏幕对象
global_data user_data = {0};        //用户数据
top_src_obj user_top_src_obj = {0}; //顶层对象
user_time_data user_cur_time = {10, 30, 30, 9, 25, 5};      //当前日期时间

extern void backlight_init();


static void top_src_obj_timer_cb(lv_timer_t * timer)
{
	lv_obj_add_flag(user_top_src_obj.obj_temp_1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(user_top_src_obj.label_temp_1, LV_OBJ_FLAG_HIDDEN);
	lv_timer_pause(user_top_src_obj.timer);
}
static void top_src_obj_timer2_cb(lv_timer_t * timer)
{
   static bool flag = false;
	if(flag)
	{
		flag = false;
		lv_label_set_text_fmt(user_top_src_obj.label_time, "%d %d", user_cur_time.cur_hour, user_cur_time.cur_minute);
	}
	else
	{
		flag = true;
		lv_label_set_text_fmt(user_top_src_obj.label_time, "%d:%d", user_cur_time.cur_hour, user_cur_time.cur_minute);
	}
}

// static void timer_static_cb(lv_timer_t * timer)
// {
// 	static int cnt = 0;
// 	switch (cnt)
// 	{
// 	case 0:
// 		lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_middle, LV_ANIM_OFF);
// 		cnt++;
// 		break;
// 	case 1:
// 		lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_right, LV_ANIM_OFF);
// 		cnt++;
// 		break;
// 	case 2:
// 		lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_middle, LV_ANIM_OFF);
// 		cnt = 0;
// 		break;
// 	default:
// 		break;
// 	}
// 	printf("111\n");
// }
void haier_test(void)
{
	user_src_init();        //屏幕初始化


	memset(&user_data, 0, sizeof(user_data));
	memset(&haier_obj, 0, sizeof(haier_obj));

	user_top_src_obj.label_time = lv_label_create(lv_layer_top());
	lv_obj_set_style_text_color(user_top_src_obj.label_time, lv_color_white(), 0);  //设置字体颜色
	lv_obj_set_style_text_font(user_top_src_obj.label_time, &my_font_25, 0);
	lv_label_set_text_fmt(user_top_src_obj.label_time, "%d:%d", user_cur_time.cur_hour, user_cur_time.cur_minute);
	lv_obj_align(user_top_src_obj.label_time, LV_ALIGN_TOP_MID, 0, 130);
	user_top_src_obj.img_wifi = lv_img_create(lv_layer_top());
	lv_img_set_src(user_top_src_obj.img_wifi, WIFI_ON);
	lv_obj_align(user_top_src_obj.img_wifi, LV_ALIGN_TOP_RIGHT, -50, 130);

	user_top_src_obj.img_left_bottom = lv_img_create(lv_layer_top());
	user_top_src_obj.img_middle_bottom = lv_img_create(lv_layer_top());
	user_top_src_obj.img_right_bottom = lv_img_create(lv_layer_top());


	user_top_src_obj.obj_temp_1 = lv_obj_create(lv_layer_top());
	user_top_src_obj.label_temp_1 = lv_label_create(user_top_src_obj.obj_temp_1);
	lv_obj_remove_style_all(user_top_src_obj.obj_temp_1);                    //移除所有样式
	lv_obj_remove_style_all(user_top_src_obj.label_temp_1);                    //移除所有样式
	lv_obj_set_style_text_font(user_top_src_obj.label_temp_1, &my_font_25, 0);
	lv_obj_add_flag(user_top_src_obj.obj_temp_1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(user_top_src_obj.label_temp_1, LV_OBJ_FLAG_HIDDEN);
	if(user_top_src_obj.timer == NULL)
	{
		user_top_src_obj.timer = lv_timer_create(top_src_obj_timer_cb, 2000, NULL);
		lv_timer_pause(user_top_src_obj.timer);
	}
	if(user_top_src_obj.timer_2 == NULL)
	{
		user_top_src_obj.timer_2 = lv_timer_create(top_src_obj_timer2_cb, 500, NULL);
	}

	lv_scr_load(user_src_obj.maininterface_src);                               //加载主屏幕

	static lv_style_t style;                            //创建样式变量
	lv_style_init(&style);                              //初始化样式
	lv_style_set_bg_color(&style, lv_color_black());    //设置背景颜色
	lv_style_set_text_color(&style, lv_color_white());  //设置字体颜色
	lv_style_set_border_width(&style, 0);               //设置边框宽度
	lv_style_set_pad_all(&style, 0);                    //设置边距
	lv_obj_add_style(user_src_obj.maininterface_src, &style, 0);          //添加样式

	haier_obj.haier_tv = lv_tileview_create(user_src_obj.maininterface_src);
	lv_obj_add_style(haier_obj.haier_tv, &style, 0);          //添加样式
	lv_obj_set_style_text_font(haier_obj.haier_tv, &my_font_25, 0);

	/*时间界面*/
	haier_obj.page_time = lv_tileview_add_tile(haier_obj.haier_tv, 1, 0, LV_DIR_VER);
	haier_page_time();
	
	/*左屏*/
	haier_obj.page_left = lv_tileview_add_tile(haier_obj.haier_tv, 0, 1, LV_DIR_HOR);
	haier_page_left();
	
	/*中屏*/
	haier_obj.page_middle = lv_tileview_add_tile(haier_obj.haier_tv, 1, 1, LV_DIR_ALL);	
	haier_page_minutes();
	

	/*右屏*/
	haier_obj.page_right = lv_tileview_add_tile(haier_obj.haier_tv, 2, 1, LV_DIR_HOR);
	haier_page_right();
	
	lv_obj_set_scrollbar_mode(haier_obj.page_time, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(haier_obj.page_left, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(haier_obj.page_middle, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(haier_obj.page_right, LV_SCROLLBAR_MODE_OFF);

	
	//初始化切换到时间界面
	lv_obj_set_tile(haier_obj.haier_tv, haier_obj.page_time, LV_ANIM_OFF);
	lv_obj_set_scrollbar_mode(haier_obj.haier_tv, LV_SCROLLBAR_MODE_OFF); 

	// static lv_timer_t *timer_static = NULL;                    //定时器
	// if(timer_static == NULL)
	// {
	// 	timer_static = lv_timer_create(timer_static_cb, 500, NULL);
	// }
}

void user_src_init(void)
{
	user_src_obj.maininterface_src = lv_obj_create(NULL);                       //创建新屏幕
    user_src_obj.page_minutes_src_1 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_minutes_src_2 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_minutes_src_3 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_minutes_src_4 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_right_src_1 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_right_src_2 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_right_src_3 = lv_obj_create(NULL);                      //创建新屏幕
    user_src_obj.page_left_src_1 = lv_obj_create(NULL);                      //创建新屏幕

    lv_obj_remove_style_all(user_src_obj.maininterface_src);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.maininterface_src, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.maininterface_src, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.maininterface_src, 460, 1920);
    lv_obj_center(user_src_obj.maininterface_src);
    lv_obj_set_scrollbar_mode(user_src_obj.maininterface_src, LV_SCROLLBAR_MODE_OFF);


    lv_obj_remove_style_all(user_src_obj.page_minutes_src_1);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_minutes_src_1, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_minutes_src_1, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_minutes_src_1, 460, 1920);
    lv_obj_center(user_src_obj.page_minutes_src_1);
    lv_obj_set_scrollbar_mode(user_src_obj.page_minutes_src_1, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_minutes_src_2);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_minutes_src_2, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_minutes_src_2, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_minutes_src_2, 460, 1920);
    lv_obj_center(user_src_obj.page_minutes_src_2);
    lv_obj_set_scrollbar_mode(user_src_obj.page_minutes_src_2, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_minutes_src_3);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_minutes_src_3, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_minutes_src_3, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_minutes_src_3, 460, 1920);
    lv_obj_center(user_src_obj.page_minutes_src_3);
    lv_obj_set_scrollbar_mode(user_src_obj.page_minutes_src_3, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_minutes_src_4);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_minutes_src_4, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_minutes_src_4, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_minutes_src_4, 460, 1920);
    lv_obj_center(user_src_obj.page_minutes_src_4);
    lv_obj_set_scrollbar_mode(user_src_obj.page_minutes_src_4, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_right_src_1);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_right_src_1, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_right_src_1, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_right_src_1, 460, 1920);
    lv_obj_center(user_src_obj.page_right_src_1);
    lv_obj_set_scrollbar_mode(user_src_obj.page_right_src_1, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_right_src_2);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_right_src_2, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_right_src_2, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_right_src_2, 460, 1920);
    lv_obj_center(user_src_obj.page_right_src_2);
    lv_obj_set_scrollbar_mode(user_src_obj.page_right_src_2, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_right_src_3);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_right_src_3, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_right_src_3, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_right_src_3, 460, 1920);
    lv_obj_center(user_src_obj.page_right_src_3);
    lv_obj_set_scrollbar_mode(user_src_obj.page_right_src_3, LV_SCROLLBAR_MODE_OFF);

    lv_obj_remove_style_all(user_src_obj.page_left_src_1);                    //移除所有样式
    lv_obj_set_style_bg_opa(user_src_obj.page_left_src_1, LV_OPA_COVER, 0);   //0不透明
    lv_obj_set_style_bg_color(user_src_obj.page_left_src_1, lv_color_black(), 0);
    lv_obj_set_size(user_src_obj.page_left_src_1, 460, 1920);
    lv_obj_center(user_src_obj.page_left_src_1);
    lv_obj_set_scrollbar_mode(user_src_obj.page_left_src_1, LV_SCROLLBAR_MODE_OFF);
	// backlight_init();

}

