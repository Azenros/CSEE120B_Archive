/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 5 Exercise # Lab_button
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure PORTA as input, initialize to 1s
	DDRB = 0xFF; PORTB = 0x00; // Configure PORTB as outputs, initialize to 0s
	unsigned char led = 0x00;
	unsigned char button = 0x00;
	while(1)
	{
		// if PA0 is 1, set PB1PB0=01, else =10
		// 1) Read inputs
		button = (~PINA & 0x01) | (~PINA & 0x02) | (~PINA & 0x04); // button is connected to A0
		// 2) Perform Computation
		if (button) { // True if button is pressed
			led = (led & 0xFC) | 0x01; // Sets B to bbbbbb01
			// (clear rightmost 2 bits, then set to 01)
		}
		else {
			led = (led & 0xFC) | 0x02; // Sets B to bbbbbb10
			// (clear rightmost 2 bits, then set to 10)
		}
		// 3) Write output
		PORTB = led;
	}
}



