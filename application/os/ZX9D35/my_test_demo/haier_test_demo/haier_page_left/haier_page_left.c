#include "haier_test.h"

static void btn1_event_cb(lv_event_t * e);
static void btn2_event_cb(lv_event_t * e);

extern haier_test_obj haier_obj;
extern global_data user_data;

haier_page_left_obj page_left_obj = {0};


void haier_page_left(void)
{
    memset(&page_left_obj, 0, sizeof(page_left_obj));

    page_left_obj.label1 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label1);
    lv_obj_set_style_text_color(page_left_obj.label1, lv_color_make(211, 211, 211), 0);
    lv_obj_set_style_text_font(page_left_obj.label1, &my_font_30, 0);
    lv_label_set_text(page_left_obj.label1, "专属空间");
    lv_obj_align(page_left_obj.label1, LV_ALIGN_TOP_MID, 5, 300);

    page_left_obj.img1 = lv_img_create(haier_obj.page_left);
    lv_img_set_src(page_left_obj.img1, LEFT_IMG1);
    lv_obj_align(page_left_obj.img1, LV_ALIGN_CENTER, 0, 60);
    lv_obj_align_to(page_left_obj.img1, page_left_obj.label1, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    page_left_obj.label2 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label2);
    lv_obj_set_style_text_color(page_left_obj.label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label2, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label2, "适宜存放");
    lv_obj_align(page_left_obj.label2, LV_ALIGN_TOP_LEFT, 60, 400);

    //母乳
    page_left_obj.btn1 = lv_btn_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.btn1);
    lv_obj_set_size(page_left_obj.btn1, 190, 190);
    lv_obj_set_style_bg_opa(page_left_obj.btn1, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(page_left_obj.btn1, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中
    lv_obj_set_style_radius(page_left_obj.btn1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(page_left_obj.btn1, LV_OPA_0, 0);
    lv_obj_set_style_border_width(page_left_obj.btn1, 3, 0);
    lv_obj_set_style_border_color(page_left_obj.btn1, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_side(page_left_obj.btn1, LV_BORDER_SIDE_FULL, 0);
    lv_obj_align_to(page_left_obj.btn1, page_left_obj.label2, LV_ALIGN_OUT_BOTTOM_MID, 10, 15);
    lv_obj_add_event_cb(page_left_obj.btn1, btn1_event_cb, LV_EVENT_ALL, NULL);

    page_left_obj.btn2 = lv_btn_create(page_left_obj.btn1);
    lv_obj_remove_style_all(page_left_obj.btn2);
    lv_obj_set_size(page_left_obj.btn2, 54, 54);
    lv_obj_set_style_bg_opa(page_left_obj.btn2, LV_OPA_0, 0);
    lv_obj_set_style_radius(page_left_obj.btn2, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(page_left_obj.btn2, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(page_left_obj.btn2, btn2_event_cb, LV_EVENT_ALL, NULL);


    page_left_obj.img2 = lv_img_create(page_left_obj.btn2);
    lv_img_set_src(page_left_obj.img2, LEFT_IMG2);           // +按钮
    lv_obj_center(page_left_obj.img2);
    lv_obj_add_flag(page_left_obj.btn2, LV_OBJ_FLAG_HIDDEN);

    page_left_obj.img3 = lv_img_create(page_left_obj.btn1);
    lv_img_set_src(page_left_obj.img3, LEFT_IMG6);           //
    lv_obj_center(page_left_obj.img3);
    page_left_obj.label3 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label3);
    lv_obj_set_style_text_color(page_left_obj.label3, lv_color_make(211, 212, 212), 0);
    lv_obj_set_style_text_font(page_left_obj.label3, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label3, "母乳");
    lv_obj_align_to(page_left_obj.label3, page_left_obj.btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


    //肉泥
    page_left_obj.btn3 = lv_btn_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.btn3);
    lv_obj_set_size(page_left_obj.btn3, 190, 190);
    lv_obj_set_style_bg_opa(page_left_obj.btn3, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(page_left_obj.btn3, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中
    lv_obj_set_style_radius(page_left_obj.btn3, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(page_left_obj.btn3, LV_OPA_0, 0);
    lv_obj_set_style_border_width(page_left_obj.btn3, 3, 0);
    lv_obj_set_style_border_color(page_left_obj.btn3, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_side(page_left_obj.btn3, LV_BORDER_SIDE_FULL, 0);
    lv_obj_align_to(page_left_obj.btn3, page_left_obj.btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_add_event_cb(page_left_obj.btn3, btn1_event_cb, LV_EVENT_ALL, NULL);

    

    page_left_obj.btn4 = lv_btn_create(page_left_obj.btn3);
    lv_obj_remove_style_all(page_left_obj.btn4);
    lv_obj_set_size(page_left_obj.btn4, 54, 54);
    lv_obj_set_style_bg_opa(page_left_obj.btn4, LV_OPA_0, 0);
    lv_obj_set_style_radius(page_left_obj.btn4, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(page_left_obj.btn4, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(page_left_obj.btn4, btn2_event_cb, LV_EVENT_ALL, NULL);


    page_left_obj.img4 = lv_img_create(page_left_obj.btn4);
    lv_img_set_src(page_left_obj.img4, LEFT_IMG2);           // +按钮
    lv_obj_center(page_left_obj.img4);
    lv_obj_add_flag(page_left_obj.btn4, LV_OBJ_FLAG_HIDDEN);

    page_left_obj.img5 = lv_img_create(page_left_obj.btn3);
    lv_img_set_src(page_left_obj.img5, LEFT_IMG6);           //
    lv_obj_center(page_left_obj.img5);
    page_left_obj.label4 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label4);
    lv_obj_set_style_text_color(page_left_obj.label4, lv_color_make(211, 212, 212), 0);
    lv_obj_set_style_text_font(page_left_obj.label4, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label4, "肉泥");
    lv_obj_align_to(page_left_obj.label4, page_left_obj.btn3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


    //滴眼液
    page_left_obj.btn5 = lv_btn_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.btn5);
    lv_obj_set_size(page_left_obj.btn5, 190, 190);
    lv_obj_set_style_bg_opa(page_left_obj.btn5, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(page_left_obj.btn5, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中
    lv_obj_set_style_radius(page_left_obj.btn5, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(page_left_obj.btn5, LV_OPA_0, 0);
    lv_obj_set_style_border_width(page_left_obj.btn5, 3, 0);
    lv_obj_set_style_border_color(page_left_obj.btn5, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_side(page_left_obj.btn5, LV_BORDER_SIDE_FULL, 0);
    lv_obj_align_to(page_left_obj.btn5, page_left_obj.label3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(page_left_obj.btn5, btn1_event_cb, LV_EVENT_ALL, NULL);

    page_left_obj.btn6 = lv_btn_create(page_left_obj.btn5);
    lv_obj_remove_style_all(page_left_obj.btn6);
    lv_obj_set_size(page_left_obj.btn6, 54, 54);
    lv_obj_set_style_bg_opa(page_left_obj.btn6, LV_OPA_0, 0);
    lv_obj_set_style_radius(page_left_obj.btn6, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(page_left_obj.btn6, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(page_left_obj.btn6, btn2_event_cb, LV_EVENT_ALL, NULL);

    page_left_obj.img6 = lv_img_create(page_left_obj.btn6);
    lv_img_set_src(page_left_obj.img6, LEFT_IMG2);           // +按钮
    lv_obj_center(page_left_obj.img6);
    lv_obj_add_flag(page_left_obj.btn6, LV_OBJ_FLAG_HIDDEN);

    page_left_obj.img7 = lv_img_create(page_left_obj.btn5);
    lv_img_set_src(page_left_obj.img7, LEFT_IMG6);           //
    lv_obj_center(page_left_obj.img7);
    page_left_obj.label5 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label5);
    lv_obj_set_style_text_color(page_left_obj.label5, lv_color_make(211, 212, 212), 0);
    lv_obj_set_style_text_font(page_left_obj.label5, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label5, "滴眼液");
    lv_obj_align_to(page_left_obj.label5, page_left_obj.btn5, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    //胰岛素
    page_left_obj.btn7 = lv_btn_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.btn7);
    lv_obj_set_size(page_left_obj.btn7, 190, 190);
    lv_obj_set_style_bg_opa(page_left_obj.btn7, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(page_left_obj.btn7, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中
    lv_obj_set_style_radius(page_left_obj.btn7, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(page_left_obj.btn7, LV_OPA_0, 0);
    lv_obj_set_style_border_width(page_left_obj.btn7, 3, 0);
    lv_obj_set_style_border_color(page_left_obj.btn7, lv_color_make(228, 185, 132), 0);
    lv_obj_set_style_border_side(page_left_obj.btn7, LV_BORDER_SIDE_FULL, 0);
    lv_obj_align_to(page_left_obj.btn7, page_left_obj.btn5, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_add_event_cb(page_left_obj.btn7, btn1_event_cb, LV_EVENT_ALL, NULL);

    page_left_obj.btn8 = lv_btn_create(page_left_obj.btn7);
    lv_obj_remove_style_all(page_left_obj.btn8);
    lv_obj_set_size(page_left_obj.btn8, 54, 54);
    lv_obj_set_style_bg_opa(page_left_obj.btn8, LV_OPA_0, 0);
    lv_obj_set_style_radius(page_left_obj.btn8, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_align(page_left_obj.btn8, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(page_left_obj.btn8, btn2_event_cb, LV_EVENT_ALL, NULL);

    page_left_obj.img8 = lv_img_create(page_left_obj.btn8);
    lv_img_set_src(page_left_obj.img8, LEFT_IMG2);           // +按钮
    lv_obj_center(page_left_obj.img8);
    lv_obj_add_flag(page_left_obj.btn8, LV_OBJ_FLAG_HIDDEN);

    page_left_obj.img9 = lv_img_create(page_left_obj.btn7);
    lv_img_set_src(page_left_obj.img9, LEFT_IMG6);           //
    lv_obj_center(page_left_obj.img9);
    page_left_obj.label6 = lv_label_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.label6);
    lv_obj_set_style_text_color(page_left_obj.label6, lv_color_make(211, 212, 212), 0);
    lv_obj_set_style_text_font(page_left_obj.label6, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label6, "胰岛素");
    lv_obj_align_to(page_left_obj.label6, page_left_obj.btn7, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);



    //标签
    page_left_obj.obj_temp = lv_obj_create(haier_obj.page_left);
    lv_obj_remove_style_all(page_left_obj.obj_temp);
    lv_obj_set_style_bg_opa(page_left_obj.obj_temp, LV_OPA_0, 0);
    lv_obj_set_size(page_left_obj.obj_temp, 400, 250);
    lv_obj_set_style_border_color(page_left_obj.obj_temp, lv_color_make(77, 77, 77), 0);
    lv_obj_set_style_border_opa(page_left_obj.obj_temp, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(page_left_obj.obj_temp, 1, 0);
    lv_obj_set_style_border_side(page_left_obj.obj_temp, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_align(page_left_obj.obj_temp, LV_ALIGN_CENTER, 0, 150);
    lv_obj_add_flag(page_left_obj.obj_temp, LV_OBJ_FLAG_HIDDEN);

    page_left_obj.label7 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label7);
    lv_obj_set_style_text_color(page_left_obj.label7, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label7, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label7, "存放常识");
    lv_obj_align(page_left_obj.label7, LV_ALIGN_TOP_LEFT, 30, 35);
    
    page_left_obj.label8 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label8);
    lv_obj_set_style_text_color(page_left_obj.label8, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label8, &my_font_80, 0);
    lv_label_set_text(page_left_obj.label8, "--");
    lv_obj_align_to(page_left_obj.label8, page_left_obj.label7, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    page_left_obj.label9 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label9);
    lv_obj_set_style_text_color(page_left_obj.label9, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label9, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label9, "℃");
    lv_obj_align_to(page_left_obj.label9, page_left_obj.label8, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
    page_left_obj.label10 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label10);
    lv_obj_set_style_text_color(page_left_obj.label10, lv_color_make(135, 135, 135), 0);
    lv_obj_set_style_text_font(page_left_obj.label10, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label10, "适宜温度");
    lv_obj_align_to(page_left_obj.label10, page_left_obj.label8, LV_ALIGN_OUT_BOTTOM_MID, 3, 5);

    page_left_obj.label11 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label11);
    lv_obj_set_style_text_color(page_left_obj.label11, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label11, &my_font_80, 0);
    lv_label_set_text(page_left_obj.label11, "--");
    lv_obj_align_to(page_left_obj.label11, page_left_obj.label8, LV_ALIGN_OUT_RIGHT_MID, 120, 0);
    page_left_obj.label12 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label12);
    lv_obj_set_style_text_color(page_left_obj.label12, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(page_left_obj.label12, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label12, "天");
    lv_obj_align_to(page_left_obj.label12, page_left_obj.label11, LV_ALIGN_OUT_RIGHT_TOP, 3, 0);
    page_left_obj.label13 = lv_label_create(page_left_obj.obj_temp);
    lv_obj_remove_style_all(page_left_obj.label13);
    lv_obj_set_style_text_color(page_left_obj.label13, lv_color_make(135, 135, 135), 0);
    lv_obj_set_style_text_font(page_left_obj.label13, &my_font_28, 0);
    lv_label_set_text(page_left_obj.label13, "存放天数");
    lv_obj_align_to(page_left_obj.label13, page_left_obj.label11, LV_ALIGN_OUT_BOTTOM_MID, 3, 5);
}
void user_set_suitable_tp(int tp)
{
    if(page_left_obj.label8)
        lv_label_set_text_fmt(page_left_obj.label8, "%d", tp);
}
void user_set_suitable_day(int day)
{
    if(page_left_obj.label11)
        lv_label_set_text_fmt(page_left_obj.label11, "%d", day);
}
static void btn1_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *child = lv_obj_get_child(target, 0);

    if(code == LV_EVENT_FOCUSED)
    {
        if(child)
            lv_obj_clear_flag(child, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_border_opa(target, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(target, lv_color_make(45, 37, 26), 0);        //45, 37, 26 选中

        lv_obj_clear_flag(page_left_obj.obj_temp, LV_OBJ_FLAG_HIDDEN);
        if(target == page_left_obj.btn1)    //母乳
        {
            user_set_suitable_tp(4);
            user_set_suitable_day(1);
        }else if(target == page_left_obj.btn3){     //肉泥
            user_set_suitable_tp(0);
            user_set_suitable_day(3);
        }else if(target == page_left_obj.btn5){     //滴眼液
            user_set_suitable_tp(5);
            user_set_suitable_day(30);
        }else if(target == page_left_obj.btn7){     //胰岛素
            user_set_suitable_tp(5);
            user_set_suitable_day(30);
        }
    }
    if(code == LV_EVENT_DEFOCUSED)
    {
        if(child)
            lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_border_opa(target, LV_OPA_0, 0);
        lv_obj_set_style_bg_color(target, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中

        lv_obj_add_flag(page_left_obj.obj_temp, LV_OBJ_FLAG_HIDDEN);
    }
}

static void btn2_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *parent = lv_obj_get_parent(target);
    lv_obj_t *child = lv_obj_get_child(target, 0);

    static bool breast_milk_flag = false;               //母乳
    static bool meat_puree_flag = false;                //肉泥
    static bool eye_drops_flag = false;                 //滴眼液
    static bool insulin_flag = false;                   //胰岛素 

    if(code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_border_opa(parent, LV_OPA_COVER, 0);       //外边框
        if(target == page_left_obj.btn2){               //母乳
            if(breast_milk_flag){
            breast_milk_flag = false;
            if(child)
                lv_img_set_src(child, LEFT_IMG2);
            }else{
                breast_milk_flag = true;
                if(child)
                    lv_img_set_src(child, LEFT_IMG4);
            }
        }else if(target == page_left_obj.btn4){         //肉泥
            if(meat_puree_flag){
            meat_puree_flag = false;
            if(child)
                lv_img_set_src(child, LEFT_IMG2);
            }else{
                meat_puree_flag = true;
                if(child)
                    lv_img_set_src(child, LEFT_IMG4);
            }
        }else if(target == page_left_obj.btn6){         //滴眼液
            if(eye_drops_flag){
            eye_drops_flag = false;
            if(child)
                lv_img_set_src(child, LEFT_IMG2);
            }else{
                eye_drops_flag = true;
                if(child)
                    lv_img_set_src(child, LEFT_IMG4);
            }
        }else if(target == page_left_obj.btn8){         //胰岛素
            if(insulin_flag){
            insulin_flag = false;
            if(child)
                lv_img_set_src(child, LEFT_IMG2);
            }else{
                insulin_flag = true;
                if(child)
                    lv_img_set_src(child, LEFT_IMG4);
            }
        }
    }
    if(code == LV_EVENT_FOCUSED)
    {
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_border_opa(parent, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(parent, lv_color_make(45, 37, 26), 0);        //45, 37, 26 选中
        lv_obj_clear_flag(page_left_obj.obj_temp, LV_OBJ_FLAG_HIDDEN);
        if(target == page_left_obj.btn1){    //母乳
            user_set_suitable_tp(4);
            user_set_suitable_day(1);
        }else if(target == page_left_obj.btn3){     //肉泥
            user_set_suitable_tp(0);
            user_set_suitable_day(3);
        }else if(target == page_left_obj.btn5){     //滴眼液
            user_set_suitable_tp(5);
            user_set_suitable_day(30);
        }else if(target == page_left_obj.btn7){     //胰岛素
            user_set_suitable_tp(5);
            user_set_suitable_day(30);
        }
    }
    if(code == LV_EVENT_DEFOCUSED)
    {
        lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_border_opa(parent, LV_OPA_0, 0);
        lv_obj_set_style_bg_color(parent, lv_color_make(35, 36, 35), 0);        //45, 37, 26 选中
        lv_obj_add_flag(page_left_obj.obj_temp, LV_OBJ_FLAG_HIDDEN);
    }
}
