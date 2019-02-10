/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 9 Exercise # 3
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
enum States{Start, Idle, Play, Rest} state;
unsigned char tmpA = 0x00;
unsigned char note = 0x00;
unsigned short note_size;
unsigned short i = 0;

// double notes[] = {293.65, 293.65, 587.33, 440, 415.31, 392, 349.23, 293.65, 349.23, 392,
// 				  261.63, 261.63, 587.33, 440, 415.31, 392, 349.23, 293.65, 349.23, 392,
// 				  246.94, 246.94, 587.33, 440, 415.31, 392, 349.23, 293.65, 349.23, 392,
// 				  220, 220, 587.33, 440, 415.31, 392, 349.23, 293.65, 349.23, 392, 0};
				  
double notes[] = {587.33, 587.33, 1174.66, 880, 830.61, 784, 698.46, 587.33, 698.46, 784,
				  523.25, 523.25, 1174.66, 880, 830.61, 784, 698.46, 587.33, 698.46, 784,
				  493.88, 493.88, 1174.66, 880, 830.61, 784, 698.46, 587.33, 698.46, 784,
				  466.16, 466.16, 1174.66, 880, 830.61, 784, 698.46, 587.33, 698.46, 784, 0};
				  
int time_hold[] = {2000, 2000, 4000, 4000, 4000, 4000, 6000, 2000, 2000, 2000,
				   2000, 2000, 4000, 4000, 4000, 4000, 6000, 2000, 2000, 2000,
				   2000, 2000, 4000, 4000, 4000, 4000, 6000, 2000, 2000, 2000,
				   2000, 2000, 4000, 4000, 4000, 4000, 6000, 2000, 2000, 2000};
				   
int in_between[] = {2000, 2000, 3000, 9000, 3000, 3000, 1000, 2000, 1000, 1000,
				    2000, 2000, 3000, 9000, 3000, 3000, 1000, 2000, 1000, 1000,
					2000, 2000, 3000, 9000, 3000, 3000, 1000, 2000, 1000, 1000,
					2000, 2000, 3000, 9000, 3000, 3000, 1000, 2000, 1000, 1000};

 
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
		
		// prevents OCR0A from underflowing, using prescaler 64					
		// 31250 is largest frequency that will not result in underflow
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
		case Idle: state = (tmpA == 0x01) ? Play : Idle; break;
		case Play: 
			if (notes[note] == 0) {
				state = Idle;
				i = 0; note = 0;
			}
			else if (i >= time_hold[note]) {
				state = Rest;
				i = 0;
			}
			break;
		case Rest:
			if (i >= in_between[note]) {
				state = Play;
				i = 0;
				note++;
			}
			break;
		default: break;
	}
	switch (state) {
		case Start: break;
		case Idle: i = 0; note = 0; break;
		case Play:
			set_PWM(notes[note]);
			i++;
			break;
		case Rest:
			set_PWM(0);
			i++;
			break;	
		default: break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x08; PORTB = 0x00;
	PWM_on();
	while (notes[note] != 0) {
		note_size++;
		note++;
	}
	note = 0;
    while (1) {
		tmpA = ~PINA & 0x01;
		Tick();
		
    }
}

