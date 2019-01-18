/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 3 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void) {
	DDRA = 0; PORTA = 0;
	DDRB = 0; PORTB = 0;
	DDRC = 0xFF; PORTC = 0;
	unsigned short count = 0;
    /* Replace with your application code */
    while (1) {
		count = 0;
		count = (PORTA & 0x01) + ((PORTA & 0x02) >> 1) + ((PORTA & 0x04) >> 2) + ((PORTA & 0x08) >> 3)
				+ ((PORTA & 0x10) >> 4) + ((PORTA & 0x20) >> 5) + ((PORTA & 0x40) >> 6) + ((PORTA & 0x80) >> 7)
				+ (PORTB & 0x01) + ((PORTB & 0x02) >> 1) + ((PORTB & 0x04) >> 2) + ((PORTB & 0x08) >> 3)
				+ ((PORTB & 0x10) >> 4) + ((PORTB & 0x20) >> 5) + ((PORTB & 0x40) >> 6) + ((PORTB & 0x80) >> 7);
		PORTC = count;
    }
}

