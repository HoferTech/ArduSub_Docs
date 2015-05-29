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
/*  Include section
* Add all #includes here
*
***************************************************/
#include "pwm.h"
#include "thruster.h"
#include "ioport.h"
/***************************************************/
/*  Defines section
* Add all #defines here
*
***************************************************/
struct pwm_config pwm_cfg[6];
ioport_pin_t direction_pins[6] = {DIRA,DIRB,DIRC,DIRD,DIRE,DIRF};
U8 thruster_per_sp[6];
/***************************************************/
/*  Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/

void thruster_init(void)
{
	//config pwm channels
	pwm_init(&pwm_cfg[0],PWM_TCF0,PWM_CH_A,20000);
	pwm_init(&pwm_cfg[1],PWM_TCF0,PWM_CH_B,20000);
	pwm_init(&pwm_cfg[2],PWM_TCF0,PWM_CH_C,20000);
	pwm_init(&pwm_cfg[3],PWM_TCF0,PWM_CH_D,20000);
	pwm_init(&pwm_cfg[4],PWM_TCD0,PWM_CH_A,20000);
	pwm_init(&pwm_cfg[5],PWM_TCD0,PWM_CH_B,20000);
	//set duty cycles
	pwm_start(&pwm_cfg[0],0);
	pwm_start(&pwm_cfg[1],0);
	pwm_start(&pwm_cfg[2],0);
	pwm_start(&pwm_cfg[3],0);
	pwm_start(&pwm_cfg[4],0);
	pwm_start(&pwm_cfg[5],0);
	//set io port for direction
	ioport_set_pin_dir(DIRA,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(DIRB,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(DIRC,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(DIRD,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(DIRE,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(DIRF,IOPORT_DIR_OUTPUT);
	//set setpoints to 0
	thruster_per_sp[0] = 0;
	thruster_per_sp[1] = 0;
	thruster_per_sp[2] = 0;
	thruster_per_sp[3] = 0;
	thruster_per_sp[4] = 0;
	thruster_per_sp[5] = 0;
	
}

void thruster_set(U8 index,U8 percentage,U8 direction)
{
	thruster_per_sp[index] = percentage;
	pwm_set_duty_cycle_percent(&pwm_cfg[index],percentage);
	ioport_set_pin_level(direction_pins[index],direction);
}

void thruster_enable(U8 index)
{
	pwm_set_duty_cycle_percent(&pwm_cfg[index],thruster_per_sp[index]);
}

void thruster_disable(U8 index)
{
	pwm_set_duty_cycle_percent(&pwm_cfg[index],0);
}

void thruster_dir_set(U8 index,U8 forward)
{
	ioport_set_pin_level(direction_pins[index],forward);
}

U16 thruster_per_get(U8 index)
{
	return thruster_per_sp[index];
}