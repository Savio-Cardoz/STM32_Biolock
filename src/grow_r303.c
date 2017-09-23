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
extern uint8_t start_page, page_num;

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

err_t gen_char_file(uint8_t char_buffer_num)
{
	err_t error;
	uint8_t buffer[SIZEOF_GEN_CHAR_CMD];
	grow_r303_instr_t conf_packet;
	uint8_t *buf_ptr;
	uint32_t timeout, tmp;

	rx_config.data_len_to_receive = SIZEOF_GEN_CHAR_CMD - 1;
	rx_config.frame_received = 0;

	buffer[0] = GROW_R303_HEADER / 256;
	buffer[1] = GROW_R303_HEADER % 256;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	buffer[4] = 0xFF;
	buffer[5] = 0xFF;
	buffer[6] = GROW_R303_CMD_PKT;
	buffer[7] = 0x00;
	buffer[8] = 0x04;
	buffer[9] = GROW_R303_INSTR_IMG_TO_CHAR;
	buffer[10] = char_buffer_num;

	buf_ptr = &buffer[6];
	tmp = calculate_checksum(buf_ptr, 5);

	buffer[11] = tmp / 256;
	buffer[12] = tmp % 256;

	for(uint32_t i = 0; i < SIZEOF_GEN_CHAR_CMD; i++)
	{
		USART_SendData(USART1, buffer[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// Wait for transmission to complete
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

err_t gen_tmplt_file()
{
	err_t error;
	uint8_t buffer[SIZEOF_GEN_TMPLT_CMD];
	grow_r303_instr_t conf_packet;
	uint8_t *buf_ptr;
	uint32_t timeout;

	rx_config.data_len_to_receive = SIZEOF_GEN_TMPLT_CMD;
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
	buffer[9] = GROW_R303_INSTR_MAKE_TMPLT;
	buffer[10] = 0x00;
	buffer[11] = 0x09;

	for(uint32_t i = 0; i < SIZEOF_GEN_TMPLT_CMD; i++)
	{
		USART_SendData(USART1, buffer[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// Wait for transmission to complete
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

err_t save_tmplt_file(uint8_t char_buffer_num, uint8_t tmplt_id)
{
	err_t error;
	uint8_t buffer[SIZEOF_SAVE_TMPLT_CMD];
	grow_r303_instr_t conf_packet;
	uint8_t *buf_ptr;
	uint32_t timeout, tmp;

	rx_config.data_len_to_receive = SIZEOF_SAVE_TMPLT_CMD - 3;
	rx_config.frame_received = 0;

	buffer[0] = GROW_R303_HEADER / 256;
	buffer[1] = GROW_R303_HEADER % 256;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	buffer[4] = 0xFF;
	buffer[5] = 0xFF;
	buffer[6] = GROW_R303_CMD_PKT;
	buffer[7] = 0x00;
	buffer[8] = 0x06;
	buffer[9] = GROW_R303_INSTR_STORE_TMPLT;
	buffer[10] = char_buffer_num;
	buffer[11] = tmplt_id / 256;
	buffer[12] = tmplt_id % 256;

	buf_ptr = &buffer[6];
	tmp = calculate_checksum(buf_ptr, 7);

	buffer[13] = tmp / 256;
	buffer[14] = tmp % 256;

	for(uint32_t i = 0; i < SIZEOF_SAVE_TMPLT_CMD; i++)
	{
		USART_SendData(USART1, buffer[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// Wait for transmission to complete
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

err_t search_tmplt(uint8_t char_buffer_num, uint8_t *tmplt_id)
{
	err_t error;
	uint8_t buffer[SIZEOF_SEARCH_TMPLT_CMD];
	grow_r303_instr_t conf_packet;
	uint8_t *buf_ptr;
	uint32_t timeout, tmp;

	rx_config.data_len_to_receive = SIZEOF_SEARCH_TMPLT_CMD - 1;
	rx_config.frame_received = 0;

	buffer[0] = GROW_R303_HEADER / 256;
	buffer[1] = GROW_R303_HEADER % 256;
	buffer[2] = 0xFF;
	buffer[3] = 0xFF;
	buffer[4] = 0xFF;
	buffer[5] = 0xFF;
	buffer[6] = GROW_R303_CMD_PKT;
	buffer[7] = 0x00;
	buffer[8] = 0x08;
	buffer[9] = GROW_R303_INSTR_SEARCH_LIB;
	buffer[10] = char_buffer_num;
	buffer[11] = start_page / 256;
	buffer[12] = start_page % 256;
	buffer[13] = page_num / 256;
	buffer[14] = page_num % 256;

	buf_ptr = &buffer[6];
	tmp = calculate_checksum(buf_ptr, 9);

	buffer[15] = tmp / 256;
	buffer[16] = tmp % 256;

	for(uint32_t i = 0; i < SIZEOF_SEARCH_TMPLT_CMD; i++)
	{
		USART_SendData(USART1, buffer[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		// Wait for transmission to complete
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
