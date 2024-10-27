/*
 * lcd2x16.h
 *
 * Created: 27/10/2024 12.45.01
 *  Author: ib
 */ 


#ifndef LCD2X16_H_
#define LCD2X16_H_
#include <stdint.h>

typedef struct lcd * lcd_t;

lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows);
void lcdBackLight(lcd_t self, bool newState);


#endif /* LCD2X16_H_ */