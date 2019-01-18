/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 3 Exercise # 4
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void) {
    /* Replace with your application code */
	DDRA = 0; PORTA = 0;
	DDRB = 0xFF; PORTB = 0;
	DDRC = 0xFF; PORTC = 0;
	unsigned char topNibbleA;
	unsigned char botNibbleA;
	
    while (1) {
		topNibbleA = PORTA & 0xF0;
		botNibbleA = PORTA & 0x0F;
		
		PORTB = topNibbleA >> 4;
		PORTC = botNibbleA << 4;
    }
}

