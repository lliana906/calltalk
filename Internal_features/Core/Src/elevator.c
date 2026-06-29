/*
 * elevator.c
 *
 *  Created on: Feb 5, 2026
 *      Author: parkdoyoung
 */


#include "elevator.h"
#include "stepper.h"
#include "servo.h"

#define FLOOR_STEPS		4096
#define DOOR_WAIT_MS  10000		// 문열림 지속시간 설정


static ELEVATOR_STATE ElevatorState;
static uint8_t currentFloor = 1;
static uint8_t targetFloor = 1;

static uint16_t moveSteps = 0;
static uint32_t doorTick = 0;




// Elevator 초기화
void Elevator_Init()
{

	ElevatorState = ELEVATOR_IDLE;
}



// Elevator 기능
void Elevator_CallFloor(uint8_t floor)
{

	targetFloor = floor;
	if(targetFloor == currentFloor)
	{
		ElevatorState = ELEVATOR_DOOR_OPEN;
		doorTick = HAL_GetTick();
	}

//	if(ElevatorState == ELEVATOR_IDLE || ElevatorState == ELEVATOR_DOOR_WAIT)
//	{
//		targetFloor = floor;
//	}
}

void Elevator_CallOpen()
{

	ElevatorState = ELEVATOR_DOOR_OPEN;
	doorTick = HAL_GetTick();
//	if(ElevatorState == ELEVATOR_DOOR_WAIT)
//	{
//		ElevatorState = ELEVATOR_DOOR_CLOSE;
//	}
}

void Elevator_CallClose()
{

	ElevatorState = ELEVATOR_DOOR_CLOSE;

//	if(ElevatorState == ELEVATOR_DOOR_WAIT)
//	{
//		ElevatorState = ELEVATOR_DOOR_CLOSE;
//	}
}

void Elevator_CallEMG()
{
	ElevatorState = ELEVATOR_EMG;
}

void Elevator_Task()
{
  switch(ElevatorState)
  {
      case ELEVATOR_IDLE:
          if(targetFloor > currentFloor) ElevatorState = ELEVATOR_MOVING_UP;
          else if(targetFloor < currentFloor) ElevatorState = ELEVATOR_MOVING_DOWN;
          break;

      case ELEVATOR_MOVING_UP:
          if(Stepper_Task(DIR_CW) && ++moveSteps >= FLOOR_STEPS)
          {
          		moveSteps = 0;
              currentFloor++;
              if(currentFloor == targetFloor) ElevatorState = ELEVATOR_DOOR_OPEN;
          }
          break;

      case ELEVATOR_MOVING_DOWN:
          if(Stepper_Task(DIR_CCW) && ++moveSteps >= FLOOR_STEPS)
          {
          		moveSteps = 0;
          		currentFloor--;
              if(currentFloor == targetFloor) ElevatorState = ELEVATOR_DOOR_OPEN;
          }
          break;

      case ELEVATOR_DOOR_OPEN:
          Servo_Open();
          doorTick = HAL_GetTick();
          ElevatorState = ELEVATOR_DOOR_WAIT;
          break;

      case ELEVATOR_DOOR_WAIT:
          if(HAL_GetTick() - doorTick > DOOR_WAIT_MS)
              ElevatorState = ELEVATOR_DOOR_CLOSE;
          break;

      case ELEVATOR_DOOR_CLOSE:
          Servo_Close();
          if(Servo_IsClosed()) ElevatorState = ELEVATOR_IDLE;
          break;

      case ELEVATOR_EMG:
          // 정지 유지
          break;
  }
}


// 상태 확인
uint8_t Elevator_GetCurrentFloor()
{
	return currentFloor;
}
ELEVATOR_STATE Elevator_GetState()
{
	return ElevatorState;
}





