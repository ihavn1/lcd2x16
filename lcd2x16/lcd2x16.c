/*
* lcd2x16.c
*
* Created: 27/10/2024 12.53.22
*  Author: ib
*
*	LCD Data Sheet: https://components101.com/sites/default/files/component_datasheet/16x2%20LCD%20Datasheet.pdf
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <util/delay.h>


#include "lcd2x16.h"
#include "twi/twi.h"

// Defines
#define LCD_E_BIT					0b00000100
#define LCD_R_BIT					0b00000010
#define LCD_DATA_REG_BIT			0b00000001
#define LCD_BACKLIGHT_BIT			0b00001000
#define LCD_ENTRY_SET_BIT 0b00000100;
#define LCD_AUTO_SCROLL_ENTRY_SET_BIT	0b00000001;
#define LCD_LEFT_TO_RIGHT_ENTRY_SET_BIT	0b00000010;
#define LCD_DISPLAY_STATE_SET_BIT 0b00001000;
#define LCD_DISPLAY_ON_DISPLAY_STATE_SET_BIT 0b00000100;
#define LCD_CURSOR_ON_DISPLAY_STATE_SET_BIT	0b00000010;
#define LCD_CURSOR_BLINK_DISPLAY_STATE_SET_BIT	0b00000001;

// Prototypes
void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen);
void _lcdByteWrite(lcd_t self, uint8_t byte);
void _twiByteWrite(lcd_t self, uint8_t byte);
void _lcdWrite(lcd_t self, uint8_t data, bool function);
void _lcdInit(lcd_t self);

// --------------------------------------------------
struct lcd {
	uint8_t _twiAddress;
	uint8_t _maxColumns;
	uint8_t _maxRows;
	uint8_t backLiteBit;
	uint8_t entryModeSet;
	uint8_t displayStateSet;
	twi_p twiInstance;
};

// --------------------------------------------------
lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows) {
	lcd_t newLcd = (lcd_t)malloc(sizeof(struct lcd));
	newLcd->_twiAddress = twiAddress;
	newLcd->_maxColumns = maxColumns;
	newLcd->_maxRows = maxRows;
	newLcd->backLiteBit = 0;
	newLcd->entryModeSet = LCD_ENTRY_SET_BIT;
	newLcd->displayStateSet = LCD_DISPLAY_STATE_SET_BIT;
	newLcd->twiInstance = NULL;
	
	return newLcd;
}

// --------------------------------------------------
void lcdBegin(lcd_t self) {
	self->twiInstance = twiCreate(self->_twiAddress, twi100kHz, F_CPU, NULL);
	_twiByteWrite(self, 0x00); // Clear shift register on LCD
	
	_delay_ms(50); // Wait for Power On off LCD
	
	_lcdInit(self);
}

// --------------------------------------------------
void lcdBackLight(lcd_t self, bool newState) {
	self->backLiteBit = (newState ? LCD_BACKLIGHT_BIT : 0);
	_twiByteWrite(self, self->backLiteBit); 
}

// --------------------------------------------------
void lcdAutoScroll(lcd_t self, bool newState) {
	if (true == newState) {
		self->entryModeSet |= LCD_AUTO_SCROLL_ENTRY_SET_BIT;
	}
	else {
		self->entryModeSet &= ~LCD_AUTO_SCROLL_ENTRY_SET_BIT;
	}
	
	_lcdWrite(self, self->entryModeSet, true);
}

// --------------------------------------------------
void lcdLeftToRight(lcd_t self, bool newState) {
	if (true == newState) {
		self->entryModeSet |= LCD_LEFT_TO_RIGHT_ENTRY_SET_BIT;
	}
	else {
		self->entryModeSet &= ~LCD_AUTO_SCROLL_ENTRY_SET_BIT;
	}
		
	_lcdWrite(self, self->entryModeSet, true);
}

// --------------------------------------------------
void lcdDisplayOn(lcd_t self, bool newState) {
	if (true == newState) {
		self->displayStateSet |= LCD_DISPLAY_ON_DISPLAY_STATE_SET_BIT;
	}
	else {
		self->displayStateSet &= ~LCD_DISPLAY_ON_DISPLAY_STATE_SET_BIT;
	}
		
	_lcdWrite(self, self->displayStateSet, true);
}

// --------------------------------------------------
void lcdCursorOn(lcd_t self, bool newState) {
	if (true == newState) {
		self->displayStateSet |= LCD_CURSOR_ON_DISPLAY_STATE_SET_BIT;
	}
	else {
		self->displayStateSet &= ~LCD_CURSOR_ON_DISPLAY_STATE_SET_BIT;
	}
		
	_lcdWrite(self, self->displayStateSet, true);
}

// --------------------------------------------------
void lcdCursorBlink(lcd_t self, bool newState) {
	if (true == newState) {
		self->displayStateSet |= LCD_CURSOR_BLINK_DISPLAY_STATE_SET_BIT;
	}
	else {
		self->displayStateSet &= ~LCD_CURSOR_BLINK_DISPLAY_STATE_SET_BIT;
	}
	
	_lcdWrite(self, self->displayStateSet, true);
}

// --------------------------------------------------
void lcdClear(lcd_t self){
	_lcdWrite(self,0b00000001, true);
	_delay_us(1600);
}

// --------------------------------------------------
void lcdGoto(lcd_t self, uint8_t col, uint8_t row) {
	static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	if(col > self->_maxColumns) { col = self->_maxColumns; } // sanity limits
	if(row > self->_maxRows) { row = self->_maxRows; } // sanity limits

	uint8_t newAddress = row_offsets[row] + col;

	_lcdWrite(self, 0b10000000 | newAddress, true);
}

// --------------------------------------------------
void lcdPrintChar(lcd_t self, char character) {
	_lcdWrite(self, character, false);
}

// --------------------------------------------------
void lcdPrint(lcd_t self, char *zstring) {
	for (uint8_t i = 0; i < self->_maxColumns; i++) {
		if (0 == zstring[i]) {
			return;
		}
		
		lcdPrintChar(self, zstring[i]);
	}
}

// --------------------------------------------------
void lcdPrintAt(lcd_t self, uint8_t col, uint8_t row, char *zstring) {
	lcdGoto(self,col,row);
	lcdPrint(self, zstring);
}

// --------------------------------------------------
void _lcdInit(lcd_t self) {
	// Initialize acording to data sheet p.16 (https://www.vishay.com/docs/37484/lcd016n002bcfhet.pdf)
	_lcdByteWrite(self, 0b00110000); // DB5 and DB4
	_delay_us(4200);
	_lcdByteWrite(self, 0b00110000); // DB5 and DB4
	_lcdByteWrite(self, 0b00110000); // DB5 and DB4
	_lcdByteWrite(self, 0b00100000); // Function set: Data interface len = 4 bit

// From now on the display only accept 4 bit data
 	_lcdWrite(self, 0b00101000, true); // Function set: Data interface len = 4 bit, Font 5x8, 2 lines
	
	lcdClear(self);
	lcdLeftToRight(self, true);
	lcdDisplayOn(self, true);
}

// --------------------------------------------------
void _lcdByteWrite(lcd_t self, uint8_t byte) {
	uint8_t _outputByte = byte| LCD_E_BIT;
	
	while (twiIsBusy()) {}
	twiTransmit(self->twiInstance, &_outputByte, 1);
	
	_outputByte &= ~LCD_E_BIT;
	while (twiIsBusy()) {}
	twiTransmit(self->twiInstance, &_outputByte, 1);
}

// --------------------------------------------------
void _twiByteWrite(lcd_t self, uint8_t byte) {
	while (twiIsBusy()) {}
	twiTransmit(self->twiInstance, &byte, 1);
}

// --------------------------------------------------
void _lcdWrite(lcd_t self, uint8_t data, bool function) {
	uint8_t _outputByte = data & 0xF0;  // Prepare high nibble
	
	if (!function) {
		_outputByte |= LCD_DATA_REG_BIT;
	}
	
	_outputByte |= self->backLiteBit;
	
	// High nibble
	_lcdByteWrite(self, _outputByte);
	
	// Low nibble
	_outputByte &= 0x0F;
	_outputByte |= (data & 0x0F) << 4;
	_lcdByteWrite(self, _outputByte);
}