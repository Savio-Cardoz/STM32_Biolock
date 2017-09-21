/*
 * system_defs.h
 *
 *  Created on: Aug 29, 2017
 *      Author: Cardoz
 */

#ifndef SYSTEM_DEFS_H_
#define SYSTEM_DEFS_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#define DEBUG			1
#define BUTTON_PRESSED	0

typedef enum{
	STATE_ENROLL_USER = 1,
	STATE_VERIFY_USER,
	STATE_SYS_LOCKED,
	STATE_SYS_UNLOCKED
}system_state;

typedef struct			// Using to access individual bits/pins of a register/port
{
	unsigned int bit0:1;
	unsigned int bit1:1;
	unsigned int bit2:1;
	unsigned int bit3:1;
	unsigned int bit4:1;
	unsigned int bit5:1;
	unsigned int bit6:1;
	unsigned int bit7:1;
} _io_reg;

#define REGISTER_BIT(rg,bt) ((volatile _io_reg*)&rg)->bit##bt


#define BEAT_LED_PIN 	GPIO_Pin_5
#define UART2_TX_PIN 	GPIO_Pin_2
#define UART2_RX_PIN 	GPIO_Pin_3
#define UART1_TX_PIN 	GPIO_Pin_9
#define UART1_RX_PIN 	GPIO_Pin_10
#define USER_BUTTON_PIN	GPIO_Pin_13
#define OK_LED_PIN		GPIO_Pin_0
#define ERROR_LED_PIN	GPIO_Pin_1

#define SYSTEM_CLOCK 84000000

volatile uint32_t flag_register;

#define flag_tx1_enable REGISTER_BIT(flag_register, 0)
#define flag_rx1_enable REGISTER_BIT(flag_register, 1)

void delay_ms(uint32_t delay_time);
uint32_t calculate_checksum(uint8_t *data, uint32_t data_len);

#endif /* SYSTEM_DEFS_H_ */
