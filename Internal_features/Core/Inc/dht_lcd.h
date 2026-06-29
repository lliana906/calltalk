/*
 * dht_bz.h
 *
 *  Created on: Feb 7, 2026
 *      Author: kimyujeong
 */

#ifndef INC_DHT_LCD_H_
#define INC_DHT_LCD_H_

#include <stdint.h>
#include "dht11.h"


void dht_lcd_Init(void);
void dht_lcd_Update(uint8_t temp, uint8_t humi);




#endif /* INC_DHT_LCD_H_ */
