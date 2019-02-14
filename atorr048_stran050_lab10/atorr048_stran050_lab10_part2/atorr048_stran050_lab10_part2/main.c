/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 10 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>	

enum Three_States{T_Start, B0, B1, B2} three_state;
enum Blink_States{B_Start, off, B3} blink_state;
enum Output{Start, write} out_state;
	
unsigned char tmp_three = 0x00;
unsigned char tmp_blink = 0x00;
	
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
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
 
void threeLED_Tick() {
	switch (three_state) {
		case T_Start: three_state = B0; break;
		case B0: three_state = B1; break;
		case B1: three_state = B2; break;
		case B2: three_state = B0; break;
		default: break;
	}
	switch (three_state) {
		case T_Start: break;
		case B0: tmp_three = 0x01; break;
		case B1: tmp_three = 0x02; break;
		case B2: tmp_three = 0x04; break;
		default: break;
	}
}
void blinkLED_Tick() {
	switch (blink_state) {
		case B_Start: blink_state = off; break;
		case off: blink_state = B3; break;
		case B3: blink_state = off; break;
		default: break;
	}
	switch (blink_state) {
		case B_Start: break;
		case off: tmp_blink = 0x00; break;
		case B3: tmp_blink = 0x08; break;
		default: break;
	}
}
void output_Tick() {
	switch (out_state) {
		case Start: out_state = write; break;
		case write: break;
		default: break;
	}
	switch (out_state) {
		case Start: break;
		case write: 
			PORTB = tmp_blink | tmp_three; 
			break;
		default: break;
	}
}
int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long TL_elapsedTime = 0;
	unsigned long BL_elapsedTime = 0;
	const unsigned long timerPeriod = 100;
    TimerSet(100);
	TimerOn();
	three_state = Start;
	blink_state = B_Start;
    while (1) 
    {
		if (TL_elapsedTime >= 300) {
			threeLED_Tick();
			TL_elapsedTime = 0;
		}
		if (BL_elapsedTime >= 1000) {
			blinkLED_Tick();
			BL_elapsedTime = 0;
		}
		output_Tick();
		
		while (!TimerFlag) {}	// Wait 1 sec
		TimerFlag = 0;
		
		BL_elapsedTime += timerPeriod;
		TL_elapsedTime += timerPeriod;
    }
}

