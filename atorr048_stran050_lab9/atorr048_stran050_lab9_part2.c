/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 9 Exercise # 2
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, Idle, Run, Wait}state;
unsigned char tmpA = 0x00;
unsigned char toggle = 0x00;
double frequencies[] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
unsigned char freq_at = 0;


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

void Tick() {
	switch (state) {
		case Start: state = Idle; break;
		case Idle: state = (toggle) ? Run : Idle; break;
		case Run:
			if (tmpA == 0x04) {
				toggle = 0;
				state = Wait;
			}
		default: break;
	}
	switch (state) {
		case Start: break;
		case Idle:
			if (tmpA == 0x04) {
				toggle = 1;
				state = Wait;
			}
			set_PWM(0);
			break;
		case Run:
			if (tmpA == 0x04) {
				toggle = 0;
				state = Wait;
			}
			else if (tmpA == 0x02 && freq_at < 7) { // going up
				freq_at++;
				state = Wait;
			}
			else if (tmpA == 0x01 && freq_at > 0) { // going down
				freq_at--;
				state = Wait;
			}
			set_PWM(frequencies[freq_at]);
			break;
		case Wait:
			if (tmpA == 0x00) {
				if (toggle) {
					state = Run;
				}
				else {
					state = Idle;
				}
			}
			break;
		default: break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x08; PORTB = 0x00;
	PWM_on();

    while (1) {
		tmpA = ~PINA & 0x07;
		Tick();
		
		
    }
}

