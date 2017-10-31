#include "Defines.h"
#include "CAN.h"
#include "ExternalInterrupt.h"


//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();
#include <stdbool.h>

//***** Initialization ***********************************************
void chip_init (void);

//***** MAIN CODE ***********************************************
int main( void )
{
	chip_init();			// Chip initialization
	can_init(BAUD); 		// Can initialization
	can_rx(FUNCTION);
	//int_ExternalInterrupt();
	
	sei();					// enables interrupts
	
	while(1)
	{
		//TransmitData[0] = SHUTDOWN;
		//TransmitData[1] = 0xFF;
		//can_tx(MASTERID, 2);
		//_delay_ms(100);
	}
	return(0);
}


//***** chip Initialization **********************************
void chip_init(void){

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;					// no prescaler CLK 16Mhz
}