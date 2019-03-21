/*
 * test_joystick.c
 *
 * Created: 3/7/2019 12:11:13 PM
 * Author : ruzst
 */ 

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

unsigned char tmpA;
unsigned char SM1_out;
unsigned char obstacle;
unsigned char ob1Loc, ob2Loc, ob3Loc, ob4Loc;
unsigned char pause; //shared
unsigned char charPos = 2;
unsigned char endGame;

unsigned long JSXcor;
unsigned short xPos;
unsigned short yPos;
unsigned char click;

//Enumeration of states.
enum SM1_States {SM1_Start, SM1_Wait, SM1_Press, SM1_Hold};  //pause or start the game
enum SM2_States {SM2_Start, SM2_Wait, SM2_Up, SM2_Down, SM2_Left, SM2_Right, SM2_Hold};  //move the player around
enum SM3_States {SM3_Start, SM3_Move};  // if not paused, move the obstacles to the left
enum SM4_States {SM4_Start, SM4_Check, SM4_Display, SM4_Wait4Reset}; // checks for collision; if collision, pause and display message
enum SM5_States {SM5_Start, SM5_readX, SM5_readY}; //joystick reader, constantly checks positions

int SMTick1(int state) {
	switch (state) {
		case SM1_Start: state = SM1_Wait; break;
		case SM1_Wait: state = (click) ? SM1_Press : SM1_Wait; break;
		case SM1_Press: state = (click) ? SM1_Hold : SM1_Wait; break;
		case SM1_Hold: state = (!click) ? SM1_Wait : SM1_Hold; break;
		default: state = SM1_Start; break;
	}
	switch (state) {
		case SM1_Start: break;
		case SM1_Wait: break;
		case SM1_Press: 
			pause = (pause) ? 0 : 1; 
			break;
		case SM1_Hold: break;
		default: break;
	}
	return state;
}
int SMTick2(int state) {
	switch(state) {
		case SM2_Start: state = SM2_Wait; break;
		case SM2_Wait:
			if (!endGame && !pause) {
				if (tmpA == 0x01) {
					state = SM2_Up;
				}
				else if (tmpA == 0x02) {
					state = SM2_Down;
				}
				else if (tmpA == 0x03) {
					state = SM2_Left;
				}
				else if (tmpA == 0x04) {
					state = SM2_Right;
				}
				else {
					state = SM2_Wait;
				}
			}
			break;
		case SM2_Up:
			state = (tmpA == 0x01) ? SM2_Hold : SM2_Wait;
			break;
		case SM2_Down:
			state = (tmpA == 0x02) ? SM2_Hold : SM2_Wait;
			break;
		case SM2_Left:
			state = (tmpA == 0x03) ? SM2_Hold : SM2_Wait;
			break;
		case SM2_Right:
			state = (tmpA == 0x04) ? SM2_Hold : SM2_Wait;
			break;
		case SM2_Hold:
			state = (tmpA == 0x00) ? SM2_Wait : SM2_Hold;
			break;
		default: state = SM2_Start; break;
	}
	switch (state) {
		case SM2_Start: break;
		case SM2_Wait: break;
		case SM2_Up: 
			if (charPos > 16) {
				charPos -= 16; 
			}
			break;
		case SM2_Down: 
			if (charPos <= 16) {
				charPos += 17;
			}
		case SM2_Left:
			if (charPos != 17 && charPos != 1) {
				charPos--;
			}
			break;
		case SM2_Right:
			if (charPos != 16 && charPos != 32) {
				charPos++;
			}
			break;
		default: break;
	}
	return state;
}
int SMTick3(int state) {
	switch (state) {
		case SM3_Start: state = SM3_Move; break;
		case SM3_Move: break;
		default: state = SM3_Start; break;
	}
	switch (state) {
		case SM3_Start: break;
		case SM3_Move:
			if (!pause) {
				if (ob1Loc == 1) {
					ob1Loc = 16;
				}
				else {
					ob1Loc--;
				}
				if (ob2Loc == 17) {
					ob2Loc = 32;
				}
				else {
					ob2Loc--;
				}
				if (ob3Loc == 1) {
					ob3Loc = 16;
				}
				else {
					ob3Loc--;
				}
				if (ob4Loc == 17) {
					ob4Loc = 32;
				}
				else {
					ob4Loc--;
				}
				
				
				LCD_ClearScreen();
				
				LCD_Cursor(ob1Loc);
				LCD_WriteData(obstacle);
				
				
				LCD_Cursor(ob2Loc);
				LCD_WriteData(obstacle);
				
				LCD_Cursor(ob3Loc);
				LCD_WriteData(obstacle);
				
				LCD_Cursor(ob4Loc);
				LCD_WriteData(obstacle);
				
				LCD_Cursor(charPos);
				
			}
			
			break;
		default: break;
	}
	return state;
}
int SMTick4(int state) {
	switch (state) {
		case SM4_Start: state = SM4_Check;
		case SM4_Check:
			if ((charPos == ob1Loc) || (charPos == ob2Loc) || (charPos == ob3Loc) || (charPos == ob4Loc)
			) {
				state = SM4_Display;
			}
			break;
		case SM4_Display: state = SM4_Wait4Reset; break;
		case SM4_Wait4Reset:
			if (click) {
				state = SM4_Check;
				endGame = 0;
			}
			break;
		default: state = SM4_Start; break;
	}
	switch (state) {
		case SM4_Start: break;
		case SM4_Check: break;
		case SM4_Display: 
			pause = 1; endGame = 1;
			LCD_ClearScreen();
			LCD_DisplayString(2, "TRY AGAIN?      > YES   NO");
			LCD_Cursor(18);
			break;
		case SM4_Wait4Reset: 
			ob1Loc = 15;
			ob2Loc = 27;
			ob3Loc = 7;
			ob4Loc = 19;
			charPos = 2;
			break;
		default: break;
	}
	return state;
}
int SMTick5(int state) {
	switch (state) {
		case SM5_Start:
			state = SM5_readX;
			break;
		case SM5_readX:
			//ADMUX = 0x00;
			state = SM5_readY;
			break;
		case SM5_readY:
			//ADMUX = 0x01;
			state = SM5_readX;
			break;
		default: state = SM5_Start; break;
	}
	switch (state) {
		case SM5_Start: break;
		case SM5_readX:
			xPos = ADC_Read(0);
			break;
		case SM5_readY:
			yPos = ADC_Read(1);
			
			if (xPos > 768) {
				tmpA = 0x04;
			}
			else if (yPos > 890) {
				tmpA = 0x01;
			}
			else if (yPos < 359) {
				tmpA = 0x02;
			}
			else if (xPos < 200) {
				tmpA = 0x03;
			}
			else {
				tmpA = 0x00;
			}
			break;
			
		default: break;
	}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0x04;// Joystick input
	DDRC = 0xFF; PORTC = 0x00;// LCD output
	DDRD = 0xFF, PORTD = 0x00; //LCD output (PD6-7)
	// . . . etc

	// Period for the tasks
	unsigned long int SMTick1_calc = 100;
	unsigned long int SMTick2_calc = 100;
	unsigned long int SMTick3_calc = 300;
	unsigned long int SMTick4_calc = 100;
	unsigned long int SMTick5_calc = 60;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	tmpGCD = findGCD(tmpGCD, SMTick4_calc);
	tmpGCD = findGCD(tmpGCD, SMTick5_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;


	//Declare an array of tasks
	static task task1, task2, task3, task4, task5;
	task *tasks[] = { &task1, &task2 , &task3, &task4, &task5};
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

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	// Start or reset the LCD screen
	LCD_init();
	LCD_ClearScreen();
	
	ADC_init();
	
	obstacle = '#';
	pause = 0;
	ob1Loc = 15;
	ob2Loc = 27;
	ob3Loc = 7;
	ob4Loc = 19;
	
	LCD_Cursor(ob1Loc); LCD_WriteData(obstacle);
	LCD_Cursor(ob2Loc); LCD_WriteData(obstacle);
	LCD_Cursor(ob3Loc); LCD_WriteData(obstacle);
	LCD_Cursor(ob4Loc); LCD_WriteData(obstacle);
	LCD_Cursor(2);
	

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		click = ~PINA & 0x04;
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
