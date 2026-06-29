/*
 * dht_bz.c
 *
 *  Created on: Feb 7, 2026
 *      Author: kimyujeong
 */
#include "dht_bz.h"
#include <stdio.h>
#include "bz.h"   // ✅ GPIO 버저(논블로킹) 헤더 포함
#include "dht11.h"

void dht_bz_Init(DHTBZ_HandleTypeDef *hbz,
                GPIO_TypeDef *dht_port, uint16_t dht_pin,
                uint32_t period_ms, int temp_th)
{
    hbz->lastDhtTick = 0;
    hbz->dht_ok = 0;
    hbz->period_ms = period_ms;
    hbz->temp_th = temp_th;

    dht11Init(&hbz->dht, dht_port, dht_pin);

    // ✅ 초기에는 버저 끄기 (PWM Stop 대신)
    Buzzer_Off();
}

void dht_bz_Update(DHTBZ_HandleTypeDef *hbz)
{
    uint32_t now = HAL_GetTick();

    // ✅ 논블로킹 버저 타이머 처리 (항상 호출)
    Buzzer_Task();

    /* 1) DHT는 주기마다만 읽기 */
    if (now - hbz->lastDhtTick >= hbz->period_ms)
    {
        hbz->lastDhtTick = now;

        hbz->dht_ok = dht11Read(&hbz->dht);

        if (hbz->dht_ok)
        {
            printf("T:%dC H:%d%%\r\n", hbz->dht.temperature, hbz->dht.humidity);

            /* 2) 임계 온도 이상이면 비프 요청 (논블로킹) */
            if (hbz->dht.temperature >= hbz->temp_th)   // ✅ 40도 포함하려면 >=
            {
                Buzzer_Beep(200);   // 200ms 삐 (원하는 길이로 조절)
            }
        }
        else
        {
            printf("DHT11 FAIL\r\n");
        }
    }
}
