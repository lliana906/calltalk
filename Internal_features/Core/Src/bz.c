/*
 * bz.c
 *
 *  Created on: Feb 8, 2026
 *      Author: kimyujeong
 */

#include "bz.h"

static uint8_t buzzerOn = 0;
static uint32_t buzzerTick = 0;
static uint32_t buzzerDuration = 0;

void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, GPIO_PIN_RESET);
}

/* 비프 시작 요청 (논블로킹) */
void Buzzer_Beep(uint32_t ms)
{
    buzzerOn = 1;
    buzzerDuration = ms;
    buzzerTick = HAL_GetTick();
    Buzzer_On();
}

/* main / App_Task에서 계속 호출 */
void Buzzer_Task(void)
{
    if (buzzerOn)
    {
        if (HAL_GetTick() - buzzerTick >= buzzerDuration)
        {
            buzzerOn = 0;
            Buzzer_Off();
        }
    }
}

