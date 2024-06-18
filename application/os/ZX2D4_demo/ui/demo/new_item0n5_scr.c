// 240 * 284
#include "ui_common.h"



static lv_obj_t *item0n5_scr;

static lv_obj_t *item0n5_label1_bg;
static lv_obj_t *item0n5_label1;        //模式
static lv_obj_t *item0n5_label2_bg;
static lv_obj_t *item0n5_label2;        //光类型
static lv_obj_t *item0n5_label3;        //波长

static lv_obj_t *item0n5_img1;
static lv_obj_t *item0n5_img4;
static lv_obj_t *item0n5_img5;
static lv_obj_t *item0n5_label4;        // mW/cm²  Level
static lv_obj_t *item0n5_label5_bg;
static lv_obj_t *item0n5_label5;        //

static lv_obj_t *item0n5_img2;
static lv_obj_t *item0n5_img6;
static lv_obj_t *item0n5_label6;    //Total
static lv_obj_t *item0n5_label6_1;  //Energy
static lv_obj_t *item0n5_label7;    //J/cm²
static lv_obj_t *item0n5_label8;    //总值


static lv_obj_t *item0n5_img3;
static lv_obj_t *item0n5_img7;
static lv_obj_t *item0n5_label9;    //Time:
static lv_obj_t *item0n5_label10_bg;    //
static lv_obj_t *item0n5_label10;    //
static lv_obj_t *item0n5_label11;    //Min
static lv_obj_t *item0n5_img8;
static lv_obj_t * item0n5_bar1;

static lv_obj_t *item0n5_label12_bg;
static lv_obj_t *item0n5_label12;       //Treating


static int option = 0; // 1-治疗模式    1-光类型    2-强度  3-时间
static int option_confirm = 0; // 1-治疗模式    1-光类型    2-强度  3-时间

static int obj_sel = 0; // 
static int light_mode_sel = 0; // 0-Infrared 1-Red 2-Infrared+Red 3-Dual Infrared 4-Dual-infra+Red
static int time_sel = 15; // 15 30 45 60 75 90 120  (min)
DEVICE_BASE_CONF now_conf[E_LIGHT_MAX] = {0};



static lv_obj_t *item0n5_label1;
static lv_obj_t *item0n5_label4;
static lv_timer_t *item0n5_task1;
static int refr_lock = 1;

static unsigned int work_time = 0;  // unit - sec

static void item0n5_scr_obj_create(void);
static void item0n5_scr_obj_delete(void);

static void manual_refr_obj_disp(void)
{
    //模式
    lv_label_set_text(item0n5_label1, title_mode_text[obj_sel]);
    lv_obj_center(item0n5_label1);

    refr_double_data_to_label(item0n5_label8, calc_conf_energy(light_mode_sel, time_sel));       //总能量
    lv_obj_align_to(item0n5_label8, item0n5_label7, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    if(obj_sel == 0 || obj_sel == 1 || obj_sel == 3) 
    {
        lv_label_set_text(item0n5_label4, "mW/cm²");
    }else{
        lv_label_set_text(item0n5_label4, "Level");
    }
    lv_obj_align_to(item0n5_label4, item0n5_label5_bg, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    
    lv_label_set_text_fmt(item0n5_label5, "%d", now_conf[light_mode_sel]._intensity_level);    //强度
    lv_obj_align(item0n5_label5, LV_ALIGN_CENTER, 0, 0);

    //光类型
    lv_label_set_text(item0n5_label2, light_type_text[light_mode_sel]);
    lv_obj_center(item0n5_label2);

    //波长
    if(light_mode_sel == 0)
        lv_label_set_text(item0n5_label3, "830 nm");
    else if(light_mode_sel == 1)   
        lv_label_set_text(item0n5_label3, "630 nm");
    else if(light_mode_sel == 2)  
        lv_label_set_text(item0n5_label3, "830+630 nm");
    else if(light_mode_sel == 3)   
        lv_label_set_text(item0n5_label3, "830+1050 nm");
    else if(light_mode_sel == 4)  
        lv_label_set_text(item0n5_label3, "830/630/1050 nm");
    else if(light_mode_sel == 5)  
        lv_label_set_text(item0n5_label3, "830+590 nm");
    else if(light_mode_sel == 6 || light_mode_sel == 7 || light_mode_sel == 8)  
        lv_label_set_text(item0n5_label3, "830+630 nm");
    else if(light_mode_sel == 9)  
        lv_label_set_text(item0n5_label3, "830 nm");
    lv_obj_align(item0n5_label3, LV_ALIGN_TOP_RIGHT, -20, 50);



    lv_label_set_text_fmt(item0n5_label10, "%d", time_sel);
    lv_obj_align(item0n5_label10, LV_ALIGN_BOTTOM_MID, -2, 2);

    lv_bar_set_range(item0n5_bar1, 0, time_sel);             //进度条
    lv_bar_set_value(item0n5_bar1, time_sel, LV_ANIM_OFF);

    //工作模式切换
    if(light_mode_sel == 0)
        sys_parm.work_mode = Infrared;
    else if(light_mode_sel == 1)    //
        sys_parm.work_mode = Red;
    else if(light_mode_sel == 2)    // 
        sys_parm.work_mode = Infrared_Red;
    else if(light_mode_sel == 3)    // 
        sys_parm.work_mode = Dual_Infrared;
    else if(light_mode_sel == 4)    // 
        sys_parm.work_mode = Dualinfra_Red;
    else if(light_mode_sel == 5)    // 
        sys_parm.work_mode = Infrared_Yellow;
    else if(light_mode_sel == 6)    // 
        sys_parm.work_mode = Moderate;
    else if(light_mode_sel == 7)    // 
        sys_parm.work_mode = Deep;
    else if(light_mode_sel == 8)    // 
        sys_parm.work_mode = Combination;
    else if(light_mode_sel == 9)    // 
        sys_parm.work_mode = Pain_Relief;

    if(sys_parm.work_status)
    {
        lv_label_set_text(item0n5_label5, "ON");    //强度
        lv_obj_set_style_border_opa(item0n5_label1_bg, LV_OPA_0, 0);  //模式
        lv_obj_set_style_border_opa(item0n5_label2_bg, LV_OPA_0, 0);  //光类型
        lv_obj_set_style_border_opa(item0n5_label5_bg, LV_OPA_0, 0);  //强度
        lv_obj_set_style_border_opa(item0n5_label10_bg, LV_OPA_0, 0); //时间
        lv_obj_align(item0n5_label5, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(item0n5_label12_bg, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_set_style_border_opa(item0n5_label1_bg, LV_OPA_100, 0);  //模式
        lv_obj_set_style_border_opa(item0n5_label2_bg, LV_OPA_100, 0);  //光类型
        lv_obj_set_style_border_opa(item0n5_label5_bg, LV_OPA_100, 0);  //强度
        lv_obj_set_style_border_opa(item0n5_label10_bg, LV_OPA_100, 0); //时间
        lv_obj_add_flag(item0n5_label12_bg, LV_OBJ_FLAG_HIDDEN);
    }
}

static void work_start_exec(void)
{
    // APP_DEBUG("start working and exec %s!", title_text[sys_parm.work_mode]);
    manual_refr_obj_disp();
    work_time = time_sel * 60;
    sys_parm.work_sum_time = work_time;
    sys_parm.work_now_time = work_time;
    //开灯
    if(RT_EOK != rt_event_send(__user_event, __USER_WORK_START_EVENT))
        rt_kprintf("[%s][%d]rt_event_send faild\n", __func__, __LINE__);

    lv_timer_reset(item0n5_task1);
    lv_timer_resume(item0n5_task1);
}

static void work_stop_exec(void)
{
    APP_DEBUG("stop working!");
    lv_timer_pause(item0n5_task1);

    sys_parm.work_status = 0;
    sys_parm.work_now_time = 0;
    manual_refr_obj_disp();
    //关灯
    if(RT_EOK != rt_event_send(__user_event, __USER_WORK_STOP_EVENT))
        rt_kprintf("[%s][%d]rt_event_send faild\n", __func__, __LINE__);
}

static void work_count_down_task(struct _lv_timer_t *timer)
{
    if(work_time)
    {
        work_time-=1;
        APP_DEBUG("work_time:%d\n",work_time);
        sys_parm.work_now_time = work_time;

        if(work_time % 2)
            lv_obj_set_style_bg_color(item0n5_label12_bg, lv_color_make(143, 183, 254), 0); //蓝色
        else
            lv_obj_set_style_bg_color(item0n5_label12_bg, lv_color_make(1, 199, 150), 0);   //绿色
        if(work_time%60 == 0)
        {
            time_sel--;
            lv_label_set_text_fmt(item0n5_label10, "%d", time_sel);
            lv_obj_align(item0n5_label10, LV_ALIGN_BOTTOM_MID, -2, 2);
            
            if(work_time/60 < 1)
                lv_bar_set_value(item0n5_bar1, 1, LV_ANIM_ON);
            else
                lv_bar_set_value(item0n5_bar1, work_time/60, LV_ANIM_ON);
        }
    }
    else
        work_stop_exec();
}

static void item0n5_scr_load_func(void *arg)
{
    // ... ui_init
    if(arg != NULL)
        obj_sel = *(int *)arg;
    item0n5_scr_obj_create();

    refr_lock = 0;
}

static void btn_event_handle(BTN_ID_t which, BTN_STATUS_t status)
{
    // printf("option:%d\n",option);
    // printf("option_confirm:%d\n",option_confirm);
    switch (which)
    {
    case E_K4:
        if ((status == E_S_BUTTON_SINGLE_CLICK) && (!sys_parm.work_status))
        {
            if(option_confirm != 0)
            {
                option_confirm = 0;
                break;
            }
            if(option != 0)
            {
                lv_obj_set_style_bg_opa(item0n5_label1_bg, LV_OPA_0, 0);  //模式
                lv_obj_set_style_bg_opa(item0n5_label2_bg, LV_OPA_0, 0);  //光类型
                lv_obj_set_style_bg_opa(item0n5_label5_bg, LV_OPA_0, 0);  //强度
                lv_obj_set_style_bg_opa(item0n5_label10_bg, LV_OPA_0, 0); //时间
                option = 0;
            }
            else
                scr_load_func(E_HOME_SCR, NULL);
        }
        break;
    case E_K2:
        if (status == E_S_BUTTON_SINGLE_CLICK)
        {
            if(option == 0 && option_confirm == 0)
            {
                //开始工作
                sys_parm.work_status = !sys_parm.work_status;
                if(!sys_parm.work_status)
                {
                    lv_obj_set_style_border_opa(item0n5_label1_bg, LV_OPA_100, 0);  //模式
                    lv_obj_set_style_border_opa(item0n5_label2_bg, LV_OPA_100, 0);  //光类型
                    lv_obj_set_style_border_opa(item0n5_label5_bg, LV_OPA_100, 0);  //强度
                    lv_obj_set_style_border_opa(item0n5_label10_bg, LV_OPA_100, 0); //时间
                    lv_obj_add_flag(item0n5_label12_bg, LV_OBJ_FLAG_HIDDEN);
                }
                printf("work\n");
                option_confirm = 0;
                manual_refr_obj_disp();
            }

            //进入修改模式
            if(option != 0 && option_confirm == 0)
            {
                option_confirm = option;
                break;
            }
            
            if(option_confirm != 0)
            {
                //确认修改
                option_confirm = 0;
            }

            if(sys_parm.work_status)
                work_start_exec();
            else
                work_stop_exec();
        }
        break;
    case E_K1:
        if ((status == E_S_BUTTON_SINGLE_CLICK) && (!sys_parm.work_status))
        {
            // printf("option_confirm:%d\n", option_confirm);

            if(option_confirm == 0)         //选项切换
            {
                option = option + 1 > 4 ? 0 : option + 1;
                lv_obj_set_style_bg_opa(item0n5_label1_bg, LV_OPA_0, 0);  //模式
                lv_obj_set_style_bg_opa(item0n5_label2_bg, LV_OPA_0, 0);  //光类型
                lv_obj_set_style_bg_opa(item0n5_label5_bg, LV_OPA_0, 0);  //强度
                lv_obj_set_style_bg_opa(item0n5_label10_bg, LV_OPA_0, 0); //时间
                if(option == 1)
                {
                    lv_obj_set_style_bg_opa(item0n5_label1_bg, LV_OPA_100, 0);
                }
                else if(option == 2)
                    lv_obj_set_style_bg_opa(item0n5_label2_bg, LV_OPA_100, 0);
                else if(option == 3)
                    lv_obj_set_style_bg_opa(item0n5_label5_bg, LV_OPA_100, 0);
                else if(option == 4)
                    lv_obj_set_style_bg_opa(item0n5_label10_bg, LV_OPA_100, 0);
            }
            else
            {
                if(option_confirm == 1)         //模式切换
                {
                    obj_sel = obj_sel + 1 > 4 ? 0 : obj_sel + 1; 

                    //同步配置
                    if(obj_sel == 0 || obj_sel == 1 || obj_sel == 3)   //
                        light_mode_sel = 0;
                    else if(obj_sel == 2)   //Pain Relief
                        light_mode_sel = 9;
                    else if(obj_sel == 4)   //Hair Generating
                        light_mode_sel = 6;   
                    
                    if(obj_sel == 1)
                    {
                        if(now_conf[light_mode_sel]._time >= 5 && now_conf[light_mode_sel]._time < 10)
                            time_sel = 5;
                        else if(now_conf[light_mode_sel]._time >= 10 && now_conf[light_mode_sel]._time < 15)
                            time_sel = 10;
                        else if(now_conf[light_mode_sel]._time >= 15 && now_conf[light_mode_sel]._time < 20)
                            time_sel = 15;
                        else if(now_conf[light_mode_sel]._time >= 20 && now_conf[light_mode_sel]._time < 30)
                            time_sel = 20;
                        else if(now_conf[light_mode_sel]._time >= 30 && now_conf[light_mode_sel]._time < 40)
                            time_sel = 30;
                        else if(now_conf[light_mode_sel]._time >= 40 && now_conf[light_mode_sel]._time < 50)
                            time_sel = 40;
                        else
                            time_sel = 50;
                    }
                    else{
                        if(now_conf[light_mode_sel]._time <= 15)
                            time_sel = 15;
                        else if(now_conf[light_mode_sel]._time >= 15 && now_conf[light_mode_sel]._time < 30)
                            time_sel = 15;
                        else if(now_conf[light_mode_sel]._time >= 30 && now_conf[light_mode_sel]._time < 45)
                            time_sel = 30;
                        else if(now_conf[light_mode_sel]._time >= 45 && now_conf[light_mode_sel]._time < 60)
                            time_sel = 45;
                        else if(now_conf[light_mode_sel]._time >= 60 && now_conf[light_mode_sel]._time < 75)
                            time_sel = 60;
                        else
                            time_sel = now_conf[light_mode_sel]._time;
                    }
                }
                else if(option_confirm == 2)    //光类型
                {
                    //判断当前模式
                    if(obj_sel == 0)        //Light Therapy
                    {
                        light_mode_sel = light_mode_sel + 1 > 4 ? 0 : light_mode_sel + 1;    //模式切换
                    }
                    else if(obj_sel == 1)   //Beauty&Skincare
                    {
                        light_mode_sel = light_mode_sel + 1 > 5 ? 0 : light_mode_sel + 1;    //模式切换
                        if(light_mode_sel == 3 || light_mode_sel == 4)
                            light_mode_sel = 5;
                    }
                    else if(obj_sel == 2)   //Pain Relief
                    {
                        light_mode_sel = 9;
                    }
                    else if(obj_sel == 3)   //Wound Healing
                    {
                        light_mode_sel = 0;
                    }
                    else if(obj_sel == 4)   //Hair Generating
                    {
                        // light_mode_sel = 6;
                        light_mode_sel = light_mode_sel + 1 > 8 ? 6 : light_mode_sel + 1;    //模式切换
                    }
                    time_sel = now_conf[light_mode_sel]._time;
                }
            }
            manual_refr_obj_disp();
        }
        break;
    case E_K3:
        if ((status == E_S_BUTTON_SINGLE_CLICK) && (!sys_parm.work_status))
        {
            if(option_confirm == 3)    //强度
            {
                if(obj_sel == 0 || obj_sel == 1 || obj_sel == 3) 
                {
                    now_conf[light_mode_sel]._intensity_level = now_conf[light_mode_sel]._intensity_level + 10 > 80 ? 30 : now_conf[light_mode_sel]._intensity_level + 10;    //模式切换
                    dev_conf[light_mode_sel]._intensity_level = now_conf[light_mode_sel]._intensity_level;
                }
                else{
                    now_conf[light_mode_sel]._intensity_level = now_conf[light_mode_sel]._intensity_level + 1 > 5 ? 1 : now_conf[light_mode_sel]._intensity_level + 1;    //模式切换
                    dev_conf[light_mode_sel]._intensity_level = now_conf[light_mode_sel]._intensity_level;
                }
            }
            else if(option_confirm == 4)    //时间
            {
                // time_sel = dev_conf[obj_sel]._time;
                if(obj_sel == 1)
                {
                    if(time_sel < 20)
                        time_sel = time_sel + 5 > 50 ? 5 : time_sel + 5;    //
                    else 
                        time_sel = time_sel + 10 > 50 ? 5 : time_sel + 10;    //
                    now_conf[light_mode_sel]._time = time_sel;
                    dev_conf[light_mode_sel]._time = time_sel;
                }
                else
                {
                    if(time_sel < 90)
                        time_sel = time_sel + 15 > 120 ? 15 : time_sel + 15;    //模式切换
                    else
                        time_sel = time_sel + 30 > 120 ? 15 : time_sel + 30;    //模式切换
                    now_conf[light_mode_sel]._time = time_sel;
                    dev_conf[light_mode_sel]._time = time_sel;
                }
            }
            manual_refr_obj_disp();
        }
        break;
    default:
        break;
    }
}

static void item0n5_scr_refr_func(Event_Data_t *arg)
{
    if(refr_lock)
        return;
    if(arg == NULL)
    {
        
    }
    else
    {
        if(arg->eEventID == E_KEY_EVENT)
            btn_event_handle(arg->lDataArray[0], arg->lDataArray[1]);
    }
}

static void item0n5_scr_quit_func(void *arg)
{
    refr_lock = 1;
    // ... ui_del
    item0n5_scr_obj_delete();
    obj_sel = 0;
}


static void item0n5_scr_obj_create(void)
{
    if(obj_sel >= 5)
        return;
    if(obj_sel == 0 || obj_sel == 1 || obj_sel == 3)
        light_mode_sel = 0;
    else if(obj_sel == 2)
        light_mode_sel = 9;
    else if(obj_sel == 4)
        light_mode_sel = 6;

    option = 0;
    memset(&now_conf, 0, sizeof(now_conf));
    memcpy(&now_conf, &dev_conf, sizeof(dev_conf));
    printf("obj_sel:%d\n",obj_sel);
    printf("light_mode_sel:%d\n",light_mode_sel);

    time_sel = now_conf[light_mode_sel]._time;
    // printf("text:%s\n",title_mode_text[obj_sel]);

    item0n5_label1_bg = lv_obj_create(item0n5_scr);
    lv_obj_remove_style_all(item0n5_label1_bg);
    lv_obj_set_size(item0n5_label1_bg, 143, 26);
    lv_obj_set_style_bg_opa(item0n5_label1_bg, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(item0n5_label1_bg, lv_color_make(62, 62, 62), 0);
    lv_obj_set_style_border_width(item0n5_label1_bg, 1, 0);
    lv_obj_set_style_border_color(item0n5_label1_bg, lv_color_white(), 0);
    lv_obj_set_style_border_opa(item0n5_label1_bg, LV_OPA_100, 0);
    // lv_obj_set_style_bg_opa(item0n5_label1_bg, LV_OPA_100, 0);
    lv_obj_align(item0n5_label1_bg, LV_ALIGN_TOP_MID, 0, 10);

    item0n5_label1 = lv_label_create(item0n5_label1_bg);
	lv_obj_set_style_text_font(item0n5_label1, &montserrat_el_17, 0);
	lv_obj_set_style_text_color(item0n5_label1, lv_color_white(), 0);
    lv_label_set_text(item0n5_label1, title_mode_text[obj_sel]);
    lv_obj_center(item0n5_label1);

    item0n5_label2_bg = lv_obj_create(item0n5_scr);
    lv_obj_remove_style_all(item0n5_label2_bg);
    lv_obj_set_size(item0n5_label2_bg, 115, 26);
    lv_obj_set_style_bg_opa(item0n5_label2_bg, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(item0n5_label2_bg, lv_color_make(62, 62, 62), 0);
    lv_obj_set_style_border_width(item0n5_label2_bg, 1, 0);
    lv_obj_set_style_border_color(item0n5_label2_bg, lv_color_white(), 0);
    lv_obj_set_style_border_opa(item0n5_label2_bg, LV_OPA_100, 0);
    // lv_obj_set_style_bg_opa(item0n5_label2_bg, LV_OPA_100, 0);
    lv_obj_align(item0n5_label2_bg, LV_ALIGN_TOP_LEFT, 20, 45);

    item0n5_label2 = lv_label_create(item0n5_label2_bg);
	lv_obj_set_style_text_font(item0n5_label2, &montserrat_el_16, 0);
	lv_obj_set_style_text_color(item0n5_label2, lv_color_white(), 0);
    lv_label_set_text(item0n5_label2, light_type_text[light_mode_sel]);
    lv_obj_center(item0n5_label2);

    item0n5_label3 = lv_label_create(item0n5_scr);
	lv_obj_set_style_text_font(item0n5_label3, &montserrat_el_16, 0);
	lv_obj_set_style_text_color(item0n5_label3, lv_color_white(), 0);
    if(light_mode_sel == 0)
        lv_label_set_text(item0n5_label3, "830 nm");
    else if(light_mode_sel == 1)   
        lv_label_set_text(item0n5_label3, "630 nm");
    else if(light_mode_sel == 2)  
        lv_label_set_text(item0n5_label3, "830+630 nm");
    else if(light_mode_sel == 3)   
        lv_label_set_text(item0n5_label3, "830+1050 nm");
    else if(light_mode_sel == 4)  
        lv_label_set_text(item0n5_label3, "830/630/1050 nm");
    else if(light_mode_sel == 5)  
        lv_label_set_text(item0n5_label3, "830+590 nm");
    else if(light_mode_sel == 6 || light_mode_sel == 7 || light_mode_sel == 8)  
        lv_label_set_text(item0n5_label3, "830+630 nm");
    else if(light_mode_sel == 9)  
        lv_label_set_text(item0n5_label3, "830 nm");
    lv_obj_align(item0n5_label3, LV_ALIGN_TOP_RIGHT, -20, 50);


    //
    item0n5_img1 = lv_img_create(item0n5_scr);
    lv_img_set_src(item0n5_img1, LVGL_DIR"new_mask12.png");
    lv_obj_clear_flag(item0n5_img1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(item0n5_img1, 16, 80);

    item0n5_img4 = lv_img_create(item0n5_img1);
    lv_img_set_src(item0n5_img4, LVGL_DIR"new_mask15.png");
    lv_obj_clear_flag(item0n5_img4, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(item0n5_img4, LV_ALIGN_LEFT_MID, 10, 0);

    item0n5_img5 = lv_img_create(item0n5_img1);
    lv_img_set_src(item0n5_img5, LVGL_DIR"new_mask18.png");
    lv_obj_clear_flag(item0n5_img5, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(item0n5_img5, LV_ALIGN_CENTER, -15, 3);

    

    item0n5_label5_bg = lv_obj_create(item0n5_img1);
    lv_obj_remove_style_all(item0n5_label5_bg);
    lv_obj_set_size(item0n5_label5_bg, 37, 26);
    lv_obj_set_style_bg_opa(item0n5_label5_bg, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(item0n5_label5_bg, lv_color_make(62, 62, 62), 0);
    lv_obj_set_style_border_width(item0n5_label5_bg, 1, 0);
    lv_obj_set_style_border_color(item0n5_label5_bg, lv_color_white(), 0);
    lv_obj_set_style_border_opa(item0n5_label5_bg, LV_OPA_100, 0);
    // lv_obj_set_style_bg_opa(item0n5_label5_bg, LV_OPA_100, 0);
    lv_obj_align(item0n5_label5_bg, LV_ALIGN_TOP_RIGHT, -25, 15);

    item0n5_label5 = lv_label_create(item0n5_label5_bg);
    lv_obj_set_style_text_font(item0n5_label5, &montserrat_el_23, 0);
	lv_obj_set_style_text_color(item0n5_label5, lv_color_white(), 0);
    lv_label_set_text_fmt(item0n5_label5, "%d", now_conf[light_mode_sel]._intensity_level);    //强度
    lv_obj_align(item0n5_label5, LV_ALIGN_CENTER, 0, 0);

    item0n5_label4 = lv_label_create(item0n5_img1);
    lv_obj_set_style_text_font(item0n5_label4, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label4, lv_color_white(), 0);
    if(obj_sel == 0 || obj_sel == 1 || obj_sel == 3) 
    {
        lv_label_set_text(item0n5_label4, "mW/cm²");
    }else{
        lv_label_set_text(item0n5_label4, "Level");
    }
    lv_obj_align_to(item0n5_label4, item0n5_label5_bg, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);



    //Total Energy
    item0n5_img2 = lv_img_create(item0n5_scr);
    lv_img_set_src(item0n5_img2, LVGL_DIR"new_mask13.png");
    lv_obj_clear_flag(item0n5_img2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(item0n5_img2, item0n5_img1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    item0n5_img6 = lv_img_create(item0n5_img2);
    lv_img_set_src(item0n5_img6, LVGL_DIR"new_mask16.png");
    lv_obj_clear_flag(item0n5_img6, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(item0n5_img6, LV_ALIGN_OUT_TOP_LEFT, 10, 10);

    item0n5_label6 = lv_label_create(item0n5_img2);
    lv_obj_set_style_text_font(item0n5_label6, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label6, lv_color_white(), 0);
    lv_label_set_text(item0n5_label6, "Total");
    lv_obj_align(item0n5_label6, LV_ALIGN_CENTER, 20, -15);
    
    item0n5_label6_1 = lv_label_create(item0n5_img2);
    lv_obj_set_style_text_font(item0n5_label6_1, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label6_1, lv_color_white(), 0);
    lv_label_set_text(item0n5_label6_1, "Energy");
    lv_obj_align_to(item0n5_label6_1, item0n5_label6, LV_ALIGN_OUT_BOTTOM_LEFT, -6, 0);

    item0n5_label7 = lv_label_create(item0n5_img2);
    lv_obj_set_style_text_font(item0n5_label7, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label7, lv_color_white(), 0);
    lv_label_set_text(item0n5_label7, "J/cm²");
    lv_obj_align(item0n5_label7, LV_ALIGN_BOTTOM_MID, 25, -5);

    item0n5_label8 = lv_label_create(item0n5_img2);
    lv_obj_set_style_text_font(item0n5_label8, &montserrat_el_18, 0);
	lv_obj_set_style_text_color(item0n5_label8, lv_color_white(), 0);
    lv_label_set_text(item0n5_label8, "100");
    lv_obj_align_to(item0n5_label8, item0n5_label7, LV_ALIGN_OUT_LEFT_MID, -5, 0);


    //时间
    item0n5_img3 = lv_img_create(item0n5_scr);
    lv_img_set_src(item0n5_img3, LVGL_DIR"new_mask14.png");
    lv_obj_clear_flag(item0n5_img3, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align_to(item0n5_img3, item0n5_img1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    item0n5_img7 = lv_img_create(item0n5_img3);
    lv_img_set_src(item0n5_img7, LVGL_DIR"new_mask17.png");
    lv_obj_clear_flag(item0n5_img7, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(item0n5_img7, LV_ALIGN_LEFT_MID, 10, 0);

    item0n5_label9 = lv_label_create(item0n5_img3);
    lv_obj_set_style_text_font(item0n5_label9, &montserrat_el_18, 0);
	lv_obj_set_style_text_color(item0n5_label9, lv_color_white(), 0);
    lv_label_set_text(item0n5_label9, "Time:");
    lv_obj_align(item0n5_label9, LV_ALIGN_LEFT_MID, 55, -8);

    item0n5_label10_bg = lv_obj_create(item0n5_img3);
    lv_obj_remove_style_all(item0n5_label10_bg);
    lv_obj_set_size(item0n5_label10_bg, 36, 20);
    lv_obj_set_style_bg_opa(item0n5_label10_bg, LV_OPA_0, 0);
	lv_obj_set_style_bg_color(item0n5_label10_bg, lv_color_make(62, 62, 62), 0);
    lv_obj_set_style_border_width(item0n5_label10_bg, 1, 0);
    lv_obj_set_style_border_color(item0n5_label10_bg, lv_color_white(), 0);
    lv_obj_set_style_border_opa(item0n5_label10_bg, LV_OPA_100, 0);
    // lv_obj_set_style_bg_opa(item0n5_label10_bg, LV_OPA_100, 0);
    lv_obj_align_to(item0n5_label10_bg, item0n5_label9, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

    item0n5_label10 = lv_label_create(item0n5_label10_bg);
    lv_obj_set_style_text_font(item0n5_label10, &montserrat_el_18, 0);
	lv_obj_set_style_text_color(item0n5_label10, lv_color_white(), 0);
    lv_label_set_text(item0n5_label10, "120");
    lv_obj_align(item0n5_label10, LV_ALIGN_BOTTOM_MID, -2, 2);

    item0n5_label11 = lv_label_create(item0n5_img3);
    lv_obj_set_style_text_font(item0n5_label11, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label11, lv_color_white(), 0);
    lv_label_set_text(item0n5_label11, "Mins");
    lv_obj_align_to(item0n5_label11, item0n5_label10_bg, LV_ALIGN_OUT_RIGHT_MID, 2, 2);

    item0n5_bar1 = lv_bar_create(item0n5_img3);
    lv_obj_set_size(item0n5_bar1, 217, 14);
    lv_obj_set_style_radius(item0n5_bar1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item0n5_bar1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item0n5_bar1, lv_color_white(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item0n5_bar1, lv_color_make(44, 44, 44), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(item0n5_bar1, LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(item0n5_bar1, LV_ALIGN_BOTTOM_MID, 20, -15);
    lv_bar_set_value(item0n5_bar1, 0, LV_ANIM_ON);

    item0n5_img8 = lv_img_create(item0n5_bar1);
    lv_img_set_src(item0n5_img8, LVGL_DIR"new_mask19_2.png");
    lv_obj_clear_flag(item0n5_img8, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(item0n5_img8, LV_ALIGN_CENTER, -1, 0);


    item0n5_label12_bg = lv_obj_create(item0n5_img3);
    lv_obj_remove_style_all(item0n5_label12_bg);
    lv_obj_set_size(item0n5_label12_bg, 67, 18);
    lv_obj_set_style_radius(item0n5_label12_bg, 5, 0);
    lv_obj_set_style_bg_opa(item0n5_label12_bg, LV_OPA_100, 0);
	lv_obj_set_style_bg_color(item0n5_label12_bg, lv_color_make(143, 183, 254), 0);
    lv_obj_align(item0n5_label12_bg, LV_ALIGN_TOP_RIGHT, -18, 15);

    item0n5_label12 = lv_label_create(item0n5_label12_bg);
    lv_obj_set_style_text_font(item0n5_label12, &montserrat_el_14, 0);
	lv_obj_set_style_text_color(item0n5_label12, lv_color_black(), 0);
    lv_label_set_text(item0n5_label12, "Treating");
    lv_obj_align(item0n5_label12, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(item0n5_label12_bg, LV_OBJ_FLAG_HIDDEN);


    manual_refr_obj_disp();

}

static void item0n5_scr_obj_delete(void)
{
    lv_obj_clean(item0n5_scr);
}

void item0n5_scr_create(void)
{
    item0n5_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(item0n5_scr, lv_color_black(), 0);
    lv_obj_clear_flag(item0n5_scr, LV_OBJ_FLAG_SCROLLABLE);

    item0n5_task1 = lv_timer_create(work_count_down_task, 1000, NULL);
    lv_timer_pause(item0n5_task1);

    SCR_FUNC_TYPE *_user_data = malloc(sizeof(SCR_FUNC_TYPE));
    _user_data->load_func = item0n5_scr_load_func;
    _user_data->refr_func = item0n5_scr_refr_func;
    _user_data->quit_func = item0n5_scr_quit_func;

    scr_add_user_data(item0n5_scr, E_ITEM0N5_SCR, _user_data);
}

