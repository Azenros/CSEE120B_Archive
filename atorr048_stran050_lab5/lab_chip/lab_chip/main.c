

#include <avr/io.h>


int main(void) {
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs
	while(1) {
		PORTC = 0xFF; // Writes port B's 8 pins with 00001111
	}
}


