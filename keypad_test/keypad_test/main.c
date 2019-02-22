/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 11 Exercise # 1
 
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "bit.h"
#include "timer.h"
#include <stdio.h>


unsigned char GetKeypadKey() {
	/* // Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
	// If multiple keys pressed, returns leftmost-topmost one
	// Keypad must be connected to port A
	  Keypad arrangement
			PC4 PC5 PC6 PC7
	   col  1   2   3   4
	row
	PC0 1   1 | 2 | 3 | A
	PC1 2   4 | 5 | 6 | B
	PC2 3   7 | 8 | 9 | C
	PC3 4   * | 0 | # | D
	*/
	
	PORTA = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	
	if (GetBit(PINA,0)==0) { return('1'); }
	if (GetBit(PINA,1)==0) { return('4'); }
	if (GetBit(PINA,2)==0) { return('7'); }
	if (GetBit(PINA,3)==0) { return('*'); }

	// Check keys in col 2
	PORTA = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINA,0)==0) { return('2'); }
	if (GetBit(PINA,1)==0) { return('5'); }
	if (GetBit(PINA,2)==0) { return('8'); }
	if (GetBit(PINA,3)==0) { return('0'); }

	// Check keys in col 3
	PORTA = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINA,0)==0) { return('3'); }
	if (GetBit(PINA,1)==0) { return('6'); }
	if (GetBit(PINA,2)==0) { return('9'); }
	if (GetBit(PINA,3)==0) { return('#'); }

	// Check keys in col 4	
	PORTA = 0x7F; // Enable col 7 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINA,0)==0) { return('A'); }
	if (GetBit(PINA,1)==0) { return('B'); }
	if (GetBit(PINA,2)==0) { return('C'); }
	if (GetBit(PINA,3)==0) { return('D'); }

	return('\0'); // default value
	

}
unsigned long int findGCD(unsigned long int a, unsigned long int b) { 
	//--------Find GCD function --------------------------------------------------
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
	//--------End find GCD function ----------------------------------------------
	
}

typedef struct _task {
	//--------Task scheduler data structure---------------------------------------
	// Struct for Tasks represent a running process in our simple real-time operating system.
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
	//--------End Task scheduler data structure-----------------------------------
} task;

unsigned char SM1_out;
unsigned short numGet;

//Enumeration of states.
enum SM1_States { SM1_Start, SM1_get};
enum SM2_States { SM2_Start, SM2_Display };


int SMTick1(int state) {
	switch (state) {
		case SM1_Start: state = SM1_get; break;
		case SM1_get: break;
		default: state = SM1_Start; break;
	}
	switch (state) {
		case SM1_Start: break;
		case SM1_get:
			numGet = GetKeypadKey();
			switch (numGet) {
				case '\0': SM1_out = 0x1F; break; // All 5 LEDs on
				case '1': SM1_out = 0x01; break; // hex equivalent
				case '2': SM1_out = 0x02; break;
				case '3': SM1_out = 0x03; break;
				case '4': SM1_out = 0x04; break;
				case '5': SM1_out = 0x05; break;
				case '6': SM1_out = 0x06; break;
				case '7': SM1_out = 0x07; break;
				case '8': SM1_out = 0x08; break;
				case '9': SM1_out = 0x09; break;
				case 'A': SM1_out = 0x0A; break; //ASCII 65
				case 'B': SM1_out = 0x0B; break; //ASCII 66
				case 'C': SM1_out = 0x0C; break; //ASCII 67
				case 'D': SM1_out = 0x0D; break; //ASCII 68
				case '*': SM1_out = 0x0E; break; //ASCII 42
				case '0': SM1_out = 0x00; break;
				case '#': SM1_out = 0x0F; break; //ASCII 35
				default: SM1_out = 0x1B; break; // Should never occur. Middle LED off.
			}
			break;
	}
	return state;
}
int SMTick2(int state) {
	switch(state) {
		case SM2_Start:
			state = SM2_Display;
			break;
		case SM2_Display: break;
		default: state = SM2_Start; break;
	}
	switch (state) {
		case SM2_Start: break;
		case SM2_Display:
			PORTB = SM1_out;
			break;
		default: break;
	}
	return state;
}

/*
int main(void)
{
	unsigned char x;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	while(1) {
		x = GetKeypadKey();
		switch (x) {
			case '\0': PORTB = 0x1F; break; // All 5 LEDs on
			case '1': PORTB = 0x01; break; // hex equivalent
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case 'A': PORTB = 0x0A; break; //ASCII 65
			case 'B': PORTB = 0x0B; break; //ASCII 66
			case 'C': PORTB = 0x0C; break; //ASCII 67
			case 'D': PORTB = 0x0D; break; //ASCII 68
			case '*': PORTB = 0x0E; break; //ASCII 42
			case '0': PORTB = 0x00; break;
			case '#': PORTB = 0x0F; break; //ASCII 35
			default: PORTB = 0x1B; break; // Should never occur. Middle LED off.
		}
	}
}
*/


int main() {

	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRA = 0xF0; PORTA = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
	// . . . etc

	// Period for the tasks
	unsigned long int SMTick1_calc = 50;
	unsigned long int SMTick2_calc = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;


	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.

	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &SMTick2;//Function pointer for the tick

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		numGet = GetKeypadKey();
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;
}
