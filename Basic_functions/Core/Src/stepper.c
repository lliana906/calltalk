/*
 * stepper.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */




#include "stepper.h"

static const uint8_t HALF_STEP_SEQ[8][4] =
{
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1}
};

static uint8_t  s_stepIndex = 0;
static uint32_t s_prevTick  = 0;
static volatile bool s_busy = false;
static volatile uint8_t s_dir = DIR_UP;

#define STEP_PERIOD_MS  2

static void Stepper_WritePhase(uint8_t phase)
{
  HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, HALF_STEP_SEQ[phase][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN2_PORT, IN2_PIN, HALF_STEP_SEQ[phase][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN3_PORT, IN3_PIN, HALF_STEP_SEQ[phase][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN4_PORT, IN4_PIN, HALF_STEP_SEQ[phase][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Stepper_Init(void)
{
  s_stepIndex = 0;
  s_prevTick = HAL_GetTick();
  s_busy = false;
  s_dir = DIR_UP;
  Stepper_WritePhase(0);
}

void Stepper_StartContinuous(uint8_t dir)
{
  s_dir = dir;
  s_busy = true;
}

void Stepper_Stop(void)
{
  s_busy = false;
  Stepper_WritePhase(0);
}

bool Stepper_IsBusy(void) { return s_busy; }

void Stepper_Task(void)
{
  if (!s_busy) return;

  uint32_t now = HAL_GetTick();
  if (now - s_prevTick < STEP_PERIOD_MS) return;
  s_prevTick = now;

  /* ✅ 방향 반대는 여기에서만 바꿔 (엘리베이터 로직에서 phase 뒤집지 마) */
  if (s_dir == DIR_UP)
    s_stepIndex = (s_stepIndex + 1) & 0x07;
  else
    s_stepIndex = (s_stepIndex + 7) & 0x07;   // -1 mod 8

  Stepper_WritePhase(s_stepIndex);
}


