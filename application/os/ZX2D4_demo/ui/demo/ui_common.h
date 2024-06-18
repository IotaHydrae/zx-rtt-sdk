#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include <rtdevice.h>

/**********************
 *      TYPEDEFS
 **********************/

/*********************************************************************
 *                                  NOTE
 * 
 * 1.   光疗仪设备总共有7种自定义输出策略，配置可能需要上电读取、开始工作后保存
 * 2.   工作逻辑
 *      1) 设备pwm输出策略
 *          a. 轮询读取sys_parm.work_status判定是否开始pwm输出
 *          b. pwm输出策略 通过sys_parm.work_mode从dev_conf[]配置列表中索引读取，两者关联枚举类型E_MODE_ID
 *      2) 工作开始及暂停回调，可以此处开启关闭pwm输出
 *          a. static void work_start_exec(void);
 *          b. static void work_stop_exec(void);
 *          c. item0n2_scr.c | item3_scr.c  // 关联源文件 函数定位
 *      3) 设备信息保存回调
 *          a. static void save_dev_conf(void);
 *          b. 需要记忆的变量：dev_conf[E_USER_1 - E_USER_5]
 *          c. item4t1_scr.c // 关联源文件 函数定位
 *      4) 电量刷新
 *          a. sys_parm.charge_level 赋值
 *          b. 刷新入口 scr_refr_task -> scr_refr_func(NULL); // 默认100ms
 *      5) 按键事件处理
 *          a. 参考 VIRTUAL_KEYBORAD 宏定义注释内容，创建Event_Data_t类型的变量，赋值后使用scr_refr_func(&var)传递
 *          eg. 
 *              xReceivedEvent.eEventID = E_KEY_EVENT;
 *              xReceivedEvent.lDataArray[0] = E_K1;
 *              xReceivedEvent.lDataArray[1] = E_S_CLICKED;
 *              scr_refr_func(&xReceivedEvent); // 处理按键K1的短按事件
 *      6) 对照图文件夹下有E_SCREEN_ID屏幕枚举 及 各个图标 设计图
 *      7) 函数入口 lv_qm_ui_entry
 *********************************************************************/
#ifdef LPKG_USING_RAMDISK
#define LVGL_DIR "L:/ram/"
#else
#define LVGL_DIR "L:/rodata/"
#endif

#define __DEBUG    //日志模块总开关，注释将关闭全局日志输出

#ifdef __DEBUG
    #define APP_DEBUG(format, ...) printf ("\033[41;33m"format"\033[0m\n", ##__VA_ARGS__)
#else
    #define APP_DEBUG(format, ...)
#endif

#define ASSET_PATH   "D:\\shared\\assets\\mask\\assetsV2\\"

typedef enum
{
    E_KEY_EVENT = 0,
    E_EVENT_ID_MAX,
} Event_ID_t;

typedef enum
{
    E_K1 = 0,   // 循环切换
    E_K2,       // 强度循环/往左
    E_K3,       // 时间循环/往右
    E_K4,       // 开机/启动/停止
} BTN_ID_t;

typedef enum
{
    E_S_EBUTTON_PRESS_DOWN = 0,
    E_S_BUTTON_PRESS_UP,
    E_S_BUTTON_PRESS_REPEAT,
    E_S_BUTTON_SINGLE_CLICK,
    E_S_BUTTON_DOUBLE_CLICK,
    E_S_BUTTON_LONG_PRESS_START,
    E_S_BUTTON_LONG_PRESS_HOLD,
    E_S_BUTTON_EVENT_MAX,
    E_S_BUTTON_NONE_PRESS
} BTN_STATUS_t;

typedef struct {
    Event_ID_t eEventID;    // 事件类型
    uint32_t lDataArray[2]; // lDataArray[0]-按键编号 lDataArray[1]-按键事件类型(长短按)
} Event_Data_t, *p_Event_Data_t;

typedef struct _SCR_FUNC_TYPE
{
    void (*load_func)(void *args);
    void (*refr_func)(Event_Data_t *args);  // NULL - 定时类显示刷新， 数值波动较大的, 不为NULL - 实时类显示刷新
    void (*quit_func)(void *args);
} SCR_FUNC_TYPE, *p_SCR_FUNC_TYPE;

typedef enum {
    E_HOME_SCR = 0,
    E_ITEM0N5_SCR,
    E_ITEM4_SCR,

    E_ITEM4_INFO_SCR,
    E_ITEM4_INFO_QR_SCR,
    E_ITEM4_LCD_BRIGHTNESS_SCR,
    E_ITEM4_SOUND_SCR,
    E_ITEM4_WIFI_SCR,
    E_ITEM4_RESET_SCR,

    E_TOP_LAYER,
    E_MAX_SCR,
} E_SCREEN_ID;

typedef enum {
    E_Light_Therapy = 0,    // home_scr第0项    Light Therapy
    E_BeautySkincare,       // home_scr第1项    Beauty&Skincare
    E_Pain_Relief,          // home_scr第2项    Pain Relief         // 1-2-3-4-5
    E_Wound_Healing,        // home_scr第3项    Wound Healing
    E_Hair_Generating,      // home_scr第4项    Hair Generating     // 1-2-3-4-5


    E_MODE_MAX,  // 未进入菜单项子页面
} E_MODE_ID;

typedef enum {
    Infrared = 0,
    Red,
    Infrared_Red,
    Dual_Infrared,
    Dualinfra_Red,
    Infrared_Yellow,


    Moderate,                               //6
    Deep,                                   //7
    Combination,                            //8

    Pain_Relief,
    // Hair_Generating,


    E_LIGHT_MAX,  // 未进入菜单项子页面
} E_LIGHT_TYPE;

typedef struct
{
    unsigned int _830_intensity;
    unsigned int _633_intensity;
    unsigned int _1050_intensity;
    unsigned int _590_intensity;
    
    unsigned int _intensity_level;
    unsigned int _time;
    // double _energy; //能量 = 强度 * 剩余时间（秒）/ 1000
} DEVICE_BASE_CONF;  // 模式基础配置

typedef struct _SYS_PARM_TYPE{
    E_LIGHT_TYPE work_mode;    // 模式 索引输出配置
    int work_status;    // 状态 0 - 停止 1 - 工作
    volatile int work_sum_time;    //
    volatile int work_now_time;    //
    int charge_level;    // 电池电量 0-充电 (1-7 => 100%-0%)
} SYS_PARM_TYPE, *p_SYS_PARM_TYPE;

typedef struct _SCR_LIST_TYPE{
    lv_obj_t *scr;
    SCR_FUNC_TYPE *user_data;
} SCR_LIST_TYPE, *p_SCR_LIST_TYPE;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void scr_add_user_data(lv_obj_t *scr, E_SCREEN_ID id, SCR_FUNC_TYPE *desc);
void scr_load_func(E_SCREEN_ID id, void *args);
void scr_quit_func(void *args);
void scr_refr_func(Event_Data_t *args);
E_SCREEN_ID scr_act_get(void);

void click_load_scr_event_cb(lv_event_t * e);

void refr_double_data_to_label(lv_obj_t *label, double data);
double calc_conf_energy(E_LIGHT_TYPE id, int dym_time);

void hint_cont_create(void);    // 弹窗，显示Low battery， Please charge it
void hint_cont_delete(void);
void hint_cont2_create(void);
void hint_cont2_delete(void);

extern SYS_PARM_TYPE sys_parm;
extern E_SCREEN_ID g_scr_id[E_MAX_SCR];
// extern const char *title_text[];
extern const char *title_mode_text[];
extern const char *light_type_text[];
extern DEVICE_BASE_CONF dev_conf[E_LIGHT_MAX];
extern DEVICE_BASE_CONF now_conf[E_LIGHT_MAX];
/*********************
 *      DEFINES
 *********************/




LV_FONT_DECLARE(montserrat_x20b);
LV_FONT_DECLARE(montserrat_x15seb);
LV_FONT_DECLARE(montserrat_x18m);

LV_FONT_DECLARE(montserrat_15);
LV_FONT_DECLARE(montserrat_el_14);
LV_FONT_DECLARE(montserrat_el_16);
LV_FONT_DECLARE(montserrat_el_17);
LV_FONT_DECLARE(montserrat_el_18);
LV_FONT_DECLARE(montserrat_el_19);
LV_FONT_DECLARE(montserrat_el_22);
LV_FONT_DECLARE(montserrat_el_23);


lv_obj_t *new_scr_base_cont1_create(lv_obj_t *par, const char *text1);
lv_obj_t *new_scr_base_cont2_create(lv_obj_t *par, const char *text1);

extern void top_layer_create(void);
extern void home_scr_create(void);
extern void item0n5_scr_create(void);
extern void item4_scr_create(void);
extern void item4t1_info_scr_create(void);
extern void item4t1_info_qr_scr_create(void);
extern void item4t1_lcd_brightness_scr_create(void);
extern void item4t1_sound_scr_create(void);
extern void item4t1_wifi_scr_create(void);
extern void item4t1_reset_scr_create(void);



extern rt_event_t __user_event;
#define __USER_WORK_START_EVENT     0x01
#define __USER_WORK_STOP_EVENT      0x02

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_UI_COMMON_H_*/
