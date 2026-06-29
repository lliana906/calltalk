#include "hc.h"

#define LED_PORT   GPIOB
#define LED_PIN    GPIO_PIN_5

#define PIR_PORT   GPIOB
#define PIR_PIN    GPIO_PIN_4

// PIR 감지 레벨 (모듈에 맞게 SET / RESET 선택)
#define PIR_ACTIVE_STATE  GPIO_PIN_SET

#define PIR_SAMPLE_MS   100     // 샘플링 주기
#define PIR_HOLD_MS     2000    // 감지 후 LED 유지시간

static uint32_t lastTick = 0;
static uint32_t ledHoldUntil = 0;

void hc_LED_Init(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    lastTick = HAL_GetTick();
    ledHoldUntil = 0;
}

void hc_LED_Update(void)
{
    uint32_t now = HAL_GetTick();

    // 주기적으로 PIR 확인 (논블로킹)
    if (now - lastTick >= PIR_SAMPLE_MS)
    {
        lastTick = now;

        if (HAL_GPIO_ReadPin(PIR_PORT, PIR_PIN) == PIR_ACTIVE_STATE)
        {
            // 감지되면 홀드 시작
            ledHoldUntil = now + PIR_HOLD_MS;
        }
    }

    // 홀드 시간 동안만 LED ON
    if (now < ledHoldUntil)
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}






//만약 인체감지가 너무 예민하면 사용
//#include "hc.h"
//
//#define LED_PORT   GPIOB
//#define LED_PIN    GPIO_PIN_5
//
//#define PIR_PORT   GPIOB
//#define PIR_PIN    GPIO_PIN_4
//
//// 감지 레벨(모듈에 따라 바꿔)
//// 보통 PIR은 감지 시 HIGH인 경우가 많음
//#define PIR_ACTIVE_STATE  GPIO_PIN_SET
//
//#define PIR_SAMPLE_MS   100     // 샘플링 주기
//#define PIR_HOLD_MS     3000    // 감지 후 LED 유지시간
//
//static uint32_t lastTick = 0;
//static uint32_t ledHoldUntil = 0;
//
//static uint8_t active_cnt = 0;  // 연속 감지 카운트(0~2)
//
//void hc_LED_Init(void)
//{
//    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
//    lastTick = HAL_GetTick();
//    ledHoldUntil = 0;
//    active_cnt = 0;
//}
//
//void hc_LED_Update(void)
//{
//    uint32_t now = HAL_GetTick();
//
//    // 100ms 주기 샘플링 (논블로킹)
//    if (now - lastTick >= PIR_SAMPLE_MS)
//    {
//        lastTick = now;
//
//        GPIO_PinState cur = HAL_GPIO_ReadPin(PIR_PORT, PIR_PIN);
//
//        if (cur == PIR_ACTIVE_STATE)
//        {
//            if (active_cnt < 2) active_cnt++;  // 최대 2까지
//        }
//        else
//        {
//            active_cnt = 0;                    // 연속 감지 끊기면 리셋
//        }
//
//        // ✅ 2번 연속 감지되면 "진짜 감지"로 인정 → 홀드 시작
//        if (active_cnt >= 2)
//        {
//            ledHoldUntil = now + PIR_HOLD_MS;
//            active_cnt = 0; // 같은 이벤트로 계속 연장되는 걸 줄이려면 리셋(선택)
//        }
//    }
//
//    // 홀드 시간 동안만 LED ON
//    if (now < ledHoldUntil)
//        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
//    else
//        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
//}

