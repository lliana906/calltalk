/*
 * app.c
 *
 *  Created on: Feb 5, 2026
 *      Author: parkdoyoung
 */



#include "app.h"
#include "servo.h"
#include "button.h"
#include "stepper.h"
#include "tim.h"
#include "stdbool.h"
#include "blue.h"


void App_Init(void)
{
	ButtonInit();
	Servo_Init();
	Stepper_Init();
	Elevator_Init();
}




void App_Task(void)
{

  // EMG 우선 처리
  if(Button_GetPressed(BTN_EMG))
  {
	  Elevator_CallEMG();
	  Blue_SendEMG();
  }


  // 내부 버튼
  if(Button_GetPressed(BTN_OPEN))  Elevator_CallOpen();
  if(Button_GetPressed(BTN_CLOSE)) Elevator_CallClose();
  if(Button_GetPressed(BTN_1F)) Elevator_CallFloor(1);
  if(Button_GetPressed(BTN_2F)) Elevator_CallFloor(2);
  if(Button_GetPressed(BTN_3F)) Elevator_CallFloor(3);




  // 외부 버튼
  if(Button_GetPressed(BTN_1F_UP)) Elevator_CallFloor(1);
  if(Button_GetPressed(BTN_2F_UP)) Elevator_CallFloor(2);
  if(Button_GetPressed(BTN_2F_DW)) Elevator_CallFloor(2);
  if(Button_GetPressed(BTN_3F_DW)) Elevator_CallFloor(3);




  Elevator_Task();
  Servo_Run();

}
