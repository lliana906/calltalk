/*
 * servo.h
 *
 *  Created on: Jan 27, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_



#include "stm32f4xx_hal.h"
#include <stdbool.h>


// 서보 각도 정의
#define SERVO_LEFT			40		// 닫힘
#define SERVO_CENTER		140		// 중립
#define SERVO_RIGHT			240		// 열림




// 서보 회전 상태
typedef enum
{
	SERVO_STOP,			// 정지
	SERVO_MOVING		// 목표 위치 이동
}SERVO_STATE;


// 서보 초기화
void Servo_Init();


// 문 열기
void Servo_Open();


// 문 닫기
void Servo_Close();


// main 함수 호출용
void Servo_Run();


// 완전 열림 여부
bool Servo_IsOpened();


// 완전 닫힘 여부
bool Servo_IsClosed();

#endif /* INC_SERVO_H_ */
