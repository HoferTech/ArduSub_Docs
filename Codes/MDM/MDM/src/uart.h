#ifndef UART_H
#define UART_H
#include "compiler.h"
void uart_init(U16 baud);
void uart_write(U8 *buffer_ptr,U16 size_bytes);
void uart_read(U8 *buffer_ptr,U16 size_bytes);
U16 uart_tx_count(void);
U16 uart_rx_count(void);
void uart_rx_reset(void);
#endif