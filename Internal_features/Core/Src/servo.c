


#include "servo.h"
#include "tim.h"



// 서보 상태 (정지/이동)
static SERVO_STATE servoState;


// 현재 서보모터 위치
static uint16_t currentPosition;


// 목표 위치(열림/닫힘)
static uint16_t targetPosition;


// 동작 시간
static uint32_t prevTime;



// 서보 초기화
void Servo_Init(void)
{
  servoState = SERVO_STOP;					// 초기 상태: 정지
  currentPosition   = SERVO_LEFT;		// 초기 위치 : 문 닫힘
  targetPosition    = SERVO_LEFT;		// 목표 위치 : 문 닫힘
  prevTime   = 0;										// 시간 초기화

  TIM1->CCR1 = currentPosition;			// 초기 PWM 출력 설정
}




// 문 열기
void Servo_Open()
{

	targetPosition = SERVO_RIGHT;		// 목표 위치 : 문 열림
	servoState = SERVO_MOVING;			// 이동 시작


//  if (servoState == SERVO_STOP)
//  {
//      servoState = SERVO_ROTATE;
//  }
//  else
//  {
//      servoState = SERVO_STOP;
//  }
}




// 문 닫기
void Servo_Close()
{
	targetPosition = SERVO_LEFT;		// 목표 위치 : 문 닫힘
	servoState = SERVO_MOVING;			// 이동 시작

}


// main 함수 호출용
void Servo_Run()
{
  if (servoState == SERVO_STOP)				return;		// 정지 상태 시 반환


  // 20ms마다 한 칸 이동
  if (HAL_GetTick() - prevTime < 20) 	return;

  prevTime = HAL_GetTick();


  // 목표 위치와 현재 위치 비교 후 1씩 이동
  if (currentPosition < targetPosition)
  {
      currentPosition++;
  }
  else if (currentPosition > targetPosition)
  {
      currentPosition--;
  }

  else
  {
      servoState = SERVO_STOP;	// 목표 위치 도달 시 정지
  }

  TIM1->CCR1 = currentPosition;	// PWM 값 업데이트
}



// 문 상태 확인
bool Servo_IsOpened()
{
    return (currentPosition == SERVO_RIGHT);	// 완전히 열림 여부
}

bool Servo_IsClosed()
{
	return (currentPosition == SERVO_LEFT);			// 완전히 닫힘 여부
}


