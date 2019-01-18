/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 3 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */
#include <avr/io.h>


int main(void) {
    DDRA = 0; PORTA = 0;
	DDRC = 0xFF; PORTC = 0;
	unsigned char tmpA;
	unsigned char tmpC;
    while (1) {
		tmpC = 0;
		tmpA = PORTA & 0x0F;
		if (tmpA >= 13) {
			tmpC = tmpC | 0x3F;
		}
		else if (tmpA >= 10) {
			tmpC = tmpC | 0x3E;
		}
		else if (tmpA >= 7) {
			tmpC = tmpC | 0x3C;
		}
		else if (tmpA >= 5) {
			tmpC = tmpC | 0x34;
		}
		else if (tmpA >= 3) {
			tmpC = tmpC | 0x70;
		}
		else if (tmpA >= 0) {
			tmpC = tmpC | 0x60;
		}
		else {
			tmpC = tmpC | 0x40;
		}
		PORTC = tmpC;
    }
}

