/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 3 Exercise # 5
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>


int main(void) {
    /* Replace with your application code */
	DDRD = 0xFF; PORTD = 0;
	DDRB = 0x01; PORTB = 0;
	unsigned char tmpD;
	unsigned short weight;
    while (1) {
		tmpD = PORTD;
		PORTB = PORTB & 0x01;
		weight = tmpD + (PORTB & 0x01);
		
		if (weight >= 70) {
			PORTB = PORTB | 0x02;
		}
		else if (weight >= 5) {
			PORTB = PORTB | 0x04;
		}
		
    }
}

