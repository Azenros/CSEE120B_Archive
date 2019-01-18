/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 4 Exercise # 3
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, Wait, Push_1, R_1, Push_Y, Open} state;

void Tick() {
	switch(state) {
		case Start:
			state = Wait;
			break;
		case Wait:
			PORTB = 0;
			if (PORTA == 0x04) {state = Push_1;}
			else {state = Wait;}
			break;
		case Push_1:
			if (PORTA == 0x00) {state = R_1;}
			else {state = Push_1;}
			break;
		case R_1:
			if (PORTA == 0x02) {state = Push_Y;}
			else if (PORTA == 0x00) {state = R_1;}
			else {state = Wait;}
			break;
		case Push_Y:
			state = Wait;
			break;
		default:
			break;
		}
	
	switch (state) {			
		case Push_Y:
			PORTB = 0x01;
			break;
		default:
			break;
		
	}
}

int main(void) {
    /* Replace with your application code */
	DDRA = 0; PORTA = 0;
	DDRB = 0xFF; PORTB = 0x00;
	
	state = Start;
    while (1) {
		Tick();
    }
}

