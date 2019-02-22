/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 11 Exercise # 4
 
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include "bit.h"
#include "timer.h"

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
unsigned char trigger;
unsigned char getNum;
unsigned char numHit;

char str[] = {'C','o','n','g','r','a','t','u','l','a','t','i','o','n','s','!'};

//Enumeration of states.
enum SM1_States {SM1_Start, SM1_wait, SM1_press, SM1_hold};
enum SM2_States {SM2_Start, SM2_Display, SM2_Pause};
enum SM3_States {SM3_Start, SM3_Count};

int SMTick1(int state) {
	switch (state) {
		case SM1_Start: state = SM1_wait; break;
		case SM1_wait:
			if (getNum != '\0') {
				state = SM1_press;
			}
			break;
		case SM1_press:
			if (getNum != '\0') {
				state = SM1_hold;
			}
			else {
				state = SM1_wait;
			}
			break;
		case SM1_hold: 
			if (getNum == '\0') {
				state = SM1_wait;
				
			}
			break;
			
		default: state = SM1_Start; break;
	}
	switch (state) {
		case SM1_Start: break;
		case SM1_wait: break;
		case SM1_press: 
			trigger = 1;
			break;
		case SM1_hold: trigger = 0;break;
		default: break;
	}
	return state;
}
int SMTick2(int state) {
	switch(state) {
		case SM2_Start:
			state = SM2_Display;
			break;
		case SM2_Display: break;
		case SM2_Pause:
			
		default: state = SM2_Start; break;
	}
	switch (state) {
		case SM2_Start: break;
		case SM2_Display:
			if (trigger) {
				LCD_WriteData(getNum);
				numHit++;
			}
			trigger = 0;
			break;
		case SM2_Pause:
			break;
		default: break;
	}
	return state;
}

int SMTick3(int state) {
	switch (state) {
		case SM3_Start: state = SM3_Count; break;
		case SM3_Count:
			if (numHit > 16) {
				numHit = 1;
			}
		default: state = SM3_Start; break;
	}
	switch (state) {
		case SM3_Start: break;
		case SM3_Count: 
			LCD_Cursor(numHit);
			break;
	}
	return state;
}

int main() {
	DDRA = 0xF0; PORTA = 0x0F;//Keypad input
	DDRC = 0xFF; PORTC = 0x00;// LCD output
	DDRD = 0xFF, PORTD = 0x00; //LCD output (PD6-7)
	// . . . etc

	// Period for the tasks
	unsigned long int SMTick1_calc = 100;
	unsigned long int SMTick2_calc = 100;
	unsigned long int SMTick3_calc = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	tmpGCD = findGCD(tmpGCD, SMTick3_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;


	//Declare an array of tasks
	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2 , &task3};
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

	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period;//Task current elapsed time.
	task3.TickFct = &SMTick3;//Function pointer for the tick

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	LCD_ClearScreen();
	LCD_DisplayString(1, "Congratulations!");
	LCD_Cursor(1);
	numHit = 1;
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		getNum = GetKeypadKey();
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



