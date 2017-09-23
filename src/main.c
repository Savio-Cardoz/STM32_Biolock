 /*
  *******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#define __CHECK_DEVICE_DEFINES
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stdlib.h"
#include "system_defs.h"
#include "comm_func.h"
#include "grow_r303.h"

volatile uint32_t ticks, sys_tick;
volatile rx_buffer_config_t rx_config;
uint8_t rx_buffer[50];
volatile system_state current_system_state;
uint8_t start_page = 0, page_num = 0;


void configure_system_clock(void)
{
	RCC->CR &= ~((uint32_t)RCC_CR_HSEON);		// Disable external oscillator use
}

void SysTick_Handler(void)
{
	static uint32_t last_tick_count;
	sys_tick++;

	if((current_system_state == STATE_ENROLL_USER) && (sys_tick - last_tick_count > 1000))
	{
		last_tick_count = sys_tick;
		GPIO_ToggleBits(GPIOA, BEAT_LED_PIN);
	}

	ticks--;
}

void USART1_IRQHandler(void)
{
	static uint32_t rx_data_count;

	if(USART1->SR & (1 << 5))
	{
		rx_buffer[rx_data_count] = USART1->DR;
		rx_data_count++;

		if(rx_data_count == rx_config.data_len_to_receive)
		{
			rx_config.frame_received = 1;
			rx_config.rx_data_ptr = rx_buffer;
			rx_data_count = 0;																	// Clear rx_data_count to receive more data.
		}
	}
}

void init_sys()
{
	SysTick_Config(SYSTEM_CLOCK / 8400);	// TODO set for a 1ms interrupt
}

void init_port_pins()
{
	GPIO_InitTypeDef gpio_x;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Configure for BEATING LED	*/
	gpio_x.GPIO_Pin = BEAT_LED_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_OUT;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_x.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOA, &gpio_x);

	/* Configure for OK LED	*/
	gpio_x.GPIO_Pin = OK_LED_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_OUT;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_x.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOA, &gpio_x);

	/* Configure for ERROR LED	*/
	gpio_x.GPIO_Pin = ERROR_LED_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_OUT;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_x.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOA, &gpio_x);

	/* Configure for USER BUTTON	*/
	gpio_x.GPIO_Pin = USER_BUTTON_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_IN;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_x.GPIO_Speed = GPIO_Low_Speed;

	GPIO_Init(GPIOC, &gpio_x);

	/* Configure for UART2 pins		*/
	gpio_x.GPIO_Pin = UART2_TX_PIN | UART2_RX_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_AF;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_x.GPIO_Speed = GPIO_Fast_Speed;

	GPIO_Init(GPIOA, &gpio_x);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	/* Configure for UART1 pins		*/
	gpio_x.GPIO_Pin = UART1_TX_PIN | UART1_RX_PIN;
	gpio_x.GPIO_Mode = GPIO_Mode_AF;
	gpio_x.GPIO_OType = GPIO_OType_PP;
	gpio_x.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_x.GPIO_Speed = GPIO_Fast_Speed;

	GPIO_Init(GPIOA, &gpio_x);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}



int main(void)
{
	uint32_t tmplt_num = 1;
	uint8_t match_id;

	configure_system_clock();
	init_uart();
	init_port_pins();
	init_sys();

	GPIO_ResetBits(GPIOA, BEAT_LED_PIN);
	GPIO_ResetBits(GPIOA, OK_LED_PIN);
	GPIO_ResetBits(GPIOA, ERROR_LED_PIN);

	delay_ms(1000);

	while(1)
	{

		/* Authenticate if finger is detected	*/

		if(gen_finger_img() == ERR_OK)
		{
			if(gen_char_file(GROW_R303_CHAR_BUF_1) == ERR_OK)
			{
				start_page = 1; page_num = 1;
				if(gen_tmplt_file() == ERR_OK)
				{
					if(search_tmplt(GROW_R303_CHAR_BUF_1, &match_id) == ERR_OK)
					{
						GPIO_SetBits(GPIOA, OK_LED_PIN);
						delay_ms(1000);
						GPIO_ResetBits(GPIOA, OK_LED_PIN);
					}
					else {
						GPIO_SetBits(GPIOA, ERROR_LED_PIN);
						delay_ms(1000);
						GPIO_ResetBits(GPIOA, ERROR_LED_PIN);
					}

				}
			}
		}



		if(GPIO_ReadInputDataBit(GPIOC, USER_BUTTON_PIN) == BUTTON_PRESSED)
		{
			current_system_state = STATE_ENROLL_USER;

			while(current_system_state == STATE_ENROLL_USER)
			{
				for(uint32_t z = 0; z < 2; z++)			// Capture finger images twice
				{
					while(gen_finger_img() != ERR_OK);
					GPIO_SetBits(GPIOA, OK_LED_PIN);

					if(gen_char_file(z+1) != ERR_OK)
					{
						GPIO_SetBits(GPIOA, ERROR_LED_PIN);
						break;
					}

					while(gen_finger_img() == ERR_OK);	// Wait for the finger to be lifted
					GPIO_ResetBits(GPIOA, OK_LED_PIN);
				}

				if(gen_tmplt_file() == ERR_OK)
				{
					if(save_tmplt_file(GROW_R303_CHAR_BUF_1, tmplt_num) == ERR_OK)
					{
						GPIO_SetBits(GPIOA, OK_LED_PIN);
						current_system_state = STATE_SYS_LOCKED;
					}
				}
				else
				{
					current_system_state = STATE_SYS_LOCKED;
					GPIO_ResetBits(GPIOA, OK_LED_PIN);
				}

			}
		}
	}
}

void delay_ms(uint32_t delay_time)
{
	ticks = delay_time;
	while(ticks);
}

/*	Calculate the sum of data in data buffer and return the sum value
 * 	param *data pointer to the data buffer
 * 	param data_len - length of data for which sum is to be calculated
 *
 * 	returns sum - sum of the bytes in the data buffer
 *
 */
uint32_t calculate_checksum(uint8_t *data, uint32_t data_len)
{
	uint32_t sum = 0;

	for(uint32_t i = 0; i < data_len; i++)
	{
		sum += *data;
		data++;
	}

	return sum;
}
