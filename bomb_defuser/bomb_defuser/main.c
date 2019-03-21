/*	Author: Steven Tran (stran050@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Final Project - Bomb Defusing Game: main.c
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#define F_CPU 8000000UL
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

char topHalf[7];
char botHalf[7];
char healthText[7];
char sortedWord[13];

int letterAt;
int ADC_Value1;
int ADC_Value2;

unsigned short xPos;
unsigned short yPos;

unsigned long timeLimit;
unsigned long currTime;
unsigned long currTime_ten;
unsigned long currTime_one;
unsigned long JSXcor;

//Enumeration of states.

//Joystick enumerations
enum SM1_States {SM1_Start, SM1_Wait, SM1_Press, SM1_Hold};  //clicking the joystick, and chooses the current position of the cursor
enum SM2_States {SM2_Start, SM2_Wait, SM2_Up, SM2_Down, SM2_Left, SM2_Right, SM2_Hold};  //move the player around
enum SM3_States {SM3_Start, SM3_readX, SM3_readY}; //Reader, constantly checks position of joystick
	
//LCD enumerations
enum SM4_States {SM4_Start, SM4_Reset, SM4_Countdown, SM4_Lose, SM4_Win, SM4_Wait}; //Timer and endgame text
enum SM5_States {SM5_Start, SM5_Write, SM5_Wait}; //place an anagram on the LCD
enum SM6_States {SM6_Start, SM6_Write, SM6_Wait}; //write Health Bar

//Multi-color LED enumerations
enum SM7_States {SM7_Start, SM7_Reset, SM7_Wait, SM7_R, SM7_G, SM7_B, SM7_Hold}; //changes color based on button presses
enum SM8_States {SM8_Start, SM8_Choose, SM8_Match, SM8_Wait, SM8_Reset, SM8_Hold}; //lights LED on color match
	
//Output enumerations
enum SM9_States {SM9_Start, SM9_Level, SM9_Setup, SM9_Wait, SM9_Brisk, SM9_Reset}; // unused now, will use when more levels are made
enum SM10_States {SM10_Start, SM10_Output};
	
//Temp sensor and 7seg LCD enumerations
enum SM11_States {SM11_Start, SM11_Wait, SM11_Read}; //reader for temperature
enum SM12_States {SM12_Start, SM12_LEDset, SM12_Match, SM12_Reset, SM12_Hold}; //lights the 7seg LED and confirmation LED 


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
			if (!endGame) {
				if (letterAt == 0) {
					if (charPos == 4 || charPos == 20) {
						if (charPos == 4) {
							e1 = 1;
						}
						else if (charPos == 20) {
							e2 = 1;
						}
						LCD_WriteData(286);
						letterAt++;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
					
					}
					LCD_Cursor(charPos);
				}
				else if (letterAt == 1) {
					if (charPos == 21) {
						LCD_WriteData(286);
						letterAt++;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
						
					}
					LCD_Cursor(charPos);
				}
				else if (letterAt == 2) {
					if (charPos == 3) {
						LCD_WriteData(286);
						letterAt++;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
						
					}
					LCD_Cursor(charPos);
				}
				else if (letterAt == 3) {
					if (charPos == 5) {
						LCD_WriteData(286);
						letterAt++;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
						
					}
					LCD_Cursor(charPos);
				}
				else if (letterAt == 4) {
					if (charPos == 19) {
						LCD_WriteData(286);
						letterAt++;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
						
					}
					LCD_Cursor(charPos);
				}
				else if (letterAt == 5) {
					if ( (charPos == 4 && e2) || (charPos == 20 && e1)) {
						LCD_WriteData(286);
						letterAt++;
						tmpD = tmpD | 0x20;
					}
					else {
						numLives--;
						LCD_Cursor(32-numLives);
						LCD_WriteData(286);
						
					}
					LCD_Cursor(charPos);
				}
			}
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
			//if (!endGame) {
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
			//}
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
			LCD_Cursor(charPos);
			break;
		case SM2_Down: 
			if (charPos <= 16) {
				charPos += 17;
			}
			LCD_Cursor(charPos);
		case SM2_Left:
			if (charPos != 17 && charPos != 1) {
				charPos--;
			}
			LCD_Cursor(charPos);
			break;
		case SM2_Right:
			if (charPos != 16 && charPos != 32) {
				charPos++;
			}
			LCD_Cursor(charPos);
			break;
		default: break;
	}
	return state;
}
int SMTick3(int state) {
	switch (state) {
		case SM3_Start:
			state = SM3_readX;
			break;
		case SM3_readX:
			//ADMUX = 0x00;
			state = SM3_readY;
			break;
		case SM3_readY:
			//ADMUX = 0x01;
			state = SM3_readX;
			break;
		default: state = SM3_Start; break;
	}
	switch (state) {
		case SM3_Start: break;
		case SM3_readX:
			xPos = ADC_Read(1);
			break;
		case SM3_readY:
			yPos = ADC_Read(2);
			
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
int SMTick4(int state) {
	switch (state) {
		case SM4_Start: state = SM4_Reset; break;
		case SM4_Reset: state = SM4_Countdown; break;
		case SM4_Countdown: 
			//if (gameStart) {
 				if (currTime == 0 || numLives == 0) {
	 				state = SM4_Lose;
 				}
 				else if (endGame) {
	 				state = SM4_Win;
 				}
				else if (resetButton) {
					state = SM4_Reset;
				}
 				else {
	 				state = SM4_Countdown;
 				}
			//}
			break;
		case SM4_Lose: state = SM4_Wait; break;
		case SM4_Win: state = SM4_Wait; break;
		case SM4_Wait:
			if (resetButton && endGame) {
				state = SM4_Start;
			}
			break;
		default: state = SM4_Start; break;
	}
	switch(state) {
		case SM4_Start: 
			currTime = timeLimit; 
			break;
		case SM4_Reset:	
			if (!endGame) {
				currTime_ten = currTime / 10;
				currTime_one = currTime % 10;

				LCD_Cursor(15);
				LCD_WriteData(currTime_ten + '0');
				LCD_WriteData(currTime_one + '0');
				LCD_Cursor(charPos);
			}
			break;
		case SM4_Countdown:
			if (!endGame) {
				currTime--;
				if (currTime_one == 0) {
					currTime_one = 9;
					currTime_ten--;
				}
				else {
					currTime_one--;
				}
				LCD_Cursor(15);
				LCD_WriteData(currTime_ten + '0');
				LCD_Cursor(16);
				LCD_WriteData(currTime_one + '0');
				LCD_Cursor(charPos);
			}
			
			break;
		case SM4_Lose: 
			endGame = 1;
			LCD_ClearScreen();
			LCD_DisplayString(1, "     BOOM!!!     YOU BLEW UP!!!");
			LCD_Cursor(33);
			break;
		case SM4_Win:
			LCD_ClearScreen();
			LCD_DisplayString(1, " Bomb defused.   Good work!");
			LCD_Cursor(33);
		case SM4_Wait: 
			//LCD_Cursor(33); 
			break;
		default: break;
	}
	return state;
}
int SMTick5(int state) {
	switch(state) {
		case SM5_Start: state = SM5_Write; break;
		case SM5_Write: state = SM5_Wait; break;
		case SM5_Wait: state = (resetButton) ? SM5_Start : SM5_Wait; break;
		default: state = SM5_Start; break;
	}
	switch(state) {
		case SM5_Start: break;
		case SM5_Write:	
		/*
			if (gameStart) {
				for (unsigned char a = 3; a < 5 + level; a++) {
					LCD_Cursor(a);
					LCD_WriteData(topHalf[a - 3]); 
					LCD_Cursor(a + 16);
					LCD_WriteData(botHalf[a - 3]); 
				}
				LCD_Cursor(charPos);
			}
				break;
				*/
			LCD_Cursor(3);
			LCD_WriteData('P');
			LCD_Cursor(4);
			LCD_WriteData('E');
			LCD_Cursor(5);
			LCD_WriteData('I');
			LCD_Cursor(19);
			LCD_WriteData('R');
			LCD_Cursor(20);
			LCD_WriteData('E');
			LCD_Cursor(21);
			LCD_WriteData('X');
			break;
			
		case SM5_Wait: break;
		default: break;
	}
	return state;
}
int SMTick6(int state) {
	switch (state) {
		case SM6_Start: state = SM6_Write; break;
		case SM6_Write: state = SM6_Wait; break;
		case SM6_Wait: state = (resetButton) ? SM6_Start : SM6_Wait; break;
		default: state = SM6_Start; break;
	}
	switch (state) {
		case SM6_Start: break;
		case SM6_Write:
		/*
			if (gameStart) {
				if (level == 4) {
					LCD_Cursor(32);
					LCD_WriteData('B');
				}
				else {
					for (int a = 0; a < 2 * level; a++) {
						LCD_Cursor(32 - a);
						LCD_WriteData(healthText[a]);
					}
				}
			}*/
			LCD_Cursor(27);
			LCD_WriteData('H');
			LCD_Cursor(28);
			LCD_WriteData('E');
			LCD_Cursor(29);
			LCD_WriteData('A');
			LCD_Cursor(30);
			LCD_WriteData('L');
			LCD_Cursor(31);
			LCD_WriteData('T');
			LCD_Cursor(32);
			LCD_WriteData('H');
			break;
		case SM6_Wait: break;
		default: break;
	}
	return state;
}
int SMTick7(int state) {
	switch(state) {
		case SM7_Start: state = SM7_Wait; break;
		case SM7_Wait: 
			if (!endGame) {
				if (buttonInput == 0x20) {
					state = SM7_G;
				}
				else if (buttonInput == 0x40) {
					state = SM7_B;
				}
				else if (buttonInput == 0x80) {
					state = SM7_R;
				}
				else {
					state = SM7_Wait;
				}
			}
			if (resetButton) {
				state = SM7_Reset;
			}
			break;
		case SM7_G: state = (buttonInput == 0x20) ? SM7_Hold : (resetButton) ? SM7_Reset : SM7_Wait; break;
		case SM7_R: state = (buttonInput == 0x80) ? SM7_Hold : SM7_Wait; break;
		case SM7_B: state = (buttonInput == 0x40) ? SM7_Hold : SM7_Wait; break;
		case SM7_Hold: state = (resetButton) ? SM7_Reset : (buttonInput == 0x00) ? SM7_Wait : SM7_Hold; break;
		case SM7_Reset: state = SM7_Hold; break;
		default: state = SM7_Start; break;
	}
	switch(state) {
		case SM7_Start: tmpD = 0; break;
		case SM7_Reset: tmpD = 0; break;
		case SM7_Wait: break;
		case SM7_G: tmpD = ((tmpD & 0x08) == 0x08) ? (tmpD & 0xF7) : (tmpD | 0x08);
		case SM7_B: tmpD = ((tmpD & 0x10) == 0x10) ? (tmpD & 0xEF) : (tmpD | 0x10);
		case SM7_R: tmpD = ((tmpD & 0x04) == 0x04) ? (tmpD & 0xFB) : (tmpD | 0x04);
		case SM7_Hold: break;
		default: break;
	}
	return state;
}
int SMTick8(int state) {
	switch (state) {
		case SM8_Start: state = SM8_Choose; break;
		case SM8_Choose: state = SM8_Wait; break;
		case SM8_Wait: 
			if (((tmpD & 0x1C) >> 2) == chosenColor) {
				state = SM8_Match;
			}
			else if (resetButton) {
				state = SM8_Reset;
			}
			else {
				state = SM8_Wait;
			}
			break;
		case SM8_Reset: state = SM8_Hold; break;
		case SM8_Hold: state = (resetButton) ? SM8_Hold : SM8_Choose; break;
		case SM8_Match: break;
		default: state = SM8_Start; break;
	}
	switch (state) {
		case SM8_Start: break;
		case SM8_Choose: chosenColor = rand() % 8; break;
		case SM8_Wait: break;
		case SM8_Reset: tmpD = tmpD & 0xFD; break;
		case SM8_Hold: break;
		case SM8_Match: tmpD = tmpD | 0x02;
		default: break;
	}
	return state;
}
//note: sm9 not being used at the moment
int SMTick9(int state) { //setup based on level
	switch (state) {
		case SM9_Start: state = SM9_Level; break;
		case SM9_Level: state = (click) ? SM9_Setup : SM9_Level; break;
		case SM9_Setup: state = SM9_Wait; break;
		case SM9_Wait: state = (resetButton) ? SM9_Start : SM9_Wait; break;
		case SM9_Brisk: break;
		default: state = SM9_Start; break;
	}
	switch (state) {
		case SM9_Start: 
			if (!gameStart) {
				LCD_DisplayString(1, " Choose Level:    >A  >B  >C");
				charPos = 19;
				level = 1;
				LCD_Cursor(charPos);
			}
			break;
		case SM9_Level: 
			if (tmpA == 0x04 && charPos == 19) {
				charPos = 23;
				level = 2;
			}
			else if (tmpA == 0x03 && charPos == 23) {
				charPos = 19;
				level = 1;
			} 
			else if (tmpA == 0x04 && charPos == 23) {
				charPos = 27;
				level = 3;
			}
			else if (tmpA == 0x03 && charPos == 27) {
				charPos = 23;
				level = 2;
			}
			LCD_Cursor(charPos);
			break;
		case SM9_Setup:
			if (level == 1) { //EXPIRE
				timeLimit = 99;
				numLives = 6;
				healthText[0] = 'H'; healthText[1] = 'E'; healthText[2] = 'A'; 
				healthText[3] = 'L'; healthText[4] = 'T'; healthText[5] = 'H'; 
				topHalf[0] = 'P'; topHalf[1] = 'E'; topHalf[2] = 'I';
				botHalf[0] = 'R'; botHalf[1] = 'X'; botHalf[2] = 'E';
				sortedWord[0] = 'E'; sortedWord[1] = 'X'; sortedWord[2] = 'P'; 
				sortedWord[3] = 'I'; sortedWord[4] = 'R'; sortedWord[5] = 'E'; 
			}
			else if (level == 2) {//HELPLESS
				timeLimit = 80;
				numLives = 4;
				healthText[0] = 'L'; healthText[1] = 'I';
				healthText[2] = 'F'; healthText[3] = 'E';
				topHalf[0] = 'L'; topHalf[1] = 'E'; topHalf[2] = 'P'; topHalf[3] = 'S';
				botHalf[0] = 'S'; botHalf[1] = 'H'; botHalf[2] = 'E'; botHalf[3] = 'L';
				sortedWord[0] = 'H'; sortedWord[1] = 'E'; sortedWord[2] = 'L'; sortedWord[3] = 'P';
				sortedWord[4] = 'L'; sortedWord[5] = 'E'; sortedWord[6] = 'S'; sortedWord[7] = 'S';
			}
			else if (level == 3) {//DEPRESSION
				timeLimit = 60;
				numLives = 2;
				healthText[0] = 'B'; healthText[1] = 'E';
				topHalf[0] = 'O'; topHalf[1] = 'R'; topHalf[2] = 'E'; topHalf[3] = 'I'; topHalf[4] = 'S';
				botHalf[0] = 'N'; botHalf[1] = 'D'; botHalf[2] = 'S'; botHalf[3] = 'P'; botHalf[4] = 'E';
				sortedWord[0] = 'D'; sortedWord[1] = 'E'; sortedWord[2] = 'P'; sortedWord[3] = 'R'; sortedWord[4] = 'E';
				sortedWord[5] = 'S'; sortedWord[6] = 'S'; sortedWord[7] = 'I'; sortedWord[8] = 'O'; sortedWord[9] = 'N';
			}
			else if (level == 4) {// CS120BwBRISK
				timeLimit = 30;
				numLives = 1;
				topHalf[0] = 'C'; topHalf[1] = 'S'; topHalf[2] = 'B'; topHalf[3] = '0'; topHalf[4] = 'w'; topHalf[5] = 'I';
				botHalf[0] = 'K'; botHalf[1] = 'B'; botHalf[2] = '2'; botHalf[3] = 'S'; botHalf[4] = 'R'; botHalf[5] = '1';
				sortedWord[0] = 'D'; sortedWord[1] = 'E'; sortedWord[2] = 'P'; sortedWord[3] = 'R'; sortedWord[4] = 'E'; sortedWord[5] = 'S';
				sortedWord[6] = 'S'; sortedWord[7] = 'I'; sortedWord[8] = 'O'; sortedWord[9] = 'N'; sortedWord[10] = 'N'; sortedWord[11] = 'N';
			}
			letterAt = 0;
			
			break;
		case SM9_Wait: gameStart = 1; break;
		default: break;
	}
	return state;
} //not used atm
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
					ADC_Value2 = ADC_Read(0);
				}
				if (ADC_Value2 == 0) {
					ADC_Value2 = 1000;
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
		if (ADC_Value2 >= 630 && ADC_Value2 <= 670) {
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
			if (ADC_Value2 > 700) { //0
				tmpB = 0x00;
			}
			else if (ADC_Value2 > 690) {//1
				tmpB = 0x71;
			}
			else if (ADC_Value2 > 680) {//2
				tmpB = 0x64;
			}
			else if (ADC_Value2 > 670) {//3
				tmpB = 0x70;
			}
			else if (ADC_Value2 > 660) {//4
				tmpB = 0x69;
			}
			else if (ADC_Value2 > 650) {//5
				tmpB = 0x72;
			}
			else if (ADC_Value2 > 640) {//6
				tmpB = 0x62;
			}
			else if (ADC_Value2 > 630) {//7
				tmpB = 0x78;
			}
			else if (ADC_Value2 > 620) {//8
				tmpB = 0x61;
			}
			else if (ADC_Value2 > 610) {//9
				tmpB = 0x70;
			}
			else if (ADC_Value2 > 500) { //A
				tmpB = 0x6A;
			}
			else {
				tmpB = 0x00;
			}
			
		}
		else {
			tmpB = 0x65;
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
	unsigned long int SMTick2_calc = 100;
 	unsigned long int SMTick3_calc = 10;
	unsigned long int SMTick4_calc = 1000;
 	unsigned long int SMTick5_calc = 10;
	unsigned long int SMTick6_calc = 10;
	unsigned long int SMTick7_calc = 100;
 	unsigned long int SMTick8_calc = 100;
 	//unsigned long int SMTick9_calc = 10;
 	unsigned long int SMTick10_calc = 10;
	unsigned long int SMTick11_calc = 100;
	unsigned long int SMTick12_calc = 100;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
 	tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	tmpGCD = findGCD(tmpGCD, SMTick4_calc);
	tmpGCD = findGCD(tmpGCD, SMTick5_calc);
	tmpGCD = findGCD(tmpGCD, SMTick6_calc);
	tmpGCD = findGCD(tmpGCD, SMTick7_calc);
 	tmpGCD = findGCD(tmpGCD, SMTick8_calc);
 	//tmpGCD = findGCD(tmpGCD, SMTick9_calc);
 	tmpGCD = findGCD(tmpGCD, SMTick10_calc);
	tmpGCD = findGCD(tmpGCD, SMTick11_calc);
	tmpGCD = findGCD(tmpGCD, SMTick12_calc);
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
 	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
 	unsigned long int SMTick5_period = SMTick5_calc/GCD;
 	unsigned long int SMTick6_period = SMTick6_calc/GCD;
	unsigned long int SMTick7_period = SMTick7_calc/GCD;
 	unsigned long int SMTick8_period = SMTick8_calc/GCD;
 	//unsigned long int SMTick9_period = SMTick9_calc/GCD;
 	unsigned long int SMTick10_period = SMTick10_calc/GCD;
	unsigned long int SMTick11_period = SMTick11_calc/GCD;
	unsigned long int SMTick12_period = SMTick12_calc/GCD;

	//Declare an array of tasks
	static task task1, task2, task3, task4, task5, task6, task7, task8, task10, task11, task12;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8, &task10, &task11, &task12};
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

 	// Task 6
	task6.state = -1;//Task initial state.
	task6.period = SMTick6_period;//Task Period.
	task6.elapsedTime = SMTick6_period;//Task current elapsed time.
	task6.TickFct = &SMTick6;//Function pointer for the tick

 	// Task 7
	task7.state = -1;//Task initial state.
	task7.period = SMTick7_period;//Task Period.
	task7.elapsedTime = SMTick7_period;//Task current elapsed time.
	task7.TickFct = &SMTick7;//Function pointer for the tick
 	
 	// Task 8
	task8.state = -1;//Task initial state.
	task8.period = SMTick8_period;//Task Period.
	task8.elapsedTime = SMTick8_period;//Task current elapsed time.
	task8.TickFct = &SMTick8;//Function pointer for the tick

//  	// Task 9
//  	task9.state = -1;//Task initial state.
//  	task9.period = SMTick9_period;//Task Period.
//  	task9.elapsedTime = SMTick9_period;//Task current elapsed time.
//  	task9.TickFct = &SMTick9;//Function pointer for the tick
	 
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
