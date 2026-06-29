#include "dht11.h"


void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pin)
{
	// 구조체의 포트와 핀 설정
	dht -> port = port;		// GPIO 포트 설정
	dht -> pin = pin;			// 핀 번호 설정
}


// 핀모드 설정
void dht11GpioMode(DHT11 *dht, uint8_t mode)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};			// GPIO 초기화 구조체 선언

  // 출력 모드 설정
  if(mode == OUTPUT)
	{
  	  GPIO_InitStruct.Pin = dht -> pin;
  	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	  HAL_GPIO_Init(dht -> port, &GPIO_InitStruct);
	}
  else if(mode == INPUT)
	{
  	  GPIO_InitStruct.Pin = dht -> pin;
  	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	  GPIO_InitStruct.Pull = GPIO_NOPULL;
  	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  	  HAL_GPIO_Init(dht -> port, &GPIO_InitStruct);
	}

}

uint8_t dht11Read(DHT11 *dht)
{
    uint16_t timeTick = 0;
    uint8_t pulse[40] = {0};

    uint8_t humValue1 = 0, humValue2 = 0;
    uint8_t tempValue1 = 0, tempValue2 = 0;
    uint8_t parityValue = 0;

    // TIM11: 시간측정용(1us tick 권장), TIM10: delay_us용
    HAL_TIM_Base_Start(&htim11);
    HAL_TIM_Base_Start(&htim10);

    // START 신호
    dht11GpioMode(dht, OUTPUT);
    HAL_GPIO_WritePin(dht->port, dht->pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(dht->port, dht->pin, GPIO_PIN_SET);
    delay_us(20);
    dht11GpioMode(dht, INPUT);

    // 1) DHT 응답: LOW(약 80us) 기다림
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim11) > 200)   // 200us 타임아웃
        {
            printf("Resp: waiting LOW timeout\r\n");
            HAL_TIM_Base_Stop(&htim11);
            return 0;
        }
    }

    // 2) DHT 응답: HIGH(약 80us) 기다림
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim11) > 200)
        {
            printf("Resp: waiting HIGH timeout\r\n");
            HAL_TIM_Base_Stop(&htim11);
            return 0;
        }
    }

    // 3) DHT 응답 HIGH 끝날 때까지 대기
    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim11) > 200)
        {
            printf("Resp: HIGH too long\r\n");
            HAL_TIM_Base_Stop(&htim11);
            return 0;
        }
    }

    // 4) 40비트 수신
    for (uint8_t i = 0; i < 40; i++)
    {
        // 각 비트 시작: LOW(약 50us) 끝나고 HIGH로 올라올 때까지 대기
        __HAL_TIM_SET_COUNTER(&htim11, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_RESET)
        {
            if (__HAL_TIM_GET_COUNTER(&htim11) > 120) // 120us 정도면 충분
            {
                printf("Bit %d: LOW timeout\r\n", i);
                HAL_TIM_Base_Stop(&htim11);
                return 0;
            }
        }

        // HIGH 시간 측정 시작
        __HAL_TIM_SET_COUNTER(&htim11, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pin) == GPIO_PIN_SET)
        {
            if (__HAL_TIM_GET_COUNTER(&htim11) > 120)
            {
                printf("Bit %d: HIGH timeout\r\n", i);
                HAL_TIM_Base_Stop(&htim11);
                return 0;
            }
        }
        timeTick = __HAL_TIM_GET_COUNTER(&htim11);

        // 판정: 0은 약 26~28us, 1은 약 70us
        // 여유있게 threshold만 두는게 안전함
        pulse[i] = (timeTick > 45) ? 1 : 0;
    }

    HAL_TIM_Base_Stop(&htim11);

    // 5) 데이터 조립
    for (uint8_t i = 0; i < 8; i++)  humValue1 = (humValue1 << 1) | pulse[i];
    for (uint8_t i = 8; i < 16; i++) humValue2 = (humValue2 << 1) | pulse[i];
    for (uint8_t i = 16; i < 24; i++) tempValue1 = (tempValue1 << 1) | pulse[i];
    for (uint8_t i = 24; i < 32; i++) tempValue2 = (tempValue2 << 1) | pulse[i];
    for (uint8_t i = 32; i < 40; i++) parityValue = (parityValue << 1) | pulse[i];

    uint8_t checkSum = humValue1 + humValue2 + tempValue1 + tempValue2;
    if (checkSum != parityValue)
    {
        printf("Check Sum Error: %d != %d\r\n", checkSum, parityValue);
        return 0;
    }

    dht->temperature = tempValue1;
    dht->humidity    = humValue1;

    return 1;
}

//uint8_t dht11Read(DHT11 *dht)
//{
//	bool ret = true;
//
//	uint16_t timeTick = 0;		// 시간측정
//	uint8_t pulse[40] = {0};	// 40bit data 저장 공간
//
//	// 온습도 데이터 변수 선언과 동시에 초기화
//	uint8_t humValue1 = 0, humValue2 = 0;
//	uint8_t tempValue1 = 0, tempValue2 = 0;
//	uint8_t parityValue = 0;
//
//	// 타이머 시작
//	HAL_TIM_Base_Start(&htim11);
//
//	// delay_us 때문에 스타트
//	HAL_TIM_Base_Start(&htim10);
//
//	// 통신 시작 신호 전송
//	dht11GpioMode(dht, OUTPUT);		// 출력모드 설정
//	HAL_GPIO_WritePin(dht -> port, dht -> pin, GPIO_PIN_RESET);
//	HAL_Delay(20);
//	HAL_GPIO_WritePin(dht -> port, dht -> pin, GPIO_PIN_SET);
//	delay_us(20);
//
//
//	dht11GpioMode(dht, INPUT);		// 입력모드로 변경
//
//	// DH11의 응답신호 대기
//	__HAL_TIM_SET_COUNTER(&htim11, 0);
//	while(HAL_GPIO_ReadPin(dht -> port, dht -> pin) == GPIO_PIN_RESET)		// LOW 신호까지 대기
//	{
//			if(__HAL_TIM_GET_COUNTER(&htim11) > 100)			// 타임아웃 체크
//			{
//					printf("Low Signal Time O\r\n");					// 타임아웃 오류 출력
//					break;
//			}
//	}
//
//	__HAL_TIM_SET_COUNTER(&htim11, 0);
//	while(HAL_GPIO_ReadPin(dht -> port, dht -> pin) == GPIO_PIN_SET)
//	{
//			if(__HAL_TIM_GET_COUNTER(&htim11) > 100)			// 타임아웃 체크
//			{
//					printf("High Signal Time O\r\n");
//					break;
//			}
//	}
//
//
//	// 위에 문제가 없으면 정상 신호가 들어오는 거임
//	for(uint8_t i = 0; i < 40; i++)
//	{
//		while(HAL_GPIO_ReadPin(dht -> port, dht -> pin) == GPIO_PIN_RESET);	// 50us 대기
//
//		__HAL_TIM_SET_COUNTER(&htim11, 0);
//
//		while(HAL_GPIO_ReadPin(dht -> port, dht -> pin) == GPIO_PIN_SET)
//		{
//				timeTick = __HAL_TIM_GET_COUNTER(&htim11);		// High 시간측정
//
//				//시간 길이에 따른 0 또는 1 판별
//				if(timeTick > 20 && timeTick <30)		// 26~28이면 0
//				{
//						pulse[i] = 0;
//				}
//				else if(timeTick > 65 && timeTick < 85)
//				{
//						pulse[i] = 1;
//				}
//		}
//
//	}
//
//	// 타이머 정지
//	HAL_TIM_Base_Stop(&htim11);
//
//	// 온습도 데이터 처리
//	for(uint8_t i = 0; i < 8; i++)
//	{humValue1 = (humValue1 << 1) + pulse[i];}		// 습도 상위 8비트
//	for(uint8_t i = 8; i < 16; i++)
//	{humValue2 = (humValue2 << 1) + pulse[i];}		// 습도 하위 8비트
//
//	for(uint8_t i = 16; i < 24; i++)
//	{tempValue1 = (tempValue1 << 1) + pulse[i];}		// 온도 상위 8비트
//	for(uint8_t i = 24; i < 32; i++)
//	{tempValue2 = (tempValue2 << 1) + pulse[i];}		// 온도 하위 8비트
//
//	for(uint8_t i = 32; i < 40; i++)
//	{parityValue = (parityValue << 1) + pulse[i];}		//
//
//
//	// 구조체에 온도 및 습도 저장
//	dht -> temperature = tempValue1;
//	dht -> humidity = humValue1;
//
//	// 데이터 무결성 검증
//	uint8_t checkSum = humValue1 + humValue2 + tempValue1 + tempValue2;
//	if(checkSum != parityValue)
//	{
//			printf("Check Sum Error\r\n");
//			ret = false;
//
//	}
//	return ret;
//}
