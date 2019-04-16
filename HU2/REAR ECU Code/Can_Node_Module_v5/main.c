/* MAIN.C
Contains the entry point of the program
*/

#include "Defines.h"
#include "CAN.h"
#include "ExternalInterrupt.h"


//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>		// include delays for _delay_ms();
#include <stdbool.h>


//***** Initialization ***********************************************
void chip_init (void);




//***** MAIN CODE **********************************************

int main(void)
{
	//get_mcusr();
	wdt_disable();
	//wdt_enable(WDTO_2S);
	//wdt_reset();
	
	chip_init();			// Chip initialization
	can_init(BAUD); 		// Can initialization
	can_rx(FUNCTION);
	int_ExternalInterrupt();
	
	TCCR2A = (1 << WGM21) | (1 << CS20) | (1 << CS21) | (1 << CS22);	//1024 prescaler, CTC mode
	OCR2A = 20;															//300-400Hz
	TIMSK2 = (1 << OCIE2A);												//Enable compare match interrupt
	
	//DDRC	|= (1 << PC2);
	//DDRC	|= (1 << PC3);
	//DDRC	|= (1 << PC4);
	
	
	
	//PORTC	|= (1 << PC4);
	//PORTC	|= (1 << PC3);
	//PORTC	|= (1 << PC2);


	sei();					// enables interrupts
	//wdt_disable();

	while(1)	
	{
		
	}
}


//***** chip Initialization **********************************
void chip_init(void)
{
	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;					// no prescaler CLK 16Mhz
}