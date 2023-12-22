#include <rtthread.h>
#include <rtdevice.h>
#include <aic_hal_gpio.h>
#include <aic_drv_gpio.h>
#include <string.h>
#include <rtdbg.h>
#include <stdio.h>

#define PWM_DEV_NAME        "pwm"  /* name of PWM device */
#define PWM_DEV_CHANNEL 3

#define BACKLIGHT_PIN "PE.19"
#define BACKLIGHT_ON    1
#define BACKLIGHT_OFF    0

static struct rt_device_pwm *pwm_dev;

static rt_uint32_t g_period = 5000;
static int g_pulse = 2500;
void backlight_set(int val);

void set_backlight_pulse(int pulse)
{
    printf("pulse:%d\n", pulse);
    printf("pulse:%d\n", 5000 * pulse / 100);
    g_pulse = 5000 * pulse / 100;
    backlight_set(1);
}

int get_g_pulse(void)
{
    return g_pulse;
}

void backlight_set(int val)
{
#if 1
	rt_uint32_t period, pulse;

	period = g_period;

	if (val) {
		pulse = g_pulse;
        if (pwm_dev) {    
            /* Set the PWM period and pulse width */
            rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);

            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
        }
	} else {
        pulse = 0;
        if (pwm_dev) {    
            /* Set the PWM period and pulse width */
            rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);

            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
            rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);
        }
	}
#endif
}

void backlight_init(void)
{
#if 1
    printf("backlight_init\n");
    rt_uint32_t period, pulse;

    period = 5000;    /* The period is 2.5us, the unit is nanoseconds 5us */
    pulse = 2500;          /* PWM pulse width value, the unit is nanoseconds 1500*/
    /* Search the device */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);

    if (pwm_dev) {
        /* Set the PWM period and pulse width */
        rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);

        rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    } else {
        rt_kprintf("pwm find error\n");
    }
#endif
}
