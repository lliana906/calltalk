/*
 * blue.h
 *
 *  Created on: Feb 9, 2026
 *      Author: kimyujeong
 */

#ifndef INC_BLUE_H_
#define INC_BLUE_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct
{
    uint32_t lastTxTick;
    uint8_t  alarmPrev;
    uint32_t repeat_ms;
} Blue_HandleTypeDef;

// repeat_ms: 예) 3000 (3초마다 재전송)
void Blue_Init(Blue_HandleTypeDef *hbt, uint32_t repeat_ms);

// temp가 th 이상이면 경고 송출 (논블로킹)
void Blue_Update(Blue_HandleTypeDef *hbt, int temp, int th);


//40도시 호출 및lcd로 상황보고
void Blue_UI_Update(Blue_HandleTypeDef *hbt, int dht_ok, int t, int h, int th);
void Blue_SetLcdMode(uint8_t mode, uint32_t hold_ms);


//EMG
void Blue_SendEMG(void);

#endif /* INC_BLUE_H_ */
