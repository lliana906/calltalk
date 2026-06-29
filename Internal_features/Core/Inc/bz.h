/*
 * bz.h
 *
 *  Created on: Feb 8, 2026
 *      Author: kimyujeong
 */

#ifndef INC_BZ_H_
#define INC_BZ_H_

#include "stm32f4xx_hal.h"

#define BUZZER_GPIO GPIOA
#define BUZZER_PIN  GPIO_PIN_9

void Buzzer_On(void);
void Buzzer_Off(void);

/* 논블로킹용 */
void Buzzer_Beep(uint32_t ms);  // 비프 요청
void Buzzer_Task(void);         // 주기적으로 호출

#endif /* INC_BZ_H_ */
