/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include "timer_fun.h"
#include "ioport.h"
#include "message_handler.h"
#include "thruster.h"
#include "uart.h"
#include "diag.h"


#define GP_TIMER	0


int main (void)
{
	U8 msg[32] = "Hi ArduSub\n\r";
	pmic_init();
	sysclk_init();
	
	adc_init();
	thruster_init();
	ioport_init();
	timer_init(10000);
	timer_set_counts(TIMER_HEARTBEAT,timer_ms_to_counts(250));
	timer_start(TIMER_HEARTBEAT);
	cpu_irq_enable();

	volatile U8 count = 0;
	ioport_set_pin_dir(LED1,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED2,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(UARTD_TX,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(UARTD_RX,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(TXEN,IOPORT_DIR_OUTPUT);
	board_init();
	uart_init(19200);
	do 
	{
		message_tasks();

		count++;
		if (count > 52)
		{
			
			count  = 0;
			
		}
		if(timer_is_expired(TIMER_HEARTBEAT))
		{
			timer_reset(TIMER_HEARTBEAT);
			board_pres_read();
			ext_temp_read();
			//ioport_set_pin_level(TXEN,1);
			//uart_write(&msg[0],12);
			//while(uart_tx_count() != 0);
			//blink heart beat LED
			ioport_set_pin_level(LED2,!ioport_get_pin_level(LED2));
		}
	} while (1);

	// Insert application code here, after the board has been initialized.
}
