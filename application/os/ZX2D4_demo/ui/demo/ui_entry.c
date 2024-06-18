// 240 * 284
#include "ui_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <rtdevice.h>
#include <aic_core.h>
#include <aic_hal.h>
#include "pwm.h"

#define SRC_PREFIX "D:\\shared\\assets\\mask\\font\\"
// #define VIRTUAL_KEYBORAD
#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define AIC_GPAI_NAME               "gpai"

rt_event_t __user_event = RT_NULL;
static int event_lock = 0;  // 0-开机 1-关机
static lv_obj_t *black_mask = NULL;  // 屏保


static long pin = 0;

static unsigned int g_C_0 = 0;
static unsigned int g_C_1 = 0;
static unsigned int g_C_2 = 0;
static unsigned int g_C_3 = 0;

static unsigned int p_C_0 = 0;
static unsigned int p_C_1 = 0;
static unsigned int p_C_2 = 0;
static unsigned int p_C_3 = 0;

static rt_adc_device_t gpai_dev = NULL;

static unsigned int g_E_12 = 0;
static unsigned int g_E_13 = 0;
static unsigned int p_E_12 = 0;
static unsigned int p_E_13 = 0;

#ifdef VIRTUAL_KEYBORAD
char label_name[4][16] = {"K1", "K2", "K3", "K4"};
static Event_Data_t xReceivedEvent = {
    .eEventID = 0xff,
}; 
void label_key_event_handler(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if ((code == LV_EVENT_SHORT_CLICKED) || (code == LV_EVENT_LONG_PRESSED))
    {
        char *key_name = lv_label_get_text(obj);

        if (!strcmp(key_name, "K1"))
        {
            xReceivedEvent.eEventID = E_KEY_EVENT;
            xReceivedEvent.lDataArray[0] = E_K1;
        }
        else if (!strcmp(key_name, "K2"))
        {
            xReceivedEvent.eEventID = E_KEY_EVENT;
            xReceivedEvent.lDataArray[0] = E_K2;
        }
        else if (!strcmp(key_name, "K3"))
        {
            xReceivedEvent.eEventID = E_KEY_EVENT;
            xReceivedEvent.lDataArray[0] = E_K3;
        }
        else if (!strcmp(key_name, "K4"))
        {
            xReceivedEvent.eEventID = E_KEY_EVENT;
            xReceivedEvent.lDataArray[0] = E_K4;
        }
        if(code == LV_EVENT_SHORT_CLICKED)
            xReceivedEvent.lDataArray[1] = E_S_CLICKED;
        else if(code == LV_EVENT_LONG_PRESSED)
            xReceivedEvent.lDataArray[1] = E_L_CLICKED;
        // APP_DEBUG("%s %d", key_name, xReceivedEvent.lDataArray[1]);
    }
}
lv_obj_t *vir_area;
void virtual_keyborad(void)
{
    vir_area = lv_obj_create(lv_layer_top());
    lv_obj_set_size(vir_area, 120, 30);
    lv_obj_align(vir_area, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(vir_area, lv_color_white(), 0);
    lv_obj_set_style_border_width(vir_area, 0, 0);
    lv_obj_clear_flag(vir_area, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 4; i++)
    {
        lv_obj_t *label = lv_label_create(vir_area);
        lv_label_set_text(label, label_name[i]);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_13, 0);
        lv_obj_set_style_text_color(label, lv_color_make(10, 10, 10), 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, i * 23 + 5, 0);
        lv_obj_add_event_cb(label, label_key_event_handler, LV_EVENT_SHORT_CLICKED, NULL);
        lv_obj_add_event_cb(label, label_key_event_handler, LV_EVENT_LONG_PRESSED, NULL);
        lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    }
}
#endif

static void memory_print(void)
{
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    APP_DEBUG("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d", (int)mon.total_size - mon.free_size,
        mon.used_pct,
        mon.frag_pct,
        (int)mon.free_biggest_size);
}


static void scr_refr_task(struct _lv_timer_t* t)	// 500ms
{
    scr_refr_func(NULL);    // 定时读取电量，刷新显示
#ifdef VIRTUAL_KEYBORAD
    if(xReceivedEvent.eEventID != 0xff)   //判断是否队列是否存在新数据
    {
        if(xReceivedEvent.eEventID == E_KEY_EVENT)
        {
            if((xReceivedEvent.lDataArray[0] == E_K3) && (xReceivedEvent.lDataArray[1] == E_L_CLICKED))
            {
                event_lock = !event_lock;   // 开关机  关闭pwm
                scr_load_func(E_HOME_SCR, NULL);
                if (event_lock)
                {
                    APP_DEBUG("turn off device!");
                    lv_obj_clear_flag(black_mask, LV_OBJ_FLAG_HIDDEN);
                }
                else
                {
                    APP_DEBUG("turn on device!");
                    lv_obj_add_flag(black_mask, LV_OBJ_FLAG_HIDDEN);
                }
            }
            else if(!event_lock)
            {
                scr_refr_func(&xReceivedEvent);
            }
        }
    }
    xReceivedEvent.eEventID = 0xff;
#else
    Event_Data_t xReceivedEvent;
    scr_refr_func(&xReceivedEvent); // 处理各类事件
#endif 
}



void user_task(struct _lv_timer_t *t)
{
#if 0   // 轮播
    // memory_print();
    static int i = 0;
    scr_load_func(i, NULL);
    i = i < E_TOP_LAYER - 1 ? i + 1 : E_HOME_SCR;
#else   // 打印设备状态信息
    // APP_DEBUG("status: %s", sys_parm.work_status ? "working" : "stop");
    // APP_DEBUG("mode: %d", sys_parm.work_mode);
    if((sys_parm.work_status) && (sys_parm.work_mode < E_LIGHT_MAX))
    {
        // unsigned int pvalue = 0;
        // hal_gpio_get_value(g_C_0, p_C_0, &pvalue);
        // APP_DEBUG("红灯 630: %d", pvalue);
        // hal_gpio_get_value(g_C_2, p_C_2, &pvalue);
        // APP_DEBUG("红外灯 830: %d", pvalue);
        // hal_gpio_get_value(g_C_1, p_C_1, &pvalue);
        // APP_DEBUG("红外灯 1050: %d", pvalue);
        // hal_gpio_get_value(g_C_3, p_C_3, &pvalue);
        // APP_DEBUG("黄灯 590: %d", pvalue);

        // APP_DEBUG("intensity level: %d", now_conf[sys_parm.work_mode]._intensity_level);
        // APP_DEBUG("setting time: %d", now_conf[sys_parm.work_mode]._time);
    }
#endif

}


static lv_obj_t *logo = NULL;
static lv_obj_t *logo_jpg = NULL;
static lv_timer_t *timer_player = NULL;
#define SRC2_PATH_HEAD "L:/rodata/init_jpg/"
static void jpg_player(lv_timer_t *t)
{
    char _path[64] = {0};
    static int i = 1;
    
    lv_snprintf(_path, sizeof(_path), "L:/rodata/init_jpg/%03d.jpg", i++);
    lv_img_set_src(logo_jpg, _path);
    lv_obj_align(logo_jpg, LV_ALIGN_BOTTOM_MID, 0, 0);

    if(i == 76)
    {
        lv_timer_del(timer_player);
        lv_img_cache_set_size(15);
        scr_load_func(E_HOME_SCR, NULL);    //加载主界面
    }
}

static void bootlogo_init(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    logo = lv_img_create(lv_scr_act());
    lv_img_set_src(logo, "L:/rodata/init_jpg/mask_init_font.png");
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -15);

    lv_img_cache_invalidate_src(NULL);
    logo_jpg = lv_img_create(lv_scr_act());
    timer_player = lv_timer_create(jpg_player, 25, NULL);
}


static void __check_tp_task(void *parameter)
{
    static bool flag = false;

    int val = 0;
    val = rt_adc_read(gpai_dev, 4);
    
    // rt_base_t pin_A_4 = rt_pin_get("PA.4");
    for(;;)
    {
        val = rt_adc_read(gpai_dev, 4);
        float tmp_v = (float)val/4095.0*3.0;
        // APP_DEBUG("GPAI ch%d: val:%d tmp_v:%f\n", 4, val, tmp_v);

        if(tmp_v <= 2.15 && flag == false)   //过温保护 
        {
            flag = true;
            pwm_set_duty(PWM_CH_0, PWN_SIG_A, 0.0);
            hal_gpio_clr_output(g_C_0, p_C_0);
            hal_gpio_clr_output(g_C_1, p_C_1);
            hal_gpio_clr_output(g_C_2, p_C_2);
            hal_gpio_clr_output(g_C_3, p_C_3);

            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 50.0);   //蜂鸣器
            rt_thread_delay(1000);
            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 0.0);   //蜂鸣器
            rt_thread_delay(2000);

            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 50.0);   //蜂鸣器
            rt_thread_delay(1000);
            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 0.0);   //蜂鸣器
            rt_thread_delay(2000);

            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 50.0);   //蜂鸣器
            rt_thread_delay(1000);
            pwm_set_duty(PWM_CH_1, PWN_SIG_A, 0.0);   //蜂鸣器
        }
        else if(flag && tmp_v > 2.15)
        {
            flag = false;
        }
        rt_thread_delay(1000);
    }
}

static void __check_work_task(void *parameter)
{
    static rt_uint32_t event_recv = 0;
    
    for(;;)
    {
        rt_event_recv(__user_event, 0xFFFFFFFF, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &event_recv);
        if(event_recv & __USER_WORK_START_EVENT){
            //工作状态指示灯
            hal_gpio_clr_output(g_E_12, p_E_12);
            
            switch (sys_parm.work_mode)
            {
            case Infrared:
                printf("Infrared\n");
                hal_gpio_set_output(g_C_2, p_C_2);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);
                
                break;

            case Red:
                printf("Red\n");
                hal_gpio_set_output(g_C_0, p_C_0);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);
                
                break;
            case Infrared_Red:
                printf("Infrared_Red\n");
                hal_gpio_set_output(g_C_0, p_C_0);
                hal_gpio_set_output(g_C_2, p_C_2);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);

                break;
            case Dual_Infrared:
                printf("Dual_Infrared\n");
                hal_gpio_set_output(g_C_1, p_C_1);
                hal_gpio_set_output(g_C_2, p_C_2);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);

                break;
            case Dualinfra_Red:
                printf("Dualinfra_Red\n");
                hal_gpio_set_output(g_C_0, p_C_0);
                hal_gpio_set_output(g_C_1, p_C_1);
                hal_gpio_set_output(g_C_2, p_C_2);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);

                break;
            case Infrared_Yellow:
                printf("Infrared_Yellow\n");
                hal_gpio_set_output(g_C_2, p_C_2);
                hal_gpio_set_output(g_C_3, p_C_3);

                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 30)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 38.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 40)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 50.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 50)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 63.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 60)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 75.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 70)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 88.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 80)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);

                break;
            case Moderate:
                printf("Moderate\n");

                hal_gpio_set_output(g_C_0, p_C_0);
                
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 1)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 20.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 2)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 40.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 3)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 60.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 4)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 80.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 5)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);
                
                for(;;)
                {
                    if(sys_parm.work_status == 0)
                        continue;
                    if(sys_parm.work_now_time == sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.2))
                    {
                        hal_gpio_clr_output(g_C_0, p_C_0);
                        hal_gpio_set_output(g_C_1, p_C_1);
                        hal_gpio_set_output(g_C_2, p_C_2);
                    }else if(sys_parm.work_now_time == sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.4)){
                        hal_gpio_clr_output(g_C_0, p_C_0);
                        hal_gpio_clr_output(g_C_1, p_C_1);
                        hal_gpio_set_output(g_C_2, p_C_2);
                    }else if(sys_parm.work_now_time <= sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.8)){
                        hal_gpio_clr_output(g_C_1, p_C_1);
                        hal_gpio_set_output(g_C_0, p_C_0);
                        hal_gpio_set_output(g_C_2, p_C_2);
                    }
                    rt_thread_delay(500);
                }

                break;
            case Deep:
                
                break;
            case Combination:
                
                break;
            case Pain_Relief:
                printf("Pain_Relief\n");

                hal_gpio_set_output(g_C_1, p_C_1);
                hal_gpio_set_output(g_C_2, p_C_2);
                rt_thread_delay(10);
                if(now_conf[sys_parm.work_mode]._intensity_level == 1)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 20.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 2)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 40.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 3)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 60.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 4)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 80.0);
                else if(now_conf[sys_parm.work_mode]._intensity_level == 5)
                    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 100.0);
                
                for(;;)
                {
                    if(sys_parm.work_status == 0)
                        continue;
                    if(sys_parm.work_now_time == sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.3))
                    {
                        hal_gpio_clr_output(g_C_1, p_C_1);
                        hal_gpio_set_output(g_C_0, p_C_0);
                        hal_gpio_set_output(g_C_2, p_C_2);
                    }else if(sys_parm.work_now_time == sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.5)){
                        hal_gpio_set_output(g_C_0, p_C_0);
                        hal_gpio_set_output(g_C_1, p_C_1);
                        hal_gpio_set_output(g_C_2, p_C_2);

                    }else if(sys_parm.work_now_time <= sys_parm.work_sum_time - (int)(sys_parm.work_sum_time * 0.7)){
                        int cnt = 0;
                        bool flag = true;
                        for(;;)
                        {
                            if(sys_parm.work_status == 0)
                                continue;
                            cnt++;
                            if(cnt % 10 == 0)
                            {
                                if(flag)
                                {
                                    flag = false;
                                    hal_gpio_clr_output(g_C_0, p_C_0);      //630
                                    hal_gpio_set_output(g_C_1, p_C_1);    //830
                                    hal_gpio_set_output(g_C_2, p_C_2);    //1050
                                }else{
                                    flag = true;
                                    hal_gpio_set_output(g_C_0, p_C_0);    //630
                                    hal_gpio_clr_output(g_C_1, p_C_1);      //1050
                                    hal_gpio_clr_output(g_C_2, p_C_2);      //830
                                }
                            }
                            rt_thread_delay(500);
                        }
                    }
                    rt_thread_delay(500);
                }
            default:
                break;
            }
            

        }

        if(event_recv & __USER_WORK_STOP_EVENT){
            printf("__USER_WORK_STOP_EVENT\n");
            //工作状态指示灯
            hal_gpio_set_output(g_E_12, p_E_12);

            pwm_set_duty(PWM_CH_0, PWN_SIG_A, 0);   //低电平有效
            rt_thread_delay(10);
            hal_gpio_clr_output(g_C_0, p_C_0);
            hal_gpio_clr_output(g_C_1, p_C_1);
            hal_gpio_clr_output(g_C_2, p_C_2);
            hal_gpio_clr_output(g_C_3, p_C_3);
        }
    }
}

void lv_qm_ui_entry(void)
{
    //背光B 蜂鸣器A
    pwm_config_t pwmConfig_1 = {
        .channel = PWM_CH_1,
        .freq = 2000, // 1 kHz
        .signal = {
            {.default_level = 0, .enable = 1, .duty = 100.0},    //A
            {.default_level = 0, .enable = 1, .duty = 100.0}      //B
        },
        .enable = 1
    };
    pwm_init_config(&pwmConfig_1);

    pwm_set_duty(PWM_CH_1, PWN_SIG_A, 0);   //蜂鸣器
    pwm_set_duty(PWM_CH_1, PWN_SIG_B, 50);   //背光

    //灯光PWM
    pwm_config_t pwmConfig_2 = {
        .channel = PWM_CH_0,
        .freq = 2000, // 200Hz
        .signal = {
            {.default_level = 1, .enable = 1, .duty = 0.0},    //A OUT_PWM
            {.default_level = 1, .enable = 1, .duty = 0.0}      //B
        },
        .enable = 1
    };
    pwm_init_config(&pwmConfig_2);

    pwm_set_duty(PWM_CH_0, PWN_SIG_A, 0.0);

    //工作状态指示灯
    pin = hal_gpio_name2pin("PE.12");    //
    g_E_12 = GPIO_GROUP(pin);
    p_E_12 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_E_12, p_E_12);
    hal_gpio_set_output(g_E_12, p_E_12);

    //系统状态指示灯
    pin = hal_gpio_name2pin("PE.13");    //
    g_E_13 = GPIO_GROUP(pin);
    p_E_13 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_E_13, p_E_13);
    hal_gpio_clr_output(g_E_13, p_E_13);




    pin = hal_gpio_name2pin("PC.0");    //红灯  630
    g_C_0 = GPIO_GROUP(pin);
    p_C_0 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_C_0, p_C_0);
    hal_gpio_clr_output(g_C_0, p_C_0);

    pin = hal_gpio_name2pin("PC.1");   //红外灯 1050
    g_C_1 = GPIO_GROUP(pin);
    p_C_1 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_C_1, p_C_1);
    hal_gpio_clr_output(g_C_1, p_C_1);

    pin = hal_gpio_name2pin("PC.2");   //红外灯 830
    g_C_2 = GPIO_GROUP(pin);
    p_C_2 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_C_2, p_C_2);
    hal_gpio_clr_output(g_C_2, p_C_2);

    pin = hal_gpio_name2pin("PC.3");   //黄灯 590
    g_C_3 = GPIO_GROUP(pin);
    p_C_3 = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_output(g_C_3, p_C_3);
    hal_gpio_clr_output(g_C_3, p_C_3);

    gpai_dev = (rt_adc_device_t)rt_device_find(AIC_GPAI_NAME);
    if (!gpai_dev) {
        rt_kprintf("Failed to open %s device\n", AIC_GPAI_NAME);
        return;
    }
    rt_adc_enable(gpai_dev, 4);

    __user_event = rt_event_create("__user_event", RT_IPC_FLAG_FIFO);
    if (__user_event == RT_NULL) {
        rt_kprintf("create __user_event event failed.\n");
    }

    rt_thread_t thread = rt_thread_create("__check_work_task", __check_work_task, RT_NULL, 1024*2, 30, 10);
    if(thread != RT_NULL)
        rt_thread_startup(thread);
    else
        rt_kprintf("__check_work_task create faile\n");
    
    rt_thread_t thread_tp = rt_thread_create("__check_tp_task", __check_tp_task, RT_NULL, 1024, 30, 10);
    if(thread_tp != RT_NULL)
        rt_thread_startup(thread_tp);
    else
        rt_kprintf("__check_tp_task create faile\n");

    top_layer_create();
    home_scr_create();          //0
    item0n5_scr_create();       //1

    item4_scr_create();
    item4t1_info_scr_create();
    item4t1_info_qr_scr_create();
    item4t1_lcd_brightness_scr_create();
    item4t1_sound_scr_create();
    item4t1_wifi_scr_create();
    item4t1_reset_scr_create();

    lv_img_cache_set_size(1);
    bootlogo_init();    // 开机logo -> 1s后跳转到主界面 bootlogo_del

    black_mask = lv_obj_create(lv_layer_top());
    lv_obj_set_size(black_mask, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(black_mask, lv_color_black(), 0);
    lv_obj_set_style_radius(black_mask, 0, 0);
    lv_obj_set_style_border_width(black_mask, 0, 0);
    lv_obj_add_flag(black_mask, LV_OBJ_FLAG_HIDDEN);

#ifdef VIRTUAL_KEYBORAD
    virtual_keyborad();
#endif

//按键
extern void zx_button_start_gpio(void);
    zx_button_start_gpio();

#if 1


    

    
    

	lv_timer_t *t1 = lv_timer_create(user_task, 3000, NULL);
    lv_timer_t *t2 = lv_timer_create(scr_refr_task, 100, NULL); // 100ms 读取系统参数并自动刷新界面 - 目前仅用于实现电量图标刷新， 可在此处实现按键事件接收并传递

    // lv_timer_t *t3 = lv_timer_create(work_task, 1000, NULL); // 100ms 读取系统参数并自动刷新界面 - 目前仅用于实现电量图标刷新， 可在此处实现按键事件接收并传递

#endif

}

