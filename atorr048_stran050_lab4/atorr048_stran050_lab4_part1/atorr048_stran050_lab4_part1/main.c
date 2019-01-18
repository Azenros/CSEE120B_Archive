/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 4 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, B0_release, B0_push, B1_release, B1_push} state;

void Tick() {
	switch(state) {
		case B0_release:
			state = (PORTA == 0x01) ? B1_push : B0_release;
			break;
		case B1_push:
			state = (PORTA == 0x00) ? B1_release : B1_push;
			break;
		case B1_release:
			state = (PORTA == 0x01) ? B0_push : B1_release;
			break;
		case B0_push:
			state = (PORTA == 0x00) ? B0_release : B0_push;
			break;
		case Start:
			state = B0_release;
			break;
		default:
			break;  
	}
	switch (state) {
		case B0_release:
			break;
		case B1_push:
			PORTB = 0x02;
			break;
		case B1_release:
			break;
		case B0_push:
			PORTB = 0x01;
			break;
		default:
			break;
	}
}

int main(void) {
    /* Replace with your application code */
	DDRA = 0; PORTA = 0;
	DDRB = 0xFF; PORTB = 0;
	
	PORTB = 0x01;
	state = Start;
    while (1) {
		Tick();
    }
}

