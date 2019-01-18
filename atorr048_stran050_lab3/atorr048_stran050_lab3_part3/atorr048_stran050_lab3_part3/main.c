/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 3 Exercise # 3
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void) {
	DDRA = 0; PORTA = 0;
	DDRC = 0xFF; PORTC = 0;
	unsigned char tmpA;
	unsigned char gas;
	unsigned char tmpC;
	while (1) {
		tmpC = 0;
		tmpA = PORTA;
		gas = tmpA & 0x0F;
		// if (gas >= 13) ? tmpC = tmpC | 0x3F : (gas >= 10) ? tmpC = tmpC | 0x3E : (gas >= 7) ? tmpC = tmpC |
		//	0x3C : (gas >= 5) ? tmpC = tmpC | 0x34 : (gas >= 3) ? tmpC = tmpC | 0x70 : (gas >= 0) ? tmpC = tmpC | 0x60 : tmpC = tmpC | 0x40;
		if (gas >= 13) {
			tmpC = tmpC | 0x3F;
		}
		else if (gas >= 10) {
			tmpC = tmpC | 0x3E;
		}
		else if (gas >= 7) {
			tmpC = tmpC | 0x3C;
		}
		else if (gas >= 5) {
			tmpC = tmpC | 0x34;
		}
		else if (gas >= 3) {
			tmpC = tmpC | 0x70;
		}
		else if (gas > 0) {
			tmpC = tmpC | 0x60;
		}
		else {
			tmpC = tmpC | 0x40;
		}
		
		if ((tmpA & 0x30) == 0x30) {
			tmpC = tmpC | 0x80;
		}
		PORTC = tmpC;
	}
}

