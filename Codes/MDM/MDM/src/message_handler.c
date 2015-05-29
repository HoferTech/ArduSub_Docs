#include "uart.h"
#include "thruster.h"
#include "util.h"
#include "diag.h"
#include "string.h"
#include "ioport.h"
#include "conf_board.h"

#define MSG_ID_THRUSTER_SET_PER 0x01
#define MSG_ID_THRUSTER_SET_DIR 0x02
#define MSG_ID_THRUSTER_ENABLE	0x03
#define MSG_ID_THRUSTER_DISABLE 0x04
#define MSG_ID_DIAG_GET			0x05
#define MSG_CAL_PRES			0x06


typedef enum
{
	MSG_TASK_INIT,
	MSG_TASK_WAIT_SYNC1,
	MSG_TASK_WAIT_SYNC2,
	MSG_TASK_WAIT_ADDR,
	MSG_TASK_WAIT_ID,
	MSG_TASK_WAIT_SIZE,
	MSG_TASK_WAIT_DATA,
	MSG_TASK_CHECKSUM,
	MSG_TASK_VALID,
	MSG_TASK_ERROR	
}e_message_task_state;

e_message_task_state msg_task_state = MSG_TASK_INIT;

#define SYNC_BYTE_1 0x1F
#define SYNC_BYTE_2 0xF1
#define MAX_MSG_DATA_IN_BUFF_SIZE 50
#define MAX_MSG_RSP_BUFF_SIZE 100
U8 msg_data_in_buffer[MAX_MSG_DATA_IN_BUFF_SIZE];
U8 msg_rsp_buffer[MAX_MSG_RSP_BUFF_SIZE];


U16 calc_word_checksum(U8* buffer,U16 size_bytes)
{
	U16 i;
	U16 checksum = 0;
	for (i = 0;i<size_bytes;i++)
	{
		checksum += *buffer++;
	}
	return checksum;
}

void build_msg(U8 *msg_buffer,U16 addr,U16 ID,U8 *msg_header,U16 header_size,U8 *msg_data, U16 data_size)
{
	U16 *word_ptr;
	U8 byte;
	U16 msg_size;
	U16 checksum = 0;
	U8 *msg_start_ptr;
	//grab start of buffer for uart write
	msg_start_ptr = msg_buffer;
	*msg_buffer++ = SYNC_BYTE_1;
	*msg_buffer++ = SYNC_BYTE_2;
	msg_size = header_size + data_size;
	//add address
	word_ptr = msg_buffer;
	*word_ptr++ = addr;
	//add ID
	*word_ptr++ = ID;
	//add size
	*word_ptr++ = msg_size;
	msg_buffer += 6;
	//copy in header
	memcpy(msg_buffer,msg_header,header_size);
	msg_buffer += header_size;
	//copy data
	memcpy(msg_buffer,msg_data,data_size);
	msg_buffer += data_size;
	//calculate checksums
	checksum = calc_word_checksum((U8*)&addr,2);
	checksum += calc_word_checksum((U8*)&ID,2);
	checksum += calc_word_checksum((U8*)&msg_size,2);
	checksum += calc_word_checksum((U8*)msg_header,header_size);
	checksum += calc_word_checksum((U8*)msg_data,data_size);
	//add checksum
	word_ptr = msg_buffer;
	*word_ptr = checksum;
	//send message on uart
	//total byte is data size + headers
	//turn on 485 drive
	ioport_set_pin_level(TXEN,1);
	uart_write(msg_start_ptr,msg_size + 10);
	//wait for tx to be done
//	while(uart_tx_count());
//	ioport_set_pin_level(TXEN,0);
	
}

void msg_process(U16 addr,U16 ID,U16 data_size_bytes,U8 *data)
{
	U16 thrust_index;
	U16 thrust_per;
	U16 *word_ptr;
	t_block block;
	word_ptr = (U8*)data;
	switch (ID)
	{
		case MSG_ID_THRUSTER_SET_PER:
			thrust_index = word_ptr[0];
			thrust_per = word_ptr[1];
			thruster_set(thrust_index,thrust_per,word_ptr[2]);
		break;
		case MSG_ID_THRUSTER_SET_DIR:
			thruster_dir_set(word_ptr[0],word_ptr[1]);
		break;
		case MSG_ID_THRUSTER_ENABLE:
			thruster_enable(word_ptr[0]);
		break;
		case MSG_ID_THRUSTER_DISABLE:
			thruster_disable(word_ptr[0]);
		break;
		case MSG_ID_DIAG_GET:
			diag_update();
			//get pointer to data
			block = diag_struct_ptr_get();
			//send message out on wire
			build_msg(msg_rsp_buffer,addr,ID,NULL,0,block.data_ptr,block.data_size_bytes);
		break;
		case MSG_CAL_PRES:
			calibrate_pressure((float*)&word_ptr[0]);
		break;
		default:
		break;
	}
}



void message_tasks(void)
{
	U8 data_byte;
	static U16 ID;
	static U16 size;
	static U16 addr;
	static U16 msg_checksum;
	static U16 calc_checksum;
	switch(msg_task_state)
	{
		case MSG_TASK_INIT:
			msg_task_state = MSG_TASK_WAIT_SYNC1;
		break;
		case MSG_TASK_WAIT_SYNC1:
			if (uart_rx_count())
			{
				uart_read(&data_byte,1);
				if (data_byte == SYNC_BYTE_1)
				{
					msg_task_state = MSG_TASK_WAIT_SYNC2;
				}
				else
				{
					asm(" NOP");
				}
			}
		break;
		case MSG_TASK_WAIT_SYNC2:
			if (uart_rx_count())
			{
				uart_read(&data_byte,1);
				if (data_byte == SYNC_BYTE_2)
				{
					msg_task_state = MSG_TASK_WAIT_ADDR;
				}
				else
				{
					msg_task_state = MSG_TASK_WAIT_SYNC1;
				}
			}
		break;
		case MSG_TASK_WAIT_ADDR:
			if (uart_rx_count() >= 2)
			{
				uart_read((U8*)&addr,2);
				msg_task_state = MSG_TASK_WAIT_ID;
			}
		case MSG_TASK_WAIT_ID:
			if (uart_rx_count() >= 2)
			{
				uart_read((U8*)&ID,2);
				msg_task_state = MSG_TASK_WAIT_SIZE;
			}
		break;
		case MSG_TASK_WAIT_SIZE:
			if (uart_rx_count() >= 2)
			{
				uart_read((U8*)&size,2);
				//no data so start looking for checksum
				if (size == 0)
				{
					msg_task_state = MSG_TASK_CHECKSUM;
				}
				if (size > MAX_MSG_DATA_IN_BUFF_SIZE)
				{
					msg_task_state = MSG_TASK_ERROR;
				}
				else
				{
					msg_task_state = MSG_TASK_WAIT_DATA;	
				}
				
			}
		break;
		case MSG_TASK_WAIT_DATA:
			if (uart_rx_count() >= size)
			{
				uart_read(&msg_data_in_buffer[0],size);
				msg_task_state = MSG_TASK_CHECKSUM;
			}
		break;
		case MSG_TASK_CHECKSUM:
			if (uart_rx_count() >= 2)
			{
				uart_read((U8*)&msg_checksum,2);
				calc_checksum = calc_word_checksum((U8*)&addr,2);
				calc_checksum += calc_word_checksum((U8*)&ID,2);
				calc_checksum += calc_word_checksum((U8*)&size,2);
				calc_checksum += calc_word_checksum(&msg_data_in_buffer[0],size);
				if (calc_checksum == msg_checksum)
				{
					msg_task_state = MSG_TASK_VALID;
				}
				else
				{
					msg_task_state = MSG_TASK_ERROR;
				}
			}
		break;
		case MSG_TASK_VALID:
			msg_process(addr,ID,size,&msg_data_in_buffer[0]);
			//uart_rx_reset();
			msg_task_state = MSG_TASK_WAIT_SYNC1;
		break;
		case MSG_TASK_ERROR:
			//uart_rx_reset();
			msg_task_state = MSG_TASK_WAIT_SYNC1;
		break;
		
	}
}