/*
 * lcd2x16
 *
 * Created: 27/10/2024 12.24.11
 * Author : ib
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include "lcd2x16.h"

int main(void)
{
	lcd_t lcd = 0;
	
	lcd = lcdCreate(0x27, 16, 2);
	sei();
	
	lcdBegin(lcd);
	lcdBackLight(lcd, 1);
	lcdCursorOn(lcd, 0);
	lcdCursorBlink(lcd, 0);

	lcdGoto(lcd,0,0);
	lcdPrint(lcd, "0123456789ABCDEF");
	//lcdPrint(lcd,"Peter Pan");
	//lcdPrint(lcd,"Ole Olsen");
	//lcdPrintChar(lcd,'A');
	
    while (1) 
    {


		
    }
}

