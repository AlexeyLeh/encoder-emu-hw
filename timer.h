/**
* Created: 12.01.2026
* Author: Lekhtin A.V.
*/

#ifndef INTERRUPT__H_
#define INTERRUPT__H_

#include <stdint.h>
#include "hw.h"

extern uint16_t g_timer_freq;
extern int8_t   g_timer_fullpass;
extern uint16_t g_timer_ticks;
extern int32_t  g_timer_clicks_counter;

void setup_timers(void);
void set_timer_settings(int32_t freq, int32_t clicks, uint8_t direction);

uint8_t start_encoder(void);
void stop_encoder(void);

void timer_set_done_message(const char *msg);

void stop_blink_led(void);

void start_idle_led(void);
void stop_idle_led(void);

void start_ack_led(void);
void stop_ack_led(void);


#endif // INTERRUPT__H_
