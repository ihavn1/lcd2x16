/*
 * lcd2x16
 *
 * Created: 27/10/2024 12.24.11
 * Author : ib
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "lcd2x16.h"

int main(void)
{
	lcd_t lcd = 0;
	
	lcd = lcdCreate(0x27, 16, 2);
	sei();
	
	lcdBegin(lcd);
	
    while (1) 
    {
		DDRA = 0;
    }
}

