/*
 * grow_r303.c
 *
 *  Created on: Sep 11, 2017
 *      Author: Cardoz
 */

#include "stm32f4xx.h"
#include "stdlib.h"
#include "comm_func.h"
#include "grow_r303.h"

extern volatile rx_buffer_config_t rx_config;
extern volatile uint32_t sys_tick;

err_t gen_finger_img()
{
	err_t error;
	uint8_t buffer[SIZEOF_GEN_IMG_CMD];
	grow_r303_instr_t conf_packet;
	uint8_t *buf_ptr;
	uint32_t timeout;

	rx_config.data_len_to_receive = SIZEOF_GEN_IMG_CMD;
	rx_config.frame_received = 0;

	buffer[0] = GROW_R303_HEADER / 256;
	buffer[1] = GROW_R303_HEADER % 256;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	buffer[4] = 0xFF;
	buffer[5] = 0xFF;
	buffer[6] = GROW_R303_CMD_PKT;
	buffer[7] = 0x00;
	buffer[8] = 0x03;
	buffer[9] = GROW_R303_INSTR_GET_FINGER_IMG;
	buffer[10] = 0x00;
	buffer[11] = 0x05;

	for(uint32_t i = 0; i < SIZEOF_GEN_IMG_CMD; i++)
	{
		USART_SendData(USART1, buffer[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// Wait for transmission to complete
//		delay_ms(50);
	}

	timeout = sys_tick;
	while((rx_config.frame_received == 0) && (sys_tick - timeout < 2000));

	if(sys_tick - timeout < 2000)
	{
		memcpy(&conf_packet.header, rx_config.rx_data_ptr, sizeof(grow_r303_instr_t));

		error = conf_packet.instruction_code;
	}
	else error = ERR_RECV_PKG;

	return error;
}
