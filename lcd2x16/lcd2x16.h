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
void lcdAutoScroll(lcd_t self, bool newState);  
void lcdLeftToRight(lcd_t self, bool newState);
void lcdDisplayOn(lcd_t self, bool newState);
void lcdCursorOn(lcd_t self, bool newState);
void lcdCursorBlink(lcd_t self, bool newState);
void lcdGoto(lcd_t self, uint8_t col, uint8_t row);
void lcdPrintChar(lcd_t self, char character);
void lcdPrint(lcd_t self, char *zstring);
void lcdPrintAt(lcd_t self, uint8_t col, uint8_t row, char *zstring);

#endif /* LCD2X16_H_ */