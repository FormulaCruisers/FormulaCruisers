
#include "Defines.h"
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"


//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

//***** Initialization ***********************************************
void chip_init (void);

//***** MAIN CODE ***********************************************
int main( void )
{
	chip_init();			// Chip initialization
	can_init(BAUD); 		// Can initialization
	can_rx(FUNCTION);
	int_ADC();				// ADC initialization
	int_ExternalInterrupt();
	
	sei();					// enables interrupts
	
		while(1){}
		
	return(0);
}


//***** chip Initialization **********************************
void chip_init(void){

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;				// no prescaler CLK 16Mhz
}