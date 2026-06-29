/*
 * photo.h
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_PHOTO_H_
#define INC_PHOTO_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum {
  PF_UNKNOWN = 0,   // RAW=000 같은 "아무것도 감지 안됨" -> MOVING
  PF_F1,
  PF_F2,
  PF_F3,
  PF_MOVE_1_2,
  PF_MOVE_2_3,
  PF_ERROR
} photo_fsm_t;

void Photo_Init(void);
void Photo_OnExti(uint16_t GPIO_Pin);
uint8_t Photo_Task(void);

void Photo_GetRaw(uint8_t *p1, uint8_t *p2, uint8_t *p3);
photo_fsm_t Photo_GetFSM(void);
const char* Photo_FSM_ToString(photo_fsm_t f);

#endif /* INC_PHOTO_H_ */
