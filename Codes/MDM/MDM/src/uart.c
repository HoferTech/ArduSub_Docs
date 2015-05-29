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
#include <asf.h>
#include <string.h>
#include "usart.h"
#include "uart.h"
/***************************************************/
/*  Defines section
* Add all #defines here
*
***************************************************/
    #define USART_SERIAL                     &USARTD0
    #define USART_SERIAL_BAUDRATE            9600
    #define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
    #define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
    #define USART_SERIAL_STOP_BIT            false



#define MAX_UART	1
#define UART_MAX_TX_BUFFER_BYTES	255
#define UART_MAX_RX_BUFFER_BYTES	2048
/***************************************************/
/*  Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/

U16 tx_buffer_size;
U16 tx_write_index;
U16 tx_read_index;
U8 last_byte = 0;

U16 rx_write_index;
U16 rx_read_index;
U16 rx_buffer_size;

static U8 tx_buffer[UART_MAX_TX_BUFFER_BYTES];
static U8 rx_buffer[UART_MAX_RX_BUFFER_BYTES];
  static usart_rs232_options_t USART_SERIAL_OPTIONS = {
	  .baudrate = USART_SERIAL_BAUDRATE,
	  .charlength = USART_SERIAL_CHAR_LENGTH,
	  .paritytype = USART_SERIAL_PARITY,
	  .stopbits = USART_SERIAL_STOP_BIT
  };

ISR(USARTD0_RXC_vect)
{
	asm(" nop");
	rx_buffer[rx_write_index] = USARTD0.DATA;
	rx_write_index++;
	if (rx_write_index >= UART_MAX_RX_BUFFER_BYTES)
	{
		rx_write_index = 0;
	}
	rx_buffer_size++;
}

ISR(USARTD0_TXC_vect)
{
	ioport_set_pin_level(TXEN,0);
	//usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_LO);

}

ISR(USARTD0_DRE_vect)
{
	if (tx_read_index != tx_write_index)
	{
		USARTD0.DATA = tx_buffer[tx_read_index];
		tx_read_index++;
		if (tx_read_index >= UART_MAX_TX_BUFFER_BYTES)
		{
			tx_read_index = 0;
		}
		tx_buffer_size--;
		if (tx_read_index == tx_write_index)
		{
			usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
		}
	}
}

void uart_init(U16 baud_rate)
{
	USART_SERIAL_OPTIONS.baudrate = baud_rate;
	sysclk_enable_module(SYSCLK_PORT_D, PR_USART0_bm);
   

	usart_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);
	usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_LO);
	usart_set_tx_interrupt_level(USART_SERIAL,USART_INT_LVL_LO);
	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_HI);
	//rest buffers
	tx_buffer_size = 0;
	tx_write_index = 0;
	tx_read_index = 0;
	
	rx_buffer_size = 0;
	rx_write_index = 0;
	rx_read_index = 0;
}




void uart_write(U8 *buffer_ptr,U16 size_bytes)
{
	U16 i;
	//disable tx interrupt
	usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
	for (i = 0;i<size_bytes;i++)
	{
		tx_buffer[tx_write_index] = *buffer_ptr++;
		tx_write_index++;
		//wrap buffer index
		tx_write_index %= UART_MAX_TX_BUFFER_BYTES;
		tx_buffer_size++;
	}
	//enable interrupts
	usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_LO);
}

void uart_read(U8 *buffer_ptr,U16 size_bytes)
{
	U16 i;
	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
	for (i = 0;i < size_bytes;i++)
	{
		*buffer_ptr++ = rx_buffer[rx_read_index];
		rx_read_index++;
		//wrap buffer index
		rx_read_index %= UART_MAX_RX_BUFFER_BYTES;
		rx_buffer_size--;
	}
	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_HI);
}

U16 uart_tx_count(void)
{
	U16 count;
	//usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
	count = tx_buffer_size;
	//usart_set_dre_interrupt_level(USART_SERIAL,USART_INT_LVL_LO);
	return count;
}

U16 uart_rx_count(void)
{
	
	U16 count;
//	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
	count = rx_buffer_size;
//	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_HI);
	return count;
}

void uart_rx_reset(void)
{
	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_OFF);
	rx_buffer_size = 0;
	rx_read_index = 0;
	rx_write_index = 0;
	usart_set_rx_interrupt_level(USART_SERIAL,USART_INT_LVL_HI);
}