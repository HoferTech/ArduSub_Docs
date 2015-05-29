
#ifndef TIMER_H
#define TIMER_H
#include <compiler.h>
#define MAX_TIMERS	1

#define TIMER_HEARTBEAT	0


void timer_start(U8);
void timer_stop(U8);
void timer_reset(U8);
U8 timer_is_expired(U8);
void timer_set_counts(U8,U16);
U16 timer_ms_to_counts(U16);
void timer_init(U16);
#endif