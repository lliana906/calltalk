/*
 * button.h
 *
 *  Created on: Jan 27, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_






#include "stm32f4xx_hal.h"
#include <stdbool.h>



#define BUTTON_COUNT		10				// 버튼 갯수
#define DEBOUNCE_TIME 	200			// 디바운싱 시간 (ms)




// 버튼 관련 구조체
typedef struct
{

	GPIO_TypeDef		*port;				// 버튼 연결 된 GPIO 포트
	uint16_t				number;				// 버튼 핀 번호
	GPIO_PinState		onState;			// 버튼 눌렸을 때 GPIO 상태
	uint32_t				prevTime;			// 디바운싱을 위한 이전 인식 시간(ms)

}BUTTON_CONTROL;



typedef enum
{

	BTN_1F_UP,
	BTN_2F_UP,
	BTN_2F_DW,
	BTN_3F_DW,
	BTN_CLOSE,
	BTN_OPEN,
	BTN_1F,
	BTN_2F,
	BTN_3F,
	BTN_EMG

}BUTTON_table;



// 버튼 초기화 함수 ( 디바운싱 시간 초기화)
void ButtonInit();


// 눌림 체크
bool Button_GetPressed(uint8_t num);





#endif /* INC_BUTTON_H_ */
