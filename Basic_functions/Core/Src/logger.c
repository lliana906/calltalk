/*
 * logger.c
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 */


#include "logger.h"
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef *s_huart = 0;

void Log_Init(UART_HandleTypeDef *huart)
{
  s_huart = huart;
}

void Log_Printf(const char *fmt, ...)
{
  if (!s_huart) return;

  char buf[160];

  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  if (n <= 0) return;

  // vsnprintf가 잘려도 n은 "원래 필요 길이"를 줄 수 있음 → 안전 클램프
  if (n >= (int)sizeof(buf))
  {
    n = (int)sizeof(buf) - 1;
    buf[n] = '\0';
  }

  HAL_UART_Transmit(s_huart, (uint8_t*)buf, (uint16_t)n, 50);
}

