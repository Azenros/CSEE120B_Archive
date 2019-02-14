/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 10 Exercise # 5
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>	

enum Button_States{B_Start, B_Wait, B_Plus, B_Sub, B_HoldP, B_HoldS, Reset, Triggered} button_state;
enum Plus_Hold_States{P_Start, P_Wait, P_Hold} phold_state;
enum Sub_Hold_States{S_Start, S_Wait, S_Hold} shold_state;
	
unsigned char tmp_A = 0;
unsigned char ledNum = 5;
unsigned char P_trigger = 0;
unsigned char S_trigger = 0;
unsigned short i = 0;
unsigned char j = 0;
	
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}
void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
	
}
void TimerSet(unsigned long M) {
	// Set TimerISR() to tick every M ms
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
 
 void Button_Tick() {
	switch(button_state) {
		case B_Start: button_state = B_Wait; break;
		case B_Wait: 
			if (tmp_A == 0x01) { button_state = B_Sub; }
			else if (tmp_A == 0x02) { button_state = B_Plus; }
			else if (tmp_A == 0x03) { button_state = Reset; }
			break;
		case B_Sub:
			if (tmp_A == 0x01) { button_state = B_HoldS; }
			else if (tmp_A == 0x03) { button_state = Reset; }
			else { button_state = B_Wait; }
			break;
		case B_Plus:
			if (tmp_A == 0x02) { button_state = B_HoldP; }
			else if (tmp_A == 0x03) { button_state = Reset; }
			else { button_state = B_Wait; }
			break;
		case B_HoldS:
			if (i > 20) {
				j++;
				i = 0;
			}
			if (j >= 3) {
				button_state = Triggered;
				S_trigger = 1;
				i = 0; j = 0;
			}
			else if (tmp_A == 0x03) {
				button_state = Reset;
				i = 0;
			}
			else if (tmp_A == 0x00) {
				button_state = B_Wait;
				S_trigger = 0; i = 0; j = 0;
			}
			break;
		case B_HoldP:
			if (i > 20) {
				j++;
				i = 0;
			}
			else if (j >= 3) {
				button_state = Triggered;
				P_trigger = 1;
				i = 0; j = 0;
			}
			else if (tmp_A == 0x03) {
				button_state = Reset;
				i = 0;
			}
			else if (tmp_A == 0x00) {
				button_state = B_Wait;
				P_trigger = 0; i = 0; j = 0;
			}
			break;
		case Reset:
			button_state = (tmp_A == 0x00) ? B_Wait : Reset;
			break;
		case Triggered:
			button_state = (tmp_A == 0x00) ? B_Wait : Triggered;
			break;
		default: break;
	}
	
	switch (button_state) {
		case B_Start: break;
		case B_Wait: break;
		case B_Sub:
			if (ledNum > 0) { ledNum--; }
			break;
		case B_HoldS:
			if (ledNum > 0 && i == 20) { ledNum--; }
			i++;
			break;
		case B_Plus:
			if (ledNum < 9) { ledNum++; }
			break;
		case B_HoldP:
			if (ledNum < 9 && i == 20) { ledNum++; }
			i++;
			break;
		case Reset: 
			ledNum = 0;
			P_trigger = 0;
			S_trigger = 0; 
			break;
		case Triggered: break;
		default: break;
	}
}
 void Plus_Tick() {
	 switch (phold_state) {
		 case P_Start: phold_state = P_Wait; break;
		 case P_Wait:
			if (P_trigger) {
				phold_state = P_Hold;
			}
			else {
				phold_state = P_Wait;
			}
			break;
		 case P_Hold:
			if (tmp_A != 0x02) {
				phold_state = P_Wait;
				P_trigger = 0;
			}
			break;
	 }
	 switch (phold_state) {
		 case P_Start: break;
		 case P_Wait: break;
		 case P_Hold:
			if (ledNum < 9) {
				ledNum++;
			}
			break;
	 }
 }
 void Sub_Tick() {
	 switch (shold_state) {
		 case S_Start: shold_state = S_Wait; break;
		 case S_Wait:
		 if (S_trigger) {
			 shold_state = S_Hold;
		 }
		 break;
		 case S_Hold:
		 if (tmp_A != 0x01) {
			 shold_state = S_Wait;
			 S_trigger = 0;
		 }
		 break;
	 }
	 switch (shold_state) {
		 case S_Start: break;
		 case S_Wait: break;
		 case S_Hold:
		 if (ledNum > 0) {
			 ledNum--;
		 }
		 break;
	 }
 }
 
 
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long B_elapsedTime = 0;
	unsigned long P_elapsedTime = 0;
	unsigned long S_elapsedTime = 0;

	const unsigned long timerPeriod = 50;
	
	button_state = B_Start;
	phold_state = P_Start;
	shold_state = S_Start;

    TimerSet(timerPeriod);
	TimerOn();

    while (1) 
    {
		tmp_A = ~PINA & 0x07;
		
		if (B_elapsedTime >= 50) { //200 ms
			Button_Tick();
			B_elapsedTime = 0;
		}
		if (P_elapsedTime >= 400) {//400 ms
			Plus_Tick();
			P_elapsedTime = 0;
		}
		if (S_elapsedTime >= 400) {//400 ms
			Sub_Tick();
			S_elapsedTime = 0;
		}
		
		PORTB = ledNum;
		
		while (!TimerFlag) {}	// Wait 2 ms
		TimerFlag = 0;
		
		B_elapsedTime += timerPeriod;
		P_elapsedTime += timerPeriod;
		S_elapsedTime += timerPeriod;

    }
}

