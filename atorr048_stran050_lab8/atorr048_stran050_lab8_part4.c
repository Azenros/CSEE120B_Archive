/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 8 Exercise # 4
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>

unsigned short number;
unsigned char tmpB;
//unsigned char tmpD;

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0;
	DDRD = 0x03; PORTD = 0;
    ADC_init();

    while (1) 
    {
		number = ADC;
		
		if (number >= 176) {tmpB = 0xFF;}
		else if (number >= 154) {tmpB = 0x7F;}
		else if (number >= 132) {tmpB = 0x3F;}
		else if (number >= 110) {tmpB = 0x1F;}
		else if (number >= 88) {tmpB = 0x0F;}
		else if (number >= 66) {tmpB = 0x07;}
		else if (number >= 44) {tmpB = 0x03;}
		else if (number >= 22) {tmpB = 0x01;}
		else if (number >= 0) {tmpB = 0x00;}
		else {tmpB = 0x00;} 
	
		PORTB = tmpB;
    }
}

//176 / 8 = 22
