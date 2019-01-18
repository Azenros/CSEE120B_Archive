/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 4 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, Wait, Plus_on, Plus_off, Minus_on, Minus_off, Reset} state;

void Tick() {
	switch(state) {
		case Start:
			state = Wait;
			break;
		case Wait:
			if (PORTA == 0x03) {state = Reset;}
			else if (PORTA == 0x02) {state = Minus_on;}
			else if (PORTA == 0x01) {state = Plus_on;}
			else {state = Wait;}
			break;
		case Minus_off:
			if (PORTA == 0x00) {state = Wait;}
			else if (PORTA == 0x03) {state = Reset;}
			else {state = Minus_off;}
			break;
		case Minus_on:
			if (PORTA == 0x03) {state = Reset;}
			else {state = Minus_off;}
			break;
		case Plus_off:
			if (PORTA == 0x00) {state = Wait;}
			else if (PORTA == 0x03) {state = Reset;}
			else {state = Plus_off;}
			break;
		case Plus_on:
			if (PORTA == 0x03) {state = Reset;}
			else {state = Plus_off;}
			break;
			
		case Reset:
			if (PORTA == 0x03) {state = Reset;}
			else {state = Wait;}
			break;
		default:
			break;
		}
	
	switch (state) {			
		case Wait:
			break;
		case Minus_on:
			if (PORTC > 0) { PORTC--;}
			break;
		case Minus_off:
			break;
		case Plus_on:
			if (PORTC < 9) { PORTC++;}
			break;
		case Plus_off:
			break;
		case Reset:
			PORTC = 0;
			break;
		default:
			break;
		
	}
}

int main(void) {
    /* Replace with your application code */
	DDRA = 0; PORTA = 0;
	DDRC = 0xFF; PORTC = 0x07;
	
	state = Start;
    while (1) {
		Tick();
    }
}

