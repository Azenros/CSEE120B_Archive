/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 5 Exercise # 3
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>

enum States{Start, Run}state;
unsigned char loc = 0;
unsigned char dir = 1;
unsigned char button = 0x00;
unsigned char light = 0x01;

void Tick() {
	switch (state) {
		case Start: state = Run; break; 
		case Run: break;
	}
	switch (state) {
		case Start: break;
		case Run:
			light = ~PINA & 0x01;
			if (light) {
				if (loc == 6) {
					dir = 2;
				}
				else if (loc == 0) {
					dir = 1;
				}
				
				if (dir == 0) {
					break;
				}
				else if (dir == 1) {
					light = light << 1;
					loc++;
				}
				else if (dir == 2) {
					light = light >> 1;
					loc--;
				}
				PORTC = light;
			}
			else {
				PORTC = light;
			}
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure PORTA as input, initialize to 1s
	DDRC = 0xFF; PORTC = 0x00; // Configure PORTC as outputs, initialize to 0s
	light = 0x01;
	while(1)
	{
		Tick();
	}
}


