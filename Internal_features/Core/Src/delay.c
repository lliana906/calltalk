/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: kimyujeong
 */


#include <delay.h>

void delay_us(uint16_t us)
{
  uint16_t start = __HAL_TIM_GET_COUNTER(&htim11);
  while((__HAL_TIM_GET_COUNTER(&htim11) - start) < us);
}















//void delay_us(uint16_t us)
//{
//  __HAL_TIM_SET_COUNTER(&htim10, 0);
//  while((__HAL_TIM_GET_COUNTER(&htim10)) < us);
//}
