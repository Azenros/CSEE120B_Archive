/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 5 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */
#include <avr/io.h>


int main(void) {
    DDRA = 0x00; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char tmpA;
	unsigned char tmpC;
	unsigned char button1;
	unsigned char button2;
	unsigned char button3;
	unsigned char button4;
    while (1) {
		tmpC = 0;
		button1 = PINA & 0x01;
		button2 = PINA & 0x02;
		button3 = PINA & 0x04;
		button4 = PINA & 0x08; 
		tmpA = button1 | button2 | button3 | button4;
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

