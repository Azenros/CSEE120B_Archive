#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
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
int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x0F);/* set input channel to read */
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

enum SM1_States {SM1_Start, SM1_Read};  //reading values
enum SM2_States {SM2_Start, SM2_Display};  //displaying values
	
char buffer[40];
int ADC_Value; 
unsigned char change;

int SMTick1(int state) {
	switch (state) {
		case SM1_Start: state = SM1_Read; break;
		case SM1_Read: break;
		default: state = SM1_Start; break;
	}
	switch (state) {
		case SM1_Start: 
			
			break;
		case SM1_Read:
			if (change == 0x10) {
				ADC_Value = ADC_Read(0);
			}
			break;
		default: break;
	}
	return state;
}
int SMTick2(int state) {
	switch(state) {
		case SM2_Start: state = SM2_Display; break;
		case SM2_Display: break;
		default: state = SM2_Start; break;
	}
	switch (state) {
		case SM2_Start: break;
		case SM2_Display:
			if (change == 0x10) {
				sprintf(buffer, "%d", ADC_Value);
				LCD_DisplayString(7, buffer);
				//LCD_Cursor(7);
				//LCD_WriteData(ADC_Value);
				change = 0;
			}
		default: break;
	}
	return state;
}
	

int main(void) {
	DDRA = 0x00; PORTA = 0x10; // Analog input
	DDRC = 0xFF; PORTC = 0x00; // LCD output
	DDRD = 0xFF, PORTD = 0x00; //LCD output (PD6-7)
	// . . . etc

	// Period for the tasks
	unsigned long int SMTick1_calc = 1000;
	unsigned long int SMTick2_calc = 1000;
	/*
	unsigned long int SMTick3_calc = 300;
	unsigned long int SMTick4_calc = 100;
	unsigned long int SMTick5_calc = 60;
	*/

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	//tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	//tmpGCD = findGCD(tmpGCD, SMTick4_calc);
	//tmpGCD = findGCD(tmpGCD, SMTick5_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	//unsigned long int SMTick3_period = SMTick3_calc/GCD;
	//unsigned long int SMTick4_period = SMTick4_calc/GCD;
	//unsigned long int SMTick5_period = SMTick5_calc/GCD;


	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2};
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

/*

	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period;//Task current elapsed time.
	task3.TickFct = &SMTick3;//Function pointer for the tick
	
	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = SMTick4_period;//Task Period.
	task4.elapsedTime = SMTick4_period;//Task current elapsed time.
	task4.TickFct = &SMTick4;//Function pointer for the tick
	
	// Task 5
	task5.state = -1;//Task initial state.
	task5.period = SMTick5_period;//Task Period.
	task5.elapsedTime = SMTick5_period;//Task current elapsed time.
	task5.TickFct = &SMTick5;//Function pointer for the tick
	
*/

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	// Start or reset the LCD screen
	LCD_init();
	LCD_ClearScreen();
	
	ADC_init();
	LCD_DisplayString(1, "Temp: ?");
	
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		change = ~PINA & 0x10;
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