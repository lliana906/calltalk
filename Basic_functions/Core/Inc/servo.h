/*
 * servo.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_





#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define SERVO_LEFT   40    // 닫힘
#define SERVO_RIGHT  240   // 열림

typedef enum
{
    SERVO_STOP,
    SERVO_MOVING
} SERVO_STATE;

void Servo_Init(void);
void Servo_Open(void);
void Servo_Close(void);
void Servo_Run(void);

bool Servo_IsOpened(void);
bool Servo_IsClosed(void);






#endif /* INC_SERVO_H_ */
