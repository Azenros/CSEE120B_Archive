/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 6 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

enum States{Start, one, two, four, press} state;
	
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned short i = 0;
unsigned char direction = 1;
unsigned char tmpA;

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
		case Start: state = one; break;
		case one: state = (tmpA == 0x01) ? press : two; i = 0; break;
		case two: state = (tmpA == 0x01) ? press : direction ? four : one; break;
		case four: state = (tmpA == 0x01) ? press : two; break;
		case press: 
			state = (tmpA == 0x01) ? one : press;
			break;
	}
	switch(state) {
		case Start: PORTB = 0x00; break;
		case one: PORTB = 0x01; direction = 1; break;
		case two: PORTB = 0x02; break;
		case four: PORTB = 0x04; direction = 0; break;
		case press: direction = 1; break;
			
	}
}

int main(void) {
	/* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
	while(1) {
		tmpA = ~PINA & 0x01;
		Tick();
		while (!TimerFlag) {};	// Wait 1 sec
		TimerFlag = 0;
		// Note: For the above a better style would use a synchSM with TickSM()
		// This example just illustrates the use of the ISR and flag
	}

	
}

