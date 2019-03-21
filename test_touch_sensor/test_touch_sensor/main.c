/*
 * test_touch_sensor.c
 *
 * Created: 2/28/2019 1:09:17 PM
 * Author : ruzst
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include "bit.h"
#include "timer.h"
#define DHT11_PIN 6
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

/*
//KY015 DHT11 Temperature and humidity sensor
int DHpin = 8;
byte dat [5];
byte read_data () {
	byte data;
	for (int i = 0; i < 8; i ++) {
		if (digitalRead (DHpin) == LOW) {
			while (digitalRead (DHpin) == LOW); // wait for 50us
			delayMicroseconds (30); // determine the duration of the high level to determine the data is '0 'or '1'
			if (digitalRead (DHpin) == HIGH)
			data |= (1 << (7-i)); // high front and low in the post
			while (digitalRead (DHpin) == HIGH); // data '1 ', wait for the next one receiver
		}
	}
	return data;
}

void start_test () {
	digitalWrite (DHpin, LOW); // bus down, send start signal
	delay (30); // delay greater than 18ms, so DHT11 start signal can be detected
	
	digitalWrite (DHpin, HIGH);
	delayMicroseconds (40); // Wait for DHT11 response
	
	pinMode (DHpin, INPUT);
	while (digitalRead (DHpin) == HIGH);
	delayMicroseconds (80); // DHT11 response, pulled the bus 80us
	if (digitalRead (DHpin) == LOW);
	delayMicroseconds (80); // DHT11 80us after the bus pulled to start sending data
	
	for (int i = 0; i < 4; i ++) // receive temperature and humidity data, the parity bit is not considered
	dat[i] = read_data ();
	
	pinMode (DHpin, OUTPUT);
	digitalWrite (DHpin, HIGH); // send data once after releasing the bus, wait for the host to open the next Start signal
}

void setup () {
	Serial.begin (9600);
	pinMode (DHpin, OUTPUT);
}

void loop () {
	start_test ();
	Serial.print ("Current humdity =");
	Serial.print (dat [0], DEC); // display the humidity-bit integer;
	Serial.print ('.');
	Serial.print (dat [1], DEC); // display the humidity decimal places;
	Serial.println ('%');
	Serial.print ("Current temperature =");
	Serial.print (dat [2], DEC); // display the temperature of integer bits;
	Serial.print ('.');
	Serial.print (dat [3], DEC); // display the temperature of decimal places;
	Serial.println ('C');
	delay (700);
}
*/


/*
Working lab file with interrupts.

Enable interrupts like so (Do not call this function. it is called automatically)
In main:
initUSART();
cli();
sei();

Function:
ISR(USART0_RX_vect)
{
	unsigned char data = UDR0;
	if(data == 0x42)
	{
		rxWritePos = 0;
		rxBuffer[rxWritePos] = data;
		rxWritePos++;
	}
	else if(rxWritePos < RX_BUFFER_SIZE)
	{
		rxBuffer[rxWritePos] = data;
		rxWritePos++;
	}
	else
	{
		//Do nothing, wait for start char.
	}
}

*/

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}


void Request()				/* Microcontroller send start pulse/request */
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);			/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);	/* set to high pin */
}

void Response()				/* receive response from DHT11 */
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN)) {
			/* if high pulse is greater than 30ms */
			c = (c<<1)|(0x01);	/* then its logic HIGH */
		}
		else{			/* otherwise its logic LOW */
			c = (c<<1);
		}
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}

int main(void)
{
	char data[5];
	LCD_init();			/* Initialize LCD */
	LCD_ClearScreen();			/* Clear LCD */
	LCD_Cursor(1);		/* Enter column and row position */
	LCD_DisplayString(1,"Humidity =");
	LCD_Cursor(17);
	LCD_DisplayString(17,"Temp = ");
	
	while(1)
	{
		Request();		/* send start pulse */
		Response();		/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		D_RH=Receive_data();	/* store next eight bit in D_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			LCD_Cursor(1);
			LCD_DisplayString(1, "Error");
		}
		
		else
		{
			itoa(I_RH,data,10);
			LCD_Cursor(11);
			LCD_WriteData(data[0]);
			LCD_WriteData(".");
			
			itoa(D_RH,data,10);
			LCD_WriteData(data[1]);
			LCD_WriteData("%");

			itoa(I_Temp,data,10);
			LCD_Cursor(23);
			LCD_WriteData(data[2]);
			LCD_WriteData(".");
			
			itoa(D_Temp,data,10);
			LCD_WriteData(data[3]);
			//lcddata(0xDF);
			LCD_WriteData("C ");
			
			itoa(CheckSum,data,10);
			LCD_WriteData(data[4]);
			LCD_WriteData(" ");
		}
		
		_delay_ms(10);
	}
}
