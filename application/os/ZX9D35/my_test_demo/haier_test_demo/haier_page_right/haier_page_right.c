#include "haier_test.h"


extern haier_test_obj haier_obj;
extern global_data user_data;

haier_page_right_obj page_right_obj = {0};


void __user_update_layout_page_right_pos(void);              //更新对象
void __user_update_layout_page_right_obj(void); 


void haier_page_right(void)
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

    memset(&page_right_obj, 0, sizeof(page_right_obj));

    page_right_obj.timer_img_1 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.timer_img_1, TIMER_IMG2);
    lv_obj_set_style_border_color(page_right_obj.timer_img_1, lv_color_black(), 0);
    lv_obj_set_style_border_width(page_right_obj.timer_img_1, 5, 0);
    lv_obj_align(page_right_obj.timer_img_1, LV_ALIGN_TOP_MID, 0, 325);


    page_right_obj.timer_label_1 = lv_label_create(haier_obj.page_right);
    lv_obj_remove_style_all(page_right_obj.timer_label_1);
    lv_obj_set_style_text_color(page_right_obj.timer_label_1, lv_color_make(192, 189, 182), 0);
    lv_obj_set_style_text_font(page_right_obj.timer_label_1, &my_font_30, 0);
    lv_label_set_text(page_right_obj.timer_label_1, "计时器");
    lv_obj_align_to(page_right_obj.timer_label_1, page_right_obj.timer_img_1, LV_ALIGN_OUT_TOP_MID, 0, 0);

    page_right_obj.timer_img_2 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.timer_img_2, TIMER_IMG1);
    lv_obj_align_to(page_right_obj.timer_img_2, page_right_obj.timer_label_1, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    page_right_obj.timer_line_1 = lv_line_create(haier_obj.page_right);
    lv_line_set_points(page_right_obj.timer_line_1, line_points, 2);
    lv_obj_add_style(page_right_obj.timer_line_1, &style_line, LV_PART_MAIN);
    lv_obj_align_to(page_right_obj.timer_line_1, page_right_obj.timer_img_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);





    page_right_obj.phone_img_1 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.phone_img_1, PHONE_IMG2);
    lv_obj_align_to(page_right_obj.phone_img_1, page_right_obj.timer_line_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 150);
    lv_obj_set_style_border_color(page_right_obj.phone_img_1, lv_color_black(), 0);
    lv_obj_set_style_border_width(page_right_obj.phone_img_1, 5, 0);

    page_right_obj.phone_label_1 = lv_label_create(haier_obj.page_right);
    lv_obj_remove_style_all(page_right_obj.phone_label_1);
    lv_obj_set_style_text_color(page_right_obj.phone_label_1, lv_color_make(192, 189, 182), 0);
    lv_obj_set_style_text_font(page_right_obj.phone_label_1, &my_font_30, 0);
    lv_label_set_text(page_right_obj.phone_label_1, "绑定手机");
    lv_obj_align_to(page_right_obj.phone_label_1, page_right_obj.phone_img_1, LV_ALIGN_OUT_TOP_MID, 0, -50);

    page_right_obj.phone_img_2 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.phone_img_2, PHONE_IMG1);
    lv_obj_align_to(page_right_obj.phone_img_2, page_right_obj.phone_label_1, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    page_right_obj.phone_line_1 = lv_line_create(haier_obj.page_right);
    lv_line_set_points(page_right_obj.phone_line_1, line_points, 2);
    lv_obj_add_style(page_right_obj.phone_line_1, &style_line, LV_PART_MAIN);
    lv_obj_align_to(page_right_obj.phone_line_1, page_right_obj.phone_img_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);



    page_right_obj.setup_img_1 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.setup_img_1, SET_UP_IMG2);
    lv_obj_align_to(page_right_obj.setup_img_1, page_right_obj.phone_line_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 120);
    lv_obj_set_style_border_color(page_right_obj.setup_img_1, lv_color_black(), 0);
    lv_obj_set_style_border_width(page_right_obj.setup_img_1, 5, 0);

    page_right_obj.setup_label_1 = lv_label_create(haier_obj.page_right);
    lv_obj_remove_style_all(page_right_obj.setup_label_1);
    lv_obj_set_style_text_color(page_right_obj.setup_label_1, lv_color_make(192, 189, 182), 0);
    lv_obj_set_style_text_font(page_right_obj.setup_label_1, &my_font_30, 0);
    lv_label_set_text(page_right_obj.setup_label_1, "设置中心");
    lv_obj_align_to(page_right_obj.setup_label_1, page_right_obj.setup_img_1, LV_ALIGN_OUT_TOP_MID, 0, -20);

    page_right_obj.setup_img_2 = lv_img_create(haier_obj.page_right);
    lv_img_set_src(page_right_obj.setup_img_2, SET_UP_IMG1);
    lv_obj_align_to(page_right_obj.setup_img_2, page_right_obj.setup_label_1, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    page_right_obj.setup_line_1 = lv_line_create(haier_obj.page_right);
    lv_line_set_points(page_right_obj.setup_line_1, line_points, 2);
    lv_obj_add_style(page_right_obj.setup_line_1, &style_line, LV_PART_MAIN);
    lv_obj_align_to(page_right_obj.setup_line_1, page_right_obj.setup_img_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
}

void __user_update_layout_page_right_pos(void)              //更新对象
{

}
void __user_update_layout_page_right_obj(void)
{

}


