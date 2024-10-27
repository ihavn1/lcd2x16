/*
 * lcd2x16.c
 *
 * Created: 27/10/2024 12.53.22
 *  Author: ib
 */ 

#include "lcd2x16.h"

#include "twi/twi.h"

void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen);

struct lcd {
	uint8_t _twiAddress;
	uint8_t _maxColumns;
	uint8_t _maxRows;
	twi_p twiInstance;
	};
	
lcd_t lcdCreate(uint8_t twiAddress, uint8_t maxColumns, uint8_t maxRows) {
		lcd_t newLcd = (lcd_t)malloc(sizeof(struct lcd));
		lcd->_twiAddress = twiAddress;
		lcd->_maxColumns = maxColumns;
		lcd->_maxRows = maxRows;
		lcd->twiInstance = NULL;
}

void lcdBegin(lcd_t self) {
	self->twiInstance twiCreate(self->_twiAddress twiClock_t twi100kHz, uint32_t F_CPU, void(*_twiCallback)(twi_p, twiReturnCode_t, uint8_t *, uint8_t));
}


void _twiCallback(twi_p twiInstance, twiReturnCode_t twiRc, uint8_t * twiMessageBuf, uint8_t twiMessageLen) {
	
}