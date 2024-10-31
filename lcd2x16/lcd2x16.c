/*
 * lcd2x16.c
 *
 * Created: 27/10/2024 12.53.22
 *  Author: ib
 */ 

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <util/delay.h>


#include "lcd2x16.h"
#include "twi/twi.h"

// Defines
#define E_BIT 0b00000100;
#define W_BIT 0b00000010;
#define DATA_REG_BIT 0b00000001;
#define BACKLIGHT_BIT 0b00001000;

// Prototypes
void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen);
twiReturnCode_t _twiByteWrite(lcd_t self, uint8_t byte);
twiReturnCode_t _lcdWrite(lcd_t self, uint8_t data, bool function);
void _lcdInit(lcd_t self);
void _lcdToogleE(lcd_t self);

static volatile bool twiReady = false;


// --------------------------------------------------
struct lcd {
	uint8_t _twiAddress;
	uint8_t _maxColumns;
	uint8_t _maxRows;
	twi_p twiInstance;
	};

// --------------------------------------------------	
lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows) {
		lcd_t newLcd = (lcd_t)malloc(sizeof(struct lcd));
		newLcd->_twiAddress = twiAddress;
		newLcd->_maxColumns = maxColumns;
		newLcd->_maxRows = maxRows;
		newLcd->twiInstance = NULL;
		
		return newLcd;
}

// --------------------------------------------------
void lcdBegin(lcd_t self) {
	self->twiInstance = twiCreate(self->_twiAddress, twi100kHz, F_CPU, _twiCallback);
	// Clear I2C adapter on LCD
	//_twiByteWrite(self, 0x00);
	
	// DELAY 40 mS ?????
	_delay_ms(40);
	
	_lcdInit(self);
}

// --------------------------------------------------
void _lcdInit(lcd_t self) {
	// Initialize acording to data sheet p.16 (https://www.vishay.com/docs/37484/lcd016n002bcfhet.pdf)
	_twiByteWrite(self, 0b00110000); // DB5 and DB4
	while (!twiReady) {}
	_twiByteWrite(self, 0b00100000); // Function set: Data interface len = 4 bit
	while (!twiReady) {}
	
	_delay_us(50); // ????
	// From now on the display only accept 4 bit data
}

// --------------------------------------------------
void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen) {
	twiReady = ( twiRc == TWI_TRANSMIT_OK ? true : false);
	
}

// --------------------------------------------------
// Helper functions
twiReturnCode_t _twiByteWrite(lcd_t self, uint8_t byte) {
	uint8_t _outputByte = byte;
	
	twiReturnCode_t rc = twiTransmit(self->twiInstance, &_outputByte, 1);
/*	_outputByte |= E_BIT;
	twiTransmit(self->twiInstance, &_outputByte, 1);
	_outputByte &= ~E_BIT;
	twiTransmit(self->twiInstance, &_outputByte, 1);
*/	
	return rc;  // What to return ????
}

// --------------------------------------------------
twiReturnCode_t _lcdWrite(lcd_t self, uint8_t data, bool function) {
		uint8_t _outputByte = data & 0xF0;  // Prepare high nibble
		
		if (!function) {
			_outputByte |= 0b00000001; // Set RS bit => Data
		}
		
		// High nibble
		_twiByteWrite(self, _outputByte);
		
		// Low nibble
		_outputByte &= 0x0F;
		_outputByte |= (data & 0x0F) << 4;
		_twiByteWrite(self, _outputByte);
		
		return TWI_OK;  // What to return ????
}