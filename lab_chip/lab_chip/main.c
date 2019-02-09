

#include <avr/io.h>


int main(void) {
	DDRA = 0xFF; PORTA = 0x00; // Configure port A's 8 pins as outputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs
	DDRD = 0xFF; PORTD = 0x00;
	while(1) {
		PORTA = 0xFF; // Writes port A's 8 pins with 00001111
		PORTB = 0xFF; // Writes port B's 8 pins with 00001111
		PORTC = 0xFF; // Writes port C's 8 pins with 00001111
		PORTD = 0xFF;
	}
}


