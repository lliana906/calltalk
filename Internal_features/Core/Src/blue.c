/*
 * blue.c
 *
 *  Created on: Feb 9, 2026
 *      Author: kimyujeong
 */

#include "blue.h"
#include "usart.h"     // extern UART_HandleTypeDef huart1;
#include <stdio.h>
#include "i2c_lcd.h"
#include "dht_lcd.h"

void Blue_Init(Blue_HandleTypeDef *hbt, uint32_t repeat_ms)
{
    hbt->lastTxTick = 0;
    hbt->alarmPrev  = 0;
    hbt->repeat_ms  = repeat_ms;
}

void Blue_Update(Blue_HandleTypeDef *hbt, int temp, int th)
{
    uint32_t now = HAL_GetTick();
    uint8_t alarmNow = (temp >= th);

    // alarmNow=1일 때:
    // 1) 처음 진입(!alarmPrev) -> 1회 송출
    // 2) 유지 중이면 repeat_ms마다 송출
    if (alarmNow)
    {
        if (!hbt->alarmPrev || (now - hbt->lastTxTick >= hbt->repeat_ms))
        {
            char msg[96];
            int n = snprintf(msg, sizeof(msg),
                             "WARNING: HIGH TEMPERATURE (%dC). PLEASE CHECK.\r\n",
                             temp);

            // ✅ TX (블로킹 송신이지만 길이가 짧아 시연/실습에 충분)
            HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)n, 100);

            hbt->lastTxTick = now;
        }
    }

    hbt->alarmPrev = alarmNow;
}


//40도시 호출 및lcd로 상황보고
static uint8_t  lcd_mode = 0;      // RX에서 바뀌게 할 거면 setter 필요
static uint32_t lcd_until = 0;
static uint8_t  last_lcd_mode = 0;

void Blue_UI_Update(Blue_HandleTypeDef *hbt, int dht_ok, int t, int h, int th)
{
    if (!dht_ok) return;

    // ✅ 1) TX 먼저
    Blue_Update(hbt, t, th);

    // ✅ 2) LCD 표시
    uint32_t now = HAL_GetTick();

    if (lcd_mode != 0 && now < lcd_until)
    {
        if (last_lcd_mode != lcd_mode)
        {
            last_lcd_mode = lcd_mode;
            lcd_command(CLEAR_DISPLAY);

            if (lcd_mode == 1) {
                move_cursor(0,0); lcd_string("  NO PROBLEM");
                move_cursor(1,0); lcd_string("  ALL CLEAR");
            } else if (lcd_mode == 2) {
                move_cursor(0,0); lcd_string("  DANGER!");
                move_cursor(1,0); lcd_string("  EVACUATE NOW");
            } else if (lcd_mode == 3) {
                move_cursor(0,0); lcd_string("  HAVE A NICE");
                move_cursor(1,0); lcd_string("  DAY :)");
            }
        }
    }
    else
    {
        lcd_mode = 0;
        last_lcd_mode = 0;
        dht_lcd_Update(t, h);
    }
}

void Blue_SetLcdMode(uint8_t mode, uint32_t hold_ms)
{
    lcd_mode = mode;
    lcd_until = HAL_GetTick() + hold_ms;
}

//EMG

void Blue_SendEMG(void)
{
    // 버튼은 디바운싱 되어 있지만, 혹시 연속 눌림 대비해서 1초 쿨타임(논블로킹)
    static uint32_t lastEmgTick = 0;
    uint32_t now = HAL_GetTick();

    if (now - lastEmgTick < 1000) return;

    char msg[] = "EMERGENCY BUTTON PRESSED! CHECK IMMEDIATELY.\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg)-1, 100);

    lastEmgTick = now;
}


