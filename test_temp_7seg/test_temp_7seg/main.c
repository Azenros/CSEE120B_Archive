/*
 * test_temp_7seg.c
 *
 * Created: 3/17/2019 5:38:44 PM
 * Author : ruzst
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
#include <time.h>
#include "io.c"
#include "bit.h"
#include "timer.h"

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int ADC_Read(char channel) //reads the input from the selected channel
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
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

unsigned char gameStart;
unsigned char tmpA;
unsigned char tmpB;
unsigned char tmpD;
unsigned char charPos = 2;
unsigned char endGame = 0;
unsigned char numLives;
unsigned char buttonInput;
unsigned char resetButton;
unsigned char click;
unsigned char chosenColor;
unsigned char level;
unsigned char tempFlag;
unsigned char ledUp;

unsigned char e1;
unsigned char e2;

unsigned long timeLimit;
unsigned long currTime;
unsigned long currTime_ten;
unsigned long currTime_one;
unsigned long JSXcor;

char topHalf[7];
char botHalf[7];
char healthText[7];
char sortedWord[13];
char buffer[20];

int letterAt;
int ADC_Value1;

enum SM1_States {SM1_Start, SM1_Display};
enum SM10_States {SM10_Start, SM10_Output};
enum SM11_States {SM11_Start, SM11_Wait, SM11_Read}; //reader for temperature
enum SM12_States {SM12_Start, SM12_LEDset, SM12_Match, SM12_Reset, SM12_Hold}; //lights the 7seg LED and confirmation LED

int SMTick1(int state) {
	switch (state) {
		case SM1_Start: state = SM1_Display; break;
		case SM1_Display: break;
		default: state = SM12_Start; break;
	}
	switch (state) {
		case SM1_Start: break;
		case SM1_Display: 
			sprintf(buffer, "%d", ADC_Value1);
			LCD_DisplayString(1, buffer);
			break;
		default: break;
	}
	return state;	
}
int SMTick10(int state) {
	switch (state) {
		case SM10_Start: state = SM10_Output;
		case SM10_Output: break;
		default: state = SM10_Start;
	}
	switch (state) {
		case SM10_Start: break;
		case SM10_Output:
		PORTD = tmpD;
		PORTB = tmpB;
		break;
		default: break;
	}
	return state;
}
int SMTick11(int state) {
	switch (state) {
		case SM11_Start: state = SM11_Read; break;
		case SM11_Read: break;
		default: state = SM11_Start; break;
	}
	switch (state) {
		case SM11_Start: break;
		case SM11_Read:
		if (tempFlag) {
			ADC_Value1 = ADC_Read(0);
		}
		if (ADC_Value1 == 0) {
			ADC_Value1 = 1000;
		}
		break;
		
		default: break;
	}
	return state;
}
int SMTick12(int state) {
	switch (state) {
		case SM12_Start: state = SM12_LEDset; break;
		case SM12_LEDset:
		if (ADC_Value1 < 640) {
			state = SM12_Match;
		}
		else if (resetButton) {
			state = SM12_Reset;
		}
		else {
			state = SM12_LEDset;
		}
		break;
		case SM12_Match: state = SM12_LEDset;
		break;
		case SM12_Reset: state = SM12_Hold; break;
		case SM12_Hold: state = (resetButton) ? SM12_Hold : SM12_Start; break;
		default: state = SM12_Start; break;
	}
	switch (state) {
		case SM12_Start: break;
		case SM12_LEDset:
		if (!endGame) {
			if (ADC_Value1 > 700) { //0
				tmpB = 0x00;
			}
			else if (ADC_Value1 > 690) {//1
				tmpB = 0x71;
			}
			else if (ADC_Value1 > 680) {//2
				tmpB = 0x64;
			}
			else if (ADC_Value1 > 670) {//3
				tmpB = 0x70;
			}
			else if (ADC_Value1 > 660) {//4
				tmpB = 0x69;
			}
			else if (ADC_Value1 > 650) {//5
				tmpB = 0x72;
			}
			else if (ADC_Value1 > 640) {//6
				tmpB = 0x62;
			}
			else if (ADC_Value1 > 630) {//7
				tmpB = 0x78;
			}
			else if (ADC_Value1 > 620) {//8
				tmpB = 0x61;
			}
			else if (ADC_Value1 > 610) {//9
				tmpB = 0x70;
			}
			else if (ADC_Value1 > 500) { //A
				tmpB = 0x6A;
			}
			else {
				tmpB = 0x00;
			}
			
		}
		else {
			tmpB = 0xE5;
		}
		
		if (ledUp) {
			tmpB = tmpB | 0x80;
		}
		break;
		case SM12_Match: ledUp = 1; break;
		case SM12_Reset: ledUp = 0; break;
		case SM12_Hold: break;
		default: break;
	}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xF8; // Temperature, Joystick, and Multi-LED Input
	DDRB = 0xFF; PORTB = 0x00; // 7-segment LED
	DDRC = 0xFF; PORTC = 0x00;// LCD Screen
	DDRD = 0xFE, PORTD = 0x01; //LCD Screen, RGB LEDs, Temperature LED, and Reset button
	// . . . etc

	// Period for the tasks
	unsigned long int SMTick1_calc = 100;
	unsigned long int SMTick10_calc = 10;
	unsigned long int SMTick11_calc = 100;
	unsigned long int SMTick12_calc = 100;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick10_calc);
	tmpGCD = findGCD(tmpGCD, SMTick11_calc);
	tmpGCD = findGCD(tmpGCD, SMTick12_calc);
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick10_period = SMTick10_calc/GCD;
	unsigned long int SMTick11_period = SMTick11_calc/GCD;
	unsigned long int SMTick12_period = SMTick12_calc/GCD;

	//Declare an array of tasks
	static task task1, task10, task11, task12;
	task *tasks[] = { &task1, &task11, &task10, &task12};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.

	// Task 10
	task10.state = -1;//Task initial state.
	task10.period = SMTick10_period;//Task Period.
	task10.elapsedTime = SMTick10_period;//Task current elapsed time.
	task10.TickFct = &SMTick10;//Function pointer for the tick

	// Task 11
	task11.state = -1;//Task initial state.
	task11.period = SMTick11_period;//Task Period.
	task11.elapsedTime = SMTick11_period;//Task current elapsed time.
	task11.TickFct = &SMTick11;//Function pointer for the tick
	
	// Task 12
	task12.state = -1;//Task initial state.
	task12.period = SMTick12_period;//Task Period.
	task12.elapsedTime = SMTick12_period;//Task current elapsed time.
	task12.TickFct = &SMTick12;//Function pointer for the tick

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	// Start or reset the LCD screen
	LCD_init();
	LCD_ClearScreen();
	
	ADC_init();

	LCD_Cursor(2);
	timeLimit = 60;
	numLives = 6;
	endGame = 0;

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		//updates every ms
		click = ~PINA & 0x08;
		buttonInput = ~PINA & 0xE0;
		tempFlag = ~PINA & 0x10;
		resetButton = PIND & 0x01;
		
		if ((tmpD & 0x22) == 0x22 && (tmpB & 0x80) == 0x80) {
			endGame = 1;
		}
		
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
