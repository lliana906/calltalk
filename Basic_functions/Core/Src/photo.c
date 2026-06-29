/*
 * photo.c
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 */


#include "photo.h"

/* ===== 핀 매핑 ===== */
#define PHOTO1_PORT GPIOA
#define PHOTO1_PIN  GPIO_PIN_12

#define PHOTO2_PORT GPIOB
#define PHOTO2_PIN  GPIO_PIN_7

#define PHOTO3_PORT GPIOB
#define PHOTO3_PIN  GPIO_PIN_10

/* ✅ 보통 포토모듈: 빔 끊기면 LOW(RESET) */
#define PHOTO_ACTIVE_STATE GPIO_PIN_RESET

static inline uint8_t Detected(GPIO_TypeDef *port, uint16_t pin)
{
  return (HAL_GPIO_ReadPin(port, pin) == PHOTO_ACTIVE_STATE) ? 1 : 0;
}

#define PHOTO_DEBOUNCE_MS  30

static volatile uint8_t  s_irq_pending = 0;
static volatile uint32_t s_irq_tick = 0;

static uint8_t  s_p1=0, s_p2=0, s_p3=0;
static uint8_t  s_valid = 0;
static uint32_t s_boot_tick = 0;

static void ReadNow(uint8_t *o1, uint8_t *o2, uint8_t *o3)
{
  *o1 = Detected(PHOTO1_PORT, PHOTO1_PIN);
  *o2 = Detected(PHOTO2_PORT, PHOTO2_PIN);
  *o3 = Detected(PHOTO3_PORT, PHOTO3_PIN);
}

static photo_fsm_t DecodeFSM(uint8_t p1, uint8_t p2, uint8_t p3)
{
  uint8_t sum = (uint8_t)(p1 + p2 + p3);

  if (sum == 0) return PF_UNKNOWN;          // 아무도 감지 안 함 -> MOVING 표시

  if (sum == 1)
  {
    if (p1) return PF_F1;
    if (p2) return PF_F2;
    if (p3) return PF_F3;
  }

  if (sum == 2)
  {
    if (p1 && p2 && !p3) return PF_MOVE_1_2;
    if (!p1 && p2 && p3) return PF_MOVE_2_3;
    return PF_ERROR;                        // p1&p3 같은 비정상
  }

  return PF_ERROR;
}

void Photo_Init(void)
{
  s_irq_pending = 0;
  s_irq_tick = 0;
  s_boot_tick = HAL_GetTick();

  /* ✅ 부팅하자마자 현재값 1회 읽기 */
  ReadNow(&s_p1, &s_p2, &s_p3);
  s_valid = 1;
}

void Photo_OnExti(uint16_t GPIO_Pin)
{
  /* 부팅 직후 튐 방지 */
  if (HAL_GetTick() - s_boot_tick < 200) return;

  if (GPIO_Pin == PHOTO1_PIN || GPIO_Pin == PHOTO2_PIN || GPIO_Pin == PHOTO3_PIN)
  {
    s_irq_pending = 1;
    s_irq_tick = HAL_GetTick();
  }
}

uint8_t Photo_Task(void)
{
  uint32_t now = HAL_GetTick();
  uint8_t changed = 0;

  /* 1) EXTI 기반 */
  if (s_irq_pending)
  {
    if (now - s_irq_tick < PHOTO_DEBOUNCE_MS) return 0;
    s_irq_pending = 0;

    uint8_t n1,n2,n3;
    ReadNow(&n1,&n2,&n3);

    if (n1!=s_p1 || n2!=s_p2 || n3!=s_p3)
    {
      s_p1=n1; s_p2=n2; s_p3=n3;
      changed = 1;
    }
  }

  /* 2) 백업 폴링(EXTI miss 대비) */
  static uint32_t pollTick = 0;
  if (now - pollTick >= 50)
  {
    pollTick = now;

    uint8_t n1,n2,n3;
    ReadNow(&n1,&n2,&n3);

    if (n1!=s_p1 || n2!=s_p2 || n3!=s_p3)
    {
      s_p1=n1; s_p2=n2; s_p3=n3;
      changed = 1;
    }
  }

  return changed;
}

void Photo_GetRaw(uint8_t *p1, uint8_t *p2, uint8_t *p3)
{
  if (p1) *p1 = s_p1;
  if (p2) *p2 = s_p2;
  if (p3) *p3 = s_p3;
}

photo_fsm_t Photo_GetFSM(void)
{
  if (!s_valid) return PF_UNKNOWN;
  return DecodeFSM(s_p1, s_p2, s_p3);
}

const char* Photo_FSM_ToString(photo_fsm_t f)
{
  switch (f)
  {
    case PF_F1:       return "FLOOR=1";
    case PF_F2:       return "FLOOR=2";
    case PF_F3:       return "FLOOR=3";
    case PF_MOVE_1_2: return "MOVE 1~2";
    case PF_MOVE_2_3: return "MOVE 2~3";
    case PF_UNKNOWN:  return "MOVING";
    case PF_ERROR:    return "ERROR";
    default:          return "?";
  }
}
