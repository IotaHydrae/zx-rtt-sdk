/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#include "events_handle.h"
#include "bee_anim.h"

void events_init(lv_ui *ui)
{

}

void screen_event_handle_add(lv_ui* bee){
    lv_obj_add_event_cb(bee->screen, screen_event_handler, LV_EVENT_ALL, bee);

    lv_timer_create(screen_speed_timer_cb, 1000, bee);
    lv_timer_create(screen_rand_timer_cb, 1000, bee);
    lv_timer_create(screen_gif_timer_cb, 50, bee);

    screen_obj_circulate_anim(bee->screen_label_7, 16000, 120, 40, screen_speed_text_anim_cb);
    screen_obj_anim(bee->screen_label_5, 20000, 500, 0, screen_speed_battery_text_anim_cb);
    screen_obj_circulate_anim(bee->screen_bar_1, 16000, 100, 0, screen_speed_bar_1_anim_cb);
    screen_obj_anim(bee->screen_bar_2, 20000, 100, 0, screen_speed_bar_2_anim_cb);
}
