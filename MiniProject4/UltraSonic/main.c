/*
 * main.c
 *
 *  Created on: Oct 21, 2023
 *      Author: Mahmoud Khalafallah
 */

#include "LCD/lcd.h"
#include "UltraSonic/ultrasonic.h"
#include <avr/io.h>

int main(void) {

	LCD_Init(); /*Initialize LCD 8 bit Mode 16*4*/
	Ultrasonic_init(); /*Initialize ultrasonic*/
	SREG |= 1 << 7; /*Enable global Interrupt*/

	LCD_displayString("Distance= ");
	uint16 d = Ultrasonic_readDistance();

	while (1) {

		LCD_SetCursor(0, 10);
		LCD_PrintNum16Bit(d);
		LCD_displayString(" cm ");
		d = Ultrasonic_readDistance();
	}
}
