/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 11 Exercise # 2
 
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include "bit.h"
#include "timer.h"

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

char str[38] = {'C', 'S', '1', '2', '0', 'B', ' ', 'i', 's',' ' , 'l','e','g','e','n','d',' ','.','.','.',' ','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!'};
unsigned char tmpA;
unsigned short beginStr;
unsigned short endStr;
unsigned char trigger;

//Enumeration of states.
enum SM1_States {SM1_Start, SM1_wait, SM1_moveL, SM1_moveR};
enum SM2_States {SM2_Start, SM2_Display };


int SMTick1(int state) {
	switch (state) {
		case SM1_Start:
			if (tmpA == 0x01) {
				state = SM1_moveL;
			}
			else if (tmpA == 0x02) {
				state = SM1_moveR;
			}
			break;
		case SM1_moveL:
			state = (tmpA == 0x01) ? SM1_wait : SM1_Start;
			break;
		case SM1_moveR:
			state = (tmpA == 0x02) ? SM1_wait : SM1_Start; 
			break;
		case SM1_wait:
			state = (tmpA == 0x00) ? SM1_Start : SM1_wait;
			break;
		default: state = SM1_Start; break;
	}
	switch (state) {
		case SM1_Start:  break;
		case SM1_moveL: 
			if (endStr < 38) {
				beginStr++;
				endStr++;
				trigger = 1;
			}
			break;
		case SM1_moveR:
		if (beginStr > 0) {
			beginStr--;
			endStr--;
			trigger = 1;
		}
		break;
		case SM1_wait: break;
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
		default: state = SM2_Start; break;
	}
	switch (state) {
		case SM2_Start: break;
		case SM2_Display:
			if (trigger) {
				LCD_ClearScreen();
				for (int i = beginStr; i < endStr; i++) {
					LCD_WriteData(str[i]);
				}
				
			}
			trigger = 0;
			break;
		default: break;
	}
	return state;
}


int main() {
	DDRA = 0x00, PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF, PORTD = 0x00;
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
	
	LCD_ClearScreen();
	beginStr = 0; endStr = 16;
	for (int j = beginStr; j < endStr; j++) {
		LCD_WriteData(str[j]);
	}
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		tmpA = ~PINA & 0x03;
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



