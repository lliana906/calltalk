/*
 * logger.h
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_



#include "stm32f4xx_hal.h"
#include <stdarg.h>

void Log_Init(UART_HandleTypeDef *huart);
void Log_Printf(const char *fmt, ...);



#endif /* INC_LOGGER_H_ */
