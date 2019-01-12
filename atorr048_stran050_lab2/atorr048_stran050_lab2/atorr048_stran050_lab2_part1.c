/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 2 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */


#include <avr/io.h>


int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char tmpA = 0x00;
	unsigned char tmpB = 0x00;
    while (1) {
		tmpA = PORTA;
		if (tmpA == 0x03) {
			tmpB = 0x00;
		}
		else if (tmpA == 0x01) {
			tmpB = 0x01;
		}
		else {
			tmpB = 0x00;
		}
		PORTB = tmpB;
	}
}

