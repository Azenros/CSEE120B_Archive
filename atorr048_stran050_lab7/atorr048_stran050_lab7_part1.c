/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 7 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include "io.c"


enum States{Start, Wait, Plus, Plus_hold, Minus, Minus_hold, Reset} state;

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char curr_num = 0;
unsigned char cur_inA = 0;
unsigned short count;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	//Set TimerISR() to tick every M ms
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick() {
	switch(state) {
		case Start:
			state = Wait;
			LCD_WriteData('0');
			break;
		case Wait:
			if (cur_inA == 0x03) {state = Reset;}
			else if (cur_inA == 0x02) {state = Minus;}
			else if (cur_inA == 0x01) {state = Plus;}
			else {state = Wait;}
			break;
		case Minus:
			if (cur_inA == 0x03) {state = Reset;}
			else if (cur_inA == 0x02) {state = Minus_hold;}
			else {state = Wait;}
			break;
		case Plus:
			if (cur_inA == 0x03) {state = Reset;}
			else if (cur_inA == 0x01) {state = Plus_hold;}
			else {state = Wait;}
			break;
		case Minus_hold:
			if (cur_inA == 0x03) {state = Reset;}
			else if (cur_inA == 0x00) {state = Wait;}
			else if (cur_inA == 0x02) {
				count++;
			}
			break;
		case Plus_hold:
			if (cur_inA == 0x03) {state = Reset;}
			else if (cur_inA == 0x00) {state = Wait;}
			else if (cur_inA == 0x01) {
				count++;
			}
			break;
		case Reset:
			if (cur_inA == 0x03) {state = Reset;}
			else {state = Wait;}
			break;
		default:
			break;
		}
	
	switch (state) {	
		case Wait:
			count = 0;
			break;
		case Minus:
			if (curr_num > 0) { 
				curr_num--;
				LCD_ClearScreen();
				LCD_WriteData(curr_num + '0');
			}
			break;
		case Plus:
			if (curr_num < 9) { 
				curr_num++;
				LCD_ClearScreen();
				LCD_WriteData(curr_num + '0');
			}
			break;
		case Minus_hold:
			if (count >= 10) {
				if (curr_num > 0) {
					curr_num--;
					LCD_ClearScreen();
					LCD_WriteData(curr_num + '0');
					count = 0;
				}
			}
			break;
		case Plus_hold:
		if (count >= 10) {
			if (curr_num < 9) {
				curr_num++;
				LCD_ClearScreen();
				LCD_WriteData(curr_num + '0');
				count = 0;
			}
		}
		break;
		case Reset:
			curr_num = 0;
			LCD_ClearScreen();		
			LCD_WriteData(curr_num + '0');
			break;
		default:
			//LCD_DisplayString(1, "ERROR");
			break;
		
	}
}

int main(void) {
	DDRA = 0; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	state = Start;
	curr_num = 0;
	LCD_init();
	
	TimerSet(100);
	TimerOn();
	
    while (1) {
		cur_inA = ~PINA & 0x03;
		Tick();
		while (!TimerFlag) {};	// Wait 1 sec
		TimerFlag = 0;
    }
}

	
	
