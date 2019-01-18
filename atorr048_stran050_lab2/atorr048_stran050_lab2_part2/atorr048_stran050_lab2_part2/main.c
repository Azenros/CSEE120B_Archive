/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 2 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void) {
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char count = 0x00;
	/* Replace with your application code */
	while (1) {
		tmpA = PORTA;
		count = 0;
		if ((tmpA & 0x01) == 0x00) { count++; }
		if ((tmpA & 0x02) == 0x00) { count++; }
		if ((tmpA & 0x04) == 0x00) { count++; }
		if ((tmpA & 0x08) == 0x00) { count++; }
		PORTC = count;
	}
}

