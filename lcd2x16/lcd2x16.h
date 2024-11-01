/*
 * lcd2x16.h
 *
 * Created: 27/10/2024 12.45.01
 *  Author: ib
 */ 


#ifndef LCD2X16_H_
#define LCD2X16_H_
#include <stdint.h>
#include <stdbool.h>

typedef struct lcd * lcd_t;

lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows);
void lcdBegin(lcd_t self);
void lcdClear(lcd_t self);
void lcdBackLight(lcd_t self, bool newState);
void lcdSetCursor(lcd_t self, uint8_t col, uint8_t row);

void lcdWrite(lcd_t self, char character);


#endif /* LCD2X16_H_ */