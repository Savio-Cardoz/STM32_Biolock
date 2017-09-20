/*
 * comm_func.h
 *
 *  Created on: Sep 11, 2017
 *      Author: Cardoz
 */

#ifndef COMM_FUNC_H_
#define COMM_FUNC_H_

#define SIZE_OF_COMM_BUFFER 50

typedef struct {
	uint8_t *rx_data_ptr;
	uint32_t data_len_to_receive;
	uint32_t frame_received:1;
}__attribute__((packed, aligned(1))) rx_buffer_config_t;

void init_uart();

#endif /* COMM_FUNC_H_ */
