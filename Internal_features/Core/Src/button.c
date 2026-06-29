

#include "button.h"



// 버튼 제어 구조체 배열(각 독립)
static BUTTON_CONTROL button[BUTTON_COUNT]=
		{
				// 외부 엘리베이터 버튼
				{GPIOB,GPIO_PIN_12, GPIO_PIN_RESET},			// 1층 올라감 버튼
				{GPIOB,GPIO_PIN_2, GPIO_PIN_RESET},			// 2층 올라감 버튼
				{GPIOB,GPIO_PIN_1, GPIO_PIN_RESET},			// 2층 내려감 버튼
				{GPIOB,GPIO_PIN_15, GPIO_PIN_RESET},			// 3층 내려감 버튼



				// 내부 엘리베이터 버튼
				{GPIOB,GPIO_PIN_14, GPIO_PIN_RESET},			// 닫힘 버튼
				{GPIOB,GPIO_PIN_13, GPIO_PIN_RESET},			// 열림 버튼
				{GPIOC,GPIO_PIN_4, GPIO_PIN_RESET},			// 1층 버튼
				{GPIOC,GPIO_PIN_9, GPIO_PIN_RESET},			// 2층 버튼
				{GPIOA,GPIO_PIN_5, GPIO_PIN_RESET},			// 3번 버튼
				{GPIOA,GPIO_PIN_6, GPIO_PIN_RESET},			// EMG 버튼

		};



// 버튼별 디바운싱 시간 초기화
void ButtonInit()
{
	for(uint8_t i = 0; i < BUTTON_COUNT; i++)
	{
		button[i].prevTime = 0;
	}
}



/**
 * @brief  버튼이 눌렸는지 확인하는 함수 (디바운싱 포함)
 * @param  num : 확인할 버튼 인덱스
 * @return true  : 버튼이 눌림으로 판정됨
 *         false : 눌리지 않음
 */
bool Button_GetPressed(uint8_t num)
{


//	// 인덱스 보호 코드
//	if(num >= BUTTON_COUNT)
//	{
//		return ret;
//	}


	bool ret = false;


	// 현재 버튼 핀 상태가 "눌린 상태(onState)" 인지 확인
	if(HAL_GPIO_ReadPin(button[num].port, button[num].number) == button[num].onState)
			{
				uint32_t currTime = HAL_GetTick();		// 현재 시간(ms)

        // 이전 인식 시점과 200ms 이상 차이가 날 때만 처리
				if(currTime - button[num].prevTime > DEBOUNCE_TIME)
				{
					// 20ms 후에도 여전히 눌린 상태인지 재확인
					if(HAL_GPIO_ReadPin(button[num].port, button[num].number) == button[num].onState)
					{
						ret = true;											// 유효한 버튼 입력으로 판단
						button[num].prevTime = currTime;	// 마지막 버튼 인식 시간 갱신
					}
				}
			}
	return ret;			// 버튼 눌림 여부 반환
}











