/*
 * dht_bz.h
 *
 *  Created on: Feb 7, 2026
 *      Author: kimyujeong
 */

#ifndef INC_DHT_BZ_H_
#define INC_DHT_BZ_H_

#include "main.h"
#include "dht11.h"
#include "bz.h"

//uint8_t dht_ok;

typedef struct
{
    DHT11 dht;
    uint32_t lastDhtTick;
    uint8_t dht_ok;

    int temp_th;        // 온도 임계값 (ex 40)
    uint32_t period_ms; // DHT 읽기 주기 (ex 2000ms)
} DHTBZ_HandleTypeDef;

/**
 * @brief DHT + Buzzer 통합 모듈 초기화
 * @param hbz        핸들
 * @param dht_port   DHT11 DATA 포트
 * @param dht_pin    DHT11 DATA 핀
 * @param htim_bz    버저 PWM 타이머
 * @param bz_ch      버저 PWM 채널
 * @param period_ms  DHT 읽기 주기(ms) (권장: 2000)
 * @param temp_th    버저 울릴 온도 임계값 (ex 40)
 */
void dht_bz_Init(DHTBZ_HandleTypeDef *hbz,
                GPIO_TypeDef *dht_port, uint16_t dht_pin,
                uint32_t period_ms, int temp_th);



/**
 * @brief main while(1)에서 계속 호출 (논블로킹 스케줄)
 */
void dht_bz_Update(DHTBZ_HandleTypeDef *hbz);

/* 필요하면 상태 접근용(선택) */
uint8_t DHTBZ_IsOk(DHTBZ_HandleTypeDef *hbz);
int     DHTBZ_GetTemp(DHTBZ_HandleTypeDef *hbz);
int     DHTBZ_GetHum(DHTBZ_HandleTypeDef *hbz);


#endif /* INC_DHT_BZ_H_ */
