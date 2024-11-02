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
#define E_BIT 0b00000100
#define R_BIT 0b00000010
#define DATA_REG_BIT 0b00000001
#define BACKLIGHT_BIT 0b00001000

// Prototypes
void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen);
twiReturnCode_t _twiByteWrite(lcd_t self, uint8_t byte);
twiReturnCode_t _twiByteRaw(lcd_t self, uint8_t byte);
twiReturnCode_t _lcdWrite(lcd_t self, uint8_t data, bool function);
void _lcdInit(lcd_t self);

// --------------------------------------------------
struct lcd {
	uint8_t _twiAddress;
	uint8_t _maxColumns;
	uint8_t _maxRows;
	uint8_t backLiteBit;
	twi_p twiInstance;
};

// --------------------------------------------------
lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows) {
	lcd_t newLcd = (lcd_t)malloc(sizeof(struct lcd));
	newLcd->_twiAddress = twiAddress;
	newLcd->_maxColumns = maxColumns;
	newLcd->_maxRows = maxRows;
	newLcd->backLiteBit = 0;
	newLcd->twiInstance = NULL;
	
	return newLcd;
}

// --------------------------------------------------
void lcdBegin(lcd_t self) {
	self->twiInstance = twiCreate(self->_twiAddress, twi100kHz, F_CPU, NULL);
	_twiByteRaw(self, 0x00); // Clear shift register on LCD
	
	_delay_ms(50); // Wait for Power On off LCD
	
	_lcdInit(self);
}

// --------------------------------------------------
void lcdBackLight(lcd_t self, bool newState) {
	self->backLiteBit = (newState ? BACKLIGHT_BIT : 0);
	_twiByteRaw(self, self->backLiteBit); 
}

// --------------------------------------------------
void lcdClear(lcd_t self){
	_lcdWrite(self,0b00000001, true);
	_delay_us(1600);
}

// --------------------------------------------------
void lcdSetCursor(lcd_t self, uint8_t col, uint8_t row) {
	static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	if(col > self->_maxColumns) { col = self->_maxColumns; } // sanity limits
	if(row > self->_maxRows) { row = self->_maxRows; } // sanity limits

	uint8_t newAddress = row_offsets[row] + col;

	_lcdWrite(self, 0b10000000 | newAddress, true);
}


// --------------------------------------------------
void lcdWrite(lcd_t self, char character) {
	_lcdWrite(self, character, false);
}

// --------------------------------------------------
void _lcdInit(lcd_t self) {
	// Initialize acording to data sheet p.16 (https://www.vishay.com/docs/37484/lcd016n002bcfhet.pdf)
	_twiByteWrite(self, 0b00110000); // DB5 and DB4
	// vvvvvvvvvvvvvvvv
	_delay_us(4200);
	_twiByteWrite(self, 0b00110000); // DB5 and DB4
	_twiByteWrite(self, 0b00110000); // DB5 and DB4
	_twiByteWrite(self, 0b00100000); // Function set: Data interface len = 4 bit

// From now on the display only accept 4 bit data
 	_lcdWrite(self, 0b00101000, true); // Function set: Data interface len = 4 bit, Font 5x8, 2 lines
	_lcdWrite(self, 0b00001100,true); // Display ON/Off ?????
	
	lcdClear(self);
 	
 	_lcdWrite(self, 0b00000110, true); // Left to right
}

// --------------------------------------------------
twiReturnCode_t _twiByteWrite(lcd_t self, uint8_t byte) {
	uint8_t _outputByte = byte| E_BIT;
	
	while (twiIsBusy()) {}
	twiReturnCode_t rc = twiTransmit(self->twiInstance, &_outputByte, 1);
	
	_outputByte &= ~E_BIT;
	while (twiIsBusy()) {}
	twiTransmit(self->twiInstance, &_outputByte, 1);
	
	return rc;  // What to return ????
}

// --------------------------------------------------
twiReturnCode_t _twiByteRaw(lcd_t self, uint8_t byte) {
	twiTransmit(self->twiInstance, &byte, 1);
}

// --------------------------------------------------
twiReturnCode_t _lcdWrite(lcd_t self, uint8_t data, bool function) {
	uint8_t _outputByte = data & 0xF0;  // Prepare high nibble
	
	if (!function) {
		_outputByte |= DATA_REG_BIT;
	}
	
	_outputByte |= self->backLiteBit;
	
	// High nibble
	_twiByteWrite(self, _outputByte);
	
	// Low nibble
	_outputByte &= 0x0F;
	_outputByte |= (data & 0x0F) << 4;
	_twiByteWrite(self, _outputByte);
	
	return TWI_OK;  // What to return ????
}