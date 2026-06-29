/*
 * resident_uart.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_RESIDENT_UART_H_
#define INC_RESIDENT_UART_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>

void ResidentUART_Init(UART_HandleTypeDef *huart);
void ResidentUART_Task(void);




#endif /* INC_RESIDENT_UART_H_ */
