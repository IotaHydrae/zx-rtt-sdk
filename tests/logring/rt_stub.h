/* 最小桩：只为在主机上编译真实环代码，不是移植层。 */
#ifndef RT_STUB_H
#define RT_STUB_H
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
typedef uint32_t rt_uint32_t; typedef uint16_t rt_uint16_t; typedef uint8_t rt_uint8_t;
typedef int32_t rt_base_t; typedef size_t rt_size_t; typedef int32_t rt_off_t;
typedef int rt_err_t; typedef int rt_bool_t; typedef struct rt_device *rt_device_t;
#define RT_EOK 0
#define RT_FALSE 0
#define RT_TRUE 1
#define RT_Device_Class_Char 1
#define RT_DEVICE_FLAG_RDWR 3
#define ULOG_LINE_BUF_SIZE 128
#define rt_memcpy memcpy
#define rt_memset memset
#define rt_strlen strlen
#define rt_strcmp strcmp
#define rt_snprintf snprintf
#define rt_hw_interrupt_disable() ((rt_base_t)0)
#define rt_hw_interrupt_enable(x) ((void)(x))
#define INIT_PREV_EXPORT(f)  static void __attribute__((unused)) __init_##f(void){}
#define INIT_BOARD_EXPORT(f) static void __attribute__((unused)) __initb_##f(void){}
#define INIT_APP_EXPORT(f)   static void __attribute__((unused)) __inita_##f(void){}
struct ulog_backend {
    void (*output)(struct ulog_backend *, rt_uint32_t, const char *,
                   rt_bool_t, const char *, rt_size_t);
};
struct rt_device_ops { rt_err_t (*open)(rt_device_t, rt_uint16_t);
                       rt_size_t (*write)(rt_device_t, rt_off_t, const void *, rt_size_t); };
struct rt_object { char name[16]; };
struct rt_device { struct rt_object parent; int type; const struct rt_device_ops *ops; };
static inline rt_err_t rt_device_register(rt_device_t d, const char *n, int f){(void)d;(void)n;(void)f;return 0;}
static inline rt_device_t rt_console_get_device(void){return 0;}
static inline rt_device_t rt_console_set_device(const char *n){(void)n;return 0;}
static inline void rt_hw_console_output(const char *s){(void)s;}
#undef MSH_CMD_EXPORT
#define MSH_CMD_EXPORT(name, desc) static void __attribute__((unused)) __msh_##name(void){}
static inline int rt_kprintf(const char *f, ...){(void)f;return 0;}
static inline void ulog_backend_register(struct ulog_backend *b, const char *n, rt_bool_t e){(void)b;(void)n;(void)e;}
#endif
