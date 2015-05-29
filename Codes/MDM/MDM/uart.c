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
#include "usart.h"
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
#define UART_MAX_TX_BUFFER_BYTES	256
#define UART_MAX_RX_BUFFER_BYTES	256
/***************************************************/
/*  Function Prototype Section
* Add prototypes for all functions called by this
* module, with the exception of runtime routines.
*
***************************************************/

U8 tx_buffer[MAX_UART][UART_MAX_TX_BUFFER_BYTES];
U8 rx_buffer[MAX_UART][UART_MAX_RX_BUFFER_BYTES];
  static usart_rs232_options_t USART_SERIAL_OPTIONS = {
	  .baudrate = USART_SERIAL_BAUDRATE,
	  .charlength = USART_SERIAL_CHAR_LENGTH,
	  .paritytype = USART_SERIAL_PARITY,
	  .stopbits = USART_SERIAL_STOP_BIT
  };

void uart_init(U16 baud_rate)
{
   sysclk_enable_module(SYSCLK_PORT_D, PR_USART0_bm);
   usart_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);


}

void uart_write(U8 *buffer_ptr,U16 size_bytes)
{
	
}

void uart_read(U8 buffer_ptr,U16 size_bytes)
{
	

}