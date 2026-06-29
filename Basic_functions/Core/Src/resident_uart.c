/*
 * resident_uart.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#include "resident_uart.h"
#include "logger.h"
#include "elevator.h"
#include "photo.h"
#include "servo.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static UART_HandleTypeDef *s_huart = 0;

static uint8_t  s_rx_ch = 0;
static char     s_line[64];
static uint8_t  s_len = 0;

static void StartRxIT(void)
{
  if (!s_huart) return;
  HAL_UART_Receive_IT(s_huart, &s_rx_ch, 1);
}

/* ====== 상태 문자열 ====== */
static const char* StateToStr(ELEVATOR_STATE st)
{
  switch (st)
  {
    case ELEVATOR_IDLE:         return "IDLE";
    case ELEVATOR_MOVING_UP:    return "MOVING_UP";
    case ELEVATOR_MOVING_DOWN:  return "MOVING_DOWN";
    case ELEVATOR_DOOR_OPENING: return "DOOR_OPENING";
    case ELEVATOR_DOOR_WAIT:    return "DOOR_OPEN";
    case ELEVATOR_DOOR_CLOSING: return "DOOR_CLOSING";
    case ELEVATOR_EMG:          return "EMG_STOP";
    default:                    return "?";
  }
}

static const char* PhotoToStr(photo_fsm_t f)
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

// 거주자 UI 용
static const char* SimpleStateStr(photo_fsm_t f)
{
  switch (f)
  {
    case PF_F1:       return "FLOOR=1";
    case PF_F2:       return "FLOOR=2";
    case PF_F3:       return "FLOOR=3";
    case PF_MOVE_1_2: return "MOVING";
    case PF_MOVE_2_3: return "MOVING";
    case PF_UNKNOWN:  return "MOVING";
    case PF_ERROR:    return "ERROR";
    default:          return "MOVING";
  }
}

static void Resident_AutoSendSimpleState(void)
{
  static photo_fsm_t last = (photo_fsm_t)0xFF;

  photo_fsm_t now = Photo_GetFSM();

  // 변할 때만 출력
  if (now != last)
  {
    last = now;
    Log_Printf("%s\r\n", SimpleStateStr(now));
  }
}



static void PrintHelp(void)
{
  Log_Printf(
    "CMD:\r\n"
    "  CALL 1|2|3\r\n"
    "  STATUS\r\n"
    "  RESUME\r\n"
    "  HELP\r\n"
  );
}

static void PrintStatus(void)
{
  uint8_t p1,p2,p3;
  Photo_GetRaw(&p1,&p2,&p3);
  photo_fsm_t pf = Photo_GetFSM();

  uint8_t cur = Elevator_GetCurrentFloor();
  ELEVATOR_STATE st = Elevator_GetState();
  const char *door = Servo_IsOpened() ? "OPEN" : (Servo_IsClosed() ? "CLOSE" : "HOLD");

  /* 큐(요청) 출력 */
  char qbuf[64];
  Elevator_GetQueueString(qbuf, sizeof(qbuf));

  Log_Printf("RAW=%u%u%u %s\r\n", p1,p2,p3, PhotoToStr(pf));
  Log_Printf("FLOOR=%u\r\n", cur);
  Log_Printf("STATE=%s\r\n", StateToStr(st));
  Log_Printf("DOOR=%s\r\n", door);
  Log_Printf("QUEUE=%s\r\n", qbuf);
}

static void StrToUpper(char *s)
{
  for (; *s; s++) *s = (char)toupper((unsigned char)*s);
}

static void HandleLine(char *line)
{
  while (*line==' ' || *line=='\t') line++;
  if (*line == 0) return;

  char tmp[64];
  strncpy(tmp, line, sizeof(tmp)-1);
  tmp[sizeof(tmp)-1] = 0;
  StrToUpper(tmp);

  if (!strncmp(tmp, "HELP", 4))
  {
    PrintHelp();
    return;
  }

  if (!strncmp(tmp, "STATUS", 6))
  {
    PrintStatus();
    return;
  }

  if (!strncmp(tmp, "RESUME", 6))
  {
    Elevator_ResumeFromEMG();
    Log_Printf("RESUME OK\r\n");
    return;
  }

  if (!strncmp(tmp, "CALL", 4))
  {
    char *p = tmp + 4;
    while (*p==' ' || *p=='\t') p++;
    int f = atoi(p);
    if (f < 1 || f > 3)
    {
      Log_Printf("ERR: CALL 1|2|3\r\n");
      return;
    }
    Elevator_RequestCar((uint8_t)f);
    Log_Printf("OK: CALL %d\r\n", f);
    return;
  }

  Log_Printf("ERR: UNKNOWN CMD (HELP)\r\n");
}

void ResidentUART_Init(UART_HandleTypeDef *huart)
{
  s_huart = huart;
  s_len = 0;
  memset(s_line, 0, sizeof(s_line));
  StartRxIT();
  Log_Printf("UART2 CMD READY\r\n");
}

void ResidentUART_Task(void)
{
  Resident_AutoSendSimpleState();

  /* 수신은 인터럽트 콜백에서 s_line에 쌓임. 여기서는 할 일 없음 */
}

/* ====== 반드시 main.c 또는 stm32xx_it.c에 추가 ======
   아래 콜백을 프로젝트 어딘가(전역) 1개만 두면 됨.
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart2)
{
  if (s_huart && huart2 == s_huart)
  {
    char c = (char)s_rx_ch;

    if (c == '\r' || c == '\n')
    {
      if (s_len > 0)
      {
        s_line[s_len] = 0;
        HandleLine(s_line);
        s_len = 0;
      }
    }
    else
    {
      if (s_len < (sizeof(s_line)-1))
        s_line[s_len++] = c;
      else
        s_len = 0; // overflow 방지: 라인 리셋
    }

    StartRxIT();
  }
}
