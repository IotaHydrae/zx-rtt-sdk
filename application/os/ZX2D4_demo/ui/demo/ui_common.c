// 240 * 284

#include "ui_common.h"


static lv_obj_t *hint_cont = NULL;
static lv_obj_t *hint_cont2 = NULL;


const char *title_mode_text[] = {
    "Light Therapy",
    "Beauty&Skincare",
    "Pain Relief",      // 1 2 3 4 5
    "Wound Healing",
    "Hair Generating",  // 1 2 3 4 5
};

const char *light_type_text[] = {
    "Infrared",             //830nm             0
    "Red",                  //630nm             1
    "Infrared+Red",         //830+630 nm        2
    "Dual Infrared",        //830+1050 nm       3
    "Dual-infra+Red",       //830/630/1050      4
    "Infrared+Yellow",      //830+590 nm        5

    //生发类型
    "Moderate",                               //6
    "Deep",                                   //7
    "Combination",                            //8

    "Infrared/Red",                            //9
};


SYS_PARM_TYPE sys_parm = {
    .work_mode = E_LIGHT_MAX,
    .work_status = 0,
    .work_sum_time = 0,
    .work_now_time = 0,
    .charge_level = 1,
};

DEVICE_BASE_CONF dev_conf[E_LIGHT_MAX] = {
    // Light Therapy
    {._intensity_level = 30, ._633_intensity = 0, ._830_intensity = 30,  ._1050_intensity = 0,   ._590_intensity = 0,    ._time = 15},    //Infrared  
    {._intensity_level = 30, ._633_intensity = 30, ._830_intensity = 0,  ._1050_intensity = 0,   ._590_intensity = 0,    ._time = 15},    //Red
    {._intensity_level = 30, ._633_intensity = 30, ._830_intensity = 30,  ._1050_intensity = 0,  ._590_intensity = 0,    ._time = 15},   //Infrared+Red
    {._intensity_level = 30, ._633_intensity = 0, ._830_intensity = 30,  ._1050_intensity = 30,  ._590_intensity = 0,    ._time = 15},   //Dual Infrared
    {._intensity_level = 30, ._633_intensity = 30, ._830_intensity = 30,  ._1050_intensity = 30, ._590_intensity = 0,    ._time = 15},  //Dual-infra+Red
    {._intensity_level = 30, ._633_intensity = 0, ._830_intensity = 30, ._1050_intensity = 0,    ._590_intensity = 30,    ._time = 15},   //Infrared+Yellow

    

    // Hair Generating
    {._intensity_level = 1, ._633_intensity = 0, ._830_intensity = 0, ._1050_intensity = 0,   ._590_intensity = 0,    ._time = 15}, 
    {._intensity_level = 1, ._633_intensity = 0, ._830_intensity = 0, ._1050_intensity = 0,   ._590_intensity = 0,    ._time = 15}, 
    {._intensity_level = 1, ._633_intensity = 0, ._830_intensity = 0, ._1050_intensity = 0,   ._590_intensity = 0,    ._time = 15}, 

    // Pain Relief
    {._intensity_level = 1, ._633_intensity = 0, ._830_intensity = 0, ._1050_intensity = 0,  ._590_intensity = 0,    ._time = 15}, 
};

#if 1   // 屏幕刷新加载逻辑

static E_SCREEN_ID cur_scr = E_MAX_SCR;
static SCR_FUNC_TYPE *act_scr_func = NULL;
static SCR_FUNC_TYPE *top_layer_func = NULL;
static SCR_LIST_TYPE scr_parm_list[E_MAX_SCR] = {0};

void scr_add_user_data(lv_obj_t *scr, E_SCREEN_ID id, SCR_FUNC_TYPE *desc)
{
    scr_parm_list[id].scr = scr;
    scr_parm_list[id].user_data = desc;

    if(id == E_TOP_LAYER)
        top_layer_func = desc;
}

void scr_refr_func(Event_Data_t *args)
{
    if(act_scr_func)
        act_scr_func->refr_func(args);
    if(top_layer_func)
        top_layer_func->refr_func(args);
}

void scr_quit_func(void *args)
{
    if(act_scr_func != NULL)
        act_scr_func->quit_func(args);
}

void scr_load_func(E_SCREEN_ID id, void *args)
{
    if(id >= E_MAX_SCR)
    {
        APP_DEBUG("%s value overflow", __func__);
        return;
    }
    cur_scr = id;
    if((scr_parm_list[id].scr == NULL) || (scr_parm_list[id].user_data == NULL))
        return;

    if(act_scr_func != NULL)
        act_scr_func->quit_func(args);

    scr_parm_list[id].user_data->load_func(args);

    if(top_layer_func != NULL)
        top_layer_func->load_func(&id);

    lv_scr_load(scr_parm_list[id].scr);
    act_scr_func = scr_parm_list[id].user_data;
}

E_SCREEN_ID scr_act_get(void)
{
    return cur_scr;
}

void click_load_scr_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
        if(e->user_data != NULL)
        {
            E_SCREEN_ID *pid = (E_SCREEN_ID *)e->user_data;
            scr_load_func(*pid, NULL);
        }
	}
}

/***************************************************************
  *  @brief 容器创建
  *  @note  备注: 图标容器
 **************************************************************/

lv_obj_t *new_scr_base_cont1_create(lv_obj_t *par, const char *text1)
{
    lv_obj_t *cont1 = lv_obj_create(par);
    lv_obj_set_size(cont1, 260, 50);
    lv_obj_set_style_bg_color(cont1, lv_color_make(62, 62, 62), LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont1, lv_color_make(243, 133, 40), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(cont1, 0, 0);
    lv_obj_set_style_radius(cont1, 10, 0);
    lv_obj_clear_flag(cont1, LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t *label1 = lv_label_create(cont1);
	lv_label_set_text(label1, text1);
	lv_obj_set_style_text_font(label1, &montserrat_el_17, 0);
	lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    
    return cont1;
}

lv_obj_t *new_scr_base_cont2_create(lv_obj_t *par, const char *text1)
{
    lv_obj_t *cont1 = lv_obj_create(par);
    lv_obj_set_size(cont1, 128, 50);
    lv_obj_set_style_bg_color(cont1, lv_color_make(62, 62, 62), LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont1, lv_color_make(243, 133, 40), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(cont1, 0, 0);
    lv_obj_set_style_radius(cont1, 10, 0);
    lv_obj_clear_flag(cont1, LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t *label1 = lv_label_create(cont1);
	lv_label_set_text(label1, text1);
	lv_obj_set_style_text_font(label1, &montserrat_el_17, 0);
	lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    
    return cont1;
}


void refr_double_data_to_label(lv_obj_t *label, double data)
{
    char text[64] = {0};
    snprintf(text, sizeof(text), "%0.1f", data);
    lv_label_set_text(label, text);
    text[63] = '\0';
}

double calc_conf_energy(E_LIGHT_TYPE id, int dym_time)
{
    double _energy = 0;
    int time = 0;
    if(dym_time != -1)
        time = dym_time;
    else
        time = now_conf[id]._time;

    //等级：1-2-3-4-5  -->  光强度:50-60-70-75-80   --> PWM占空比:63%-75%-88%-94%-100%
    if(id == Pain_Relief || id == Moderate || id == Deep || id == Combination)
    {
        double light_level = 0;
        if(now_conf[id]._intensity_level == 1)
            light_level = 50.0;
        else if(now_conf[id]._intensity_level == 2)
            light_level = 60.0;
        else if(now_conf[id]._intensity_level == 3)
            light_level = 70.0;
        else if(now_conf[id]._intensity_level == 4)
            light_level = 75.0;
        else if(now_conf[id]._intensity_level == 5)
            light_level = 80.0;
        _energy = (double)time * 60.0 * light_level / 1000.0;
    }else{
        _energy = (double)time * 60.0 * now_conf[id]._intensity_level / 1000.0;
    }
    printf("id:%d\n", id);
    printf("time:%d\n", time);
    printf("_energy:%f\n", _energy);
    return _energy;
}

void hint_cont_create(void)
{
    if(hint_cont)
        return;

    hint_cont = lv_obj_create(lv_layer_top());
    lv_obj_set_size(hint_cont, 240, 280);
    lv_obj_set_style_bg_color(hint_cont, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(hint_cont, 210, 0);
    lv_obj_set_style_border_width(hint_cont, 0, 0);
    lv_obj_set_style_radius(hint_cont, 0, 0);
    lv_obj_clear_flag(hint_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(hint_cont, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label1 = lv_label_create(hint_cont);
	lv_label_set_text(label1, "Low battery\n Please charge it!");
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(label1, &montserrat_x20b, 0);
	lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
}

void hint_cont_delete(void)
{
    if(!hint_cont)
        return;
    lv_obj_del_async(hint_cont);
    hint_cont = NULL;
}

void hint_cont2_create(void)
{
    if(hint_cont2)
        return;

    hint_cont2 = lv_obj_create(lv_layer_top());
    lv_obj_set_size(hint_cont2, 240, 280);
    lv_obj_set_style_bg_color(hint_cont2, lv_color_make(223, 223, 223), 0);
    lv_obj_set_style_bg_opa(hint_cont2, 255, 0);
    lv_obj_set_style_border_width(hint_cont2, 0, 0);
    lv_obj_set_style_radius(hint_cont2, 0, 0);
    lv_obj_clear_flag(hint_cont2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(hint_cont2, LV_ALIGN_CENTER, 0, 0);

}

void hint_cont2_delete(void)
{
    if(!hint_cont2)
        return;
    lv_obj_del_async(hint_cont2);
    hint_cont2 = NULL;
}

#endif
