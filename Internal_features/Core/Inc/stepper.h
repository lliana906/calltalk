/*
 * stepper.h
 *
 *  Created on: Feb 4, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_



#include "stm32f4xx_hal.h"
#include "stdbool.h"



#define STEPS_PER_REVOLATION  4096
#define DIR_CW                0   // 시계 방향
#define DIR_CCW               1   // 반시계 방항


#define IN1_PIN     GPIO_PIN_4
#define IN2_PIN     GPIO_PIN_0
#define IN3_PIN     GPIO_PIN_1
#define IN4_PIN     GPIO_PIN_0

#define IN1_PORT    GPIOA
#define IN2_PORT    GPIOB
#define IN3_PORT    GPIOC
#define IN4_PORT    GPIOC




// 초기화 함수
void Stepper_Init();


void stepMotor(uint8_t step);


// 한 스텝 실행 시 true 반환
bool Stepper_Task(uint8_t direction);


void rotateSteps(uint16_t steps, uint8_t direction);
void rotateDegrees(uint16_t degrees, uint8_t direction);




#endif /* INC_STEPPER_H_ */
