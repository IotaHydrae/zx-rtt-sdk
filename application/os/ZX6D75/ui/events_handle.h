#ifndef _EVENTS_HANDLE_H_
#define _EVENTS_HANDLE_H_

#include "bee_anim.h"


void screen_event_handler(lv_event_t *e);

void screen_speed_timer_cb(lv_timer_t* t);
void screen_rand_timer_cb(lv_timer_t* t);
void screen_gif_timer_cb(lv_timer_t* t);

#endif
