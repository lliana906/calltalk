/*
 * elevator.h
 *
 *  Created on: Feb 5, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_



#include "stm32f4xx_hal.h"
#include "stdbool.h"


// 상태머신 정의
typedef enum
{


	ELEVATOR_IDLE,
	ELEVATOR_MOVING_UP,
	ELEVATOR_MOVING_DOWN,
	ELEVATOR_DOOR_OPEN,
	ELEVATOR_DOOR_WAIT,
	ELEVATOR_DOOR_CLOSE,
	ELEVATOR_EMG


}ELEVATOR_STATE;


// Elevator 초기화
void Elevator_Init();


// Elevator 동작
void Elevator_Task();


// Elevator 기능
void Elevator_CallFloor(uint8_t floor);

void Elevator_CallOpen();

void Elevator_CallClose();

void Elevator_CallEMG();


void Elevator_Task();

// 상태 확인
uint8_t Elevator_GetCurrentFloor();
ELEVATOR_STATE Elevator_GetState();




#endif /* INC_ELEVATOR_H_ */
