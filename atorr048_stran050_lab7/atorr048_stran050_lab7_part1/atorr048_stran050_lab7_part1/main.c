/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 7 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>	
enum States{Start, one, two, four} state;
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char direction = 1;

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
		case one: state = two; direction = 1; break;
		case two: state = direction ? four : one; break;
		case four: state = two; direction = 0; break;
	}
	switch(state) {
		case Start: PORTB = 0x00; break;
		case one: PORTB = 0x01; break;
		case two: PORTB = 0x02; break;
		case four: PORTB = 0x04; break;
	}
}

int main(void) {
    /* Replace with your application code */
 
	 DDRB = 0xFF; PORTB = 0x00;
	 TimerSet(1000);
	 TimerOn();
	 while(1) {
		 Tick();
		 while (!TimerFlag) {};	// Wait 1 sec
		 TimerFlag = 0;
		 // Note: For the above a better style would use a synchSM with TickSM()
		 // This example just illustrates the use of the ISR and flag
	 }

    
}

