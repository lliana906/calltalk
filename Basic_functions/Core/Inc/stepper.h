/*
 * stepper.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_




#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

//#define STEPS_PER_REV  4096


#define DIR_UP     0   // CW
#define DIR_DOWN   1   // CCW



#define IN1_PORT GPIOA
#define IN1_PIN  GPIO_PIN_4

#define IN2_PORT GPIOB
#define IN2_PIN  GPIO_PIN_0

#define IN3_PORT GPIOC
#define IN3_PIN  GPIO_PIN_1

#define IN4_PORT GPIOC
#define IN4_PIN  GPIO_PIN_0

void Stepper_Init(void);
void Stepper_StartContinuous(uint8_t dir);
void Stepper_Stop(void);
void Stepper_Task(void);
bool Stepper_IsBusy(void);


#endif /* INC_STEPPER_H_ */
