/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 10 Exercise # 4
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>	

enum Three_States{T_Start, B0, B1, B2} three_state;
enum Blink_States{B_Start, off, B3} blink_state;
enum Speaker_States{S_Start, Quiet, Noise}speak_state;
enum FChange_States{F_Start, Wait, Up, Down, Rest}change_state;
enum Output{Start, write} out_state;
	
double frequencies[] = {246.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88};
unsigned char freq_at = 2;
	
unsigned char tmp_three = 0x00;
unsigned char tmp_blink = 0x00;
unsigned char tmp_A = 0x00;
	
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

void set_PWM(double frequency) {
	// 0.954 hz is lowest frequency possible with this function,
	// based on settings in PWM_on()
	// Passing in 0 as the frequency will stop the speaker from generating sound
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}
void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}
void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
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
		case B_Start: blink_state = B3; break;
		case off: blink_state = B3; break;
		case B3: blink_state = off; break;
		default: break;
	}
	switch (blink_state) {
		case B_Start: break;
		case off: tmp_blink = 0x00; break;
		case B3: tmp_blink = 0x10; break;
		default: break;
	}
}
void speaker_Tick() {
	switch (speak_state) {
		case Start: speak_state = Quiet; break;
		case Quiet: speak_state = (tmp_A == 0x04) ? Noise : Quiet; break;
		case Noise: speak_state = (tmp_A == 0x04) ? Noise : Quiet; break;
		default: break;
	}
	switch (speak_state) {
		case Start: break;
		case Quiet: set_PWM(0); break;
		case Noise: set_PWM(frequencies[freq_at]); break;
		default: break;
	}
	
}
void fChange_Tick() {
	switch (change_state) {
		case F_Start: change_state = Wait; break;
		case Wait: 
			if (tmp_A == 0x01) {
				change_state = Down;
			}
			else if (tmp_A == 0x02) {
				change_state = Up;
			}
			break;
		case Down: change_state = Rest; break;
		case Up: change_state = Rest; break;
		case Rest:
			if (tmp_A == 0x00) {
				change_state = Wait;
			}
			break;
		default: break;
	}
	switch (change_state) {
		case F_Start: break;
		case Wait: break;
		case Up:
			if (freq_at < 7) {
				freq_at++;
			}
			break;
		case Down:
			if (freq_at > 0) {
				freq_at--;
			}
			break;
		case Rest: break;
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
			PORTB = tmp_blink | tmp_three | 0x08; 
			break;
		default: break;
	}
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x08;
	unsigned long TL_elapsedTime = 0;
	unsigned long BL_elapsedTime = 0;
	unsigned long S_elapsedTime = 0;
	unsigned long F_elapsedTime = 0;
	const unsigned long timerPeriod = 2;
	PWM_on();
    TimerSet(timerPeriod);
	TimerOn();
	three_state = T_Start;
	blink_state = B_Start;
	speak_state = S_Start;
	change_state = F_Start;
    while (1) 
    {
		tmp_A = ~PINA & 0x07;
		if (TL_elapsedTime >= 300) { //300 ms
			threeLED_Tick();
			TL_elapsedTime = 0;
		}
		if (BL_elapsedTime >= 1000) {//1000 ms
			blinkLED_Tick();
			BL_elapsedTime = 0;
		}
		if (F_elapsedTime >= 2) {
			fChange_Tick();
			F_elapsedTime = 0;
		}
		if (S_elapsedTime >= 2) {//2 ms
			speaker_Tick();
			S_elapsedTime = 0;
		}
		output_Tick();
		
		while (!TimerFlag) {}	// Wait 2 ms
		TimerFlag = 0;
		
		BL_elapsedTime += timerPeriod;
		TL_elapsedTime += timerPeriod;
		S_elapsedTime += timerPeriod;
		F_elapsedTime += timerPeriod;
    }
}

