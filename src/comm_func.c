/*
 * comm_func.c
 *
 *  Created on: Sep 11, 2017
 *      Author: Cardoz
 */

#include "stdio.h"
#include "stm32f4xx.h"
#include "comm_func.h"

/* Variable declarations		*/



/*		Initial all uarts to be used here			*/
void init_uart()
{
	USART_InitTypeDef uart_x;
	NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure for UART	*/
	uart_x.USART_BaudRate = 115200;
	uart_x.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart_x.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart_x.USART_WordLength = USART_WordLength_8b;
	uart_x.USART_StopBits = USART_StopBits_1;
	uart_x.USART_Parity = USART_Parity_No;

	USART_Init(USART2, &uart_x);
	USART_Cmd(USART2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure for UART	*/
	uart_x.USART_BaudRate = 57600;
	uart_x.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart_x.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart_x.USART_WordLength = USART_WordLength_8b;
	uart_x.USART_StopBits = USART_StopBits_1;
	uart_x.USART_Parity = USART_Parity_No;

	USART_Init(USART1, &uart_x);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // we want to configure the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	USART_Cmd(USART1, ENABLE);

}

