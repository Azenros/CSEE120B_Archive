/*	Partner(s) Name & E-mail: Steven Tran (stran050@ucr.edu), Austin Torreflores (atorr048@ucr.edu)
 *	Lab Section: 22
 *	Assignment: Lab # 8 Exercise # 1
 *	
 *	I acknowledge all content contained herein, excluding template or example code, is my own original work.
 */

#include <avr/io.h>

unsigned short number;
unsigned char tmpB;
unsigned char tmpD;

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
		tmpB = (char)number;
		tmpD = (char)(number >> 8);
		PORTB = tmpB;
		PORTD = tmpD;
    }
}

