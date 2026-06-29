/*
 * dht_bz.c
 *
 *  Created on: Feb 7, 2026
 *      Author: kimyujeong
 */

#include "dht_lcd.h"
#include "i2c_lcd.h"
#include <stdio.h>
#include "dht11.h"

// main.c에 있는 전역 dht를 app에서 쓰고 싶으면 extern
extern DHT11 dht;

static uint32_t lastDhtTick = 0;
static uint8_t lastOk = 0;

void dht_lcd_Init(void)
{

    move_cursor(1, 0);
    lcd_string("GOOD STM");
    lastDhtTick = HAL_GetTick();
}




void dht_lcd_Update(uint8_t temp, uint8_t humi)
{
    char buf[17];

    move_cursor(0, 0);
    snprintf(buf, sizeof(buf), "Temp:%2d C      ", temp);
    lcd_string(buf);

    move_cursor(1, 0);
    snprintf(buf, sizeof(buf), "Humi:%2d %%     ", humi);
    lcd_string(buf);
}


