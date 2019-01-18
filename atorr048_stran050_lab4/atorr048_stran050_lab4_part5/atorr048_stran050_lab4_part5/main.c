/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 4 Exercise # 5
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */


#include <avr/io.h>
enum States{Start, Init, Wait, Press, Release, Check, Open, Close} state;
int passcode[4] = {2, 0, 1, 0};
int input[4];
int input_loc;

void Tick() {
	switch(state) {
		case Start:
			state = Init;
			break;
		case Init:
			state = Wait;
		case Wait:
			if (input_loc == 4) {state = Check;}
			else if (PORTA != 0x00) {state = Press;}
			else {state = Wait;}
			break;
		case Press:
			state = Release;
			break;
		case Release:
			if (PORTA == 0x00) {state = Wait;}
			else {state = Release;}
			break;
		case Check:
			for (unsigned i = 0; i < 4; i++) {
				if (passcode[i] != input[i]) {
					state = Init;
					break;
				}
			}
			state = (PORTB == 0x01) ? Close : Open;
			break;
		case Open:
			state = Wait;
			break;
		case Close:
			state = Wait;
		default:
			break;
	}
	
	switch (state) {
		case Init:
			for (unsigned i = 0; i < 4; i++) {
				input[i] = 0;
			}
			input_loc = 0;
			break;
		case Press:
			if (PORTA == 0x04) {input[input_loc] = 2;}
			else if (PORTA == 0x02) {input[input_loc] = 1;}
			else if (PORTA == 0x01) {input[input_loc] = 0;}
			else {input_loc--;}
			input_loc++;
			break;
		case Open:
			PORTB = 1;
			break;
		case Close:
			PORTB = 0;
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


