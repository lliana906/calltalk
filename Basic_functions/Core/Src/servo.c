/*
 * servo.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */



/* servo.c */
#include "servo.h"
#include "tim.h"

static SERVO_STATE servoState;
static uint16_t currentPosition;
static uint16_t targetPosition;
static uint32_t prevTime;

void Servo_Init(void)
{
    servoState = SERVO_STOP;
    currentPosition = SERVO_LEFT;   // 기본 닫힘(왼쪽)
    targetPosition  = SERVO_LEFT;
    prevTime = HAL_GetTick();
    TIM1->CCR1 = currentPosition;
}

void Servo_Open(void)
{
    targetPosition = SERVO_RIGHT;
    servoState = SERVO_MOVING;
}

void Servo_Close(void)
{
    targetPosition = SERVO_LEFT;
    servoState = SERVO_MOVING;
}

void Servo_Run(void)
{
    if (servoState == SERVO_STOP) return;

    if (HAL_GetTick() - prevTime < 20) return;
    prevTime = HAL_GetTick();

    if (currentPosition < targetPosition) currentPosition++;
    else if (currentPosition > targetPosition) currentPosition--;
    else servoState = SERVO_STOP;

    TIM1->CCR1 = currentPosition;
}

bool Servo_IsOpened(void) { return (currentPosition == SERVO_RIGHT); }
bool Servo_IsClosed(void) { return (currentPosition == SERVO_LEFT);  }







