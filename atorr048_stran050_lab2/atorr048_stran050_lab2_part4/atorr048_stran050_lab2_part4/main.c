/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 2 Exercise # 4
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	unsigned short tmpA = 0;
	unsigned short tmpB = 0;
	unsigned short tmpC = 0;
	unsigned short tmpD = 0;
	unsigned short diff = 0;
	unsigned long add = 0;
    /* Replace with your application code */
    while (1) {
		tmpA = PORTA;
		tmpB = PORTB;
		tmpC = PORTC;
		add = tmpA + tmpB + tmpC;
		if (tmpA >= tmpC) {
			diff = tmpA - tmpC;
		}
		else {
			diff = tmpC - tmpA;
		}
		if (add > 140) {
			tmpD = tmpD | 0x01;
		}
		if (diff >= 80) {
			tmpD = tmpD | 0x02;
		}

		PORTD = (add & 0xFC) | tmpD;
		 
    }
}

