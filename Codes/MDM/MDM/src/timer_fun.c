/*
Copyright (C) 2014 Chris Hofer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***************************************************/
#include <compiler.h>
#include "tc.h"
#include "timer_fun.h"

//100us tic rate 10 * 100us = 1ms
#define TIMER_MS_TO_COUNTS 10

#define TIMER_FLAGS_ENABLE	0x01
#define TIMER_FLAGS_EXPIRED	0x02

typedef struct  
{
	U16 flags;
	U16 counts;
	U16 setpoint;
}t_timer;

t_timer timer[MAX_TIMERS];
static void timer_isr(void)
{
	volatile U8 i;
	for (i = 0;i<MAX_TIMERS;i++)
	{
		if(Tst_bits(timer[i].flags,TIMER_FLAGS_ENABLE))
		{
			if(timer[i].counts)
			{
				timer[i].counts--;
			}
			if(timer[i].counts == 0)
			{
				Set_bits(timer[i].flags,TIMER_FLAGS_EXPIRED);
			}
		}
	}
}

void timer_init(U16 rate_Hz)
{
	U8 i;
	for (i=0;i<MAX_TIMERS;i++)
	{
		timer[i].flags = 0;
		timer[i].counts = 0;
		timer[i].setpoint = 0;
	}
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0, timer_isr);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, BOARD_XOSC_HZ/rate_Hz);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);

}


void timer_start(U8 timer_index)
{
	Set_bits(timer[timer_index].flags,TIMER_FLAGS_ENABLE);
}

void timer_stop(U8 timer_index)
{
	Clr_bits(timer[timer_index].flags,TIMER_FLAGS_ENABLE);
}

void timer_reset(U8 timer_index)
{
	timer[timer_index].counts = timer[timer_index].setpoint;
	Clr_bits(timer[timer_index].flags,TIMER_FLAGS_EXPIRED);
}

U8 timer_is_expired(U8 timer_index)
{
	return Tst_bits(timer[timer_index].flags,TIMER_FLAGS_EXPIRED);
}

void timer_set_counts(U8 timer_index,U16 counts)
{
	timer[timer_index].setpoint = counts;
	timer[timer_index].counts = counts;
}
	
U16 timer_ms_to_counts(U16 time_ms)
{
	return time_ms * TIMER_MS_TO_COUNTS;
}


