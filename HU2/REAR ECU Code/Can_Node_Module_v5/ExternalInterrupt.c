/* EXTERNALINTERRUPT.C
Contains all interrupt based logic. Used for AMS and IMD shutdown. Also supposedly used for nonexistent rear wheel RPM sensors
*/

#ifndef _EXTERNAL_INTERRUPTc_
#define _EXTERNAL_INTERRUPTc_

//***** Defines ***********************************************
#include "Defines.h"

//***** Libraries *********************************************
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "CAN.h"
#include "ExternalInterrupt.h"

uint16_t pulsetime[4] = {0,0,0,0};


//***** ADC CODE ***********************************************
void int_ExternalInterrupt(void)
{
	PORTD	|= 0b00000100; // Input 2   INT2   PullUp
	PORTD	|= 0b00001000; // Input 3   INT3   PullUp
	PORTE	|= 0b00010000; // Input 4   INT4   PullUp
	PORTE	|= 0b00100000; // Input 5   INT5   PullUp
	
	EIMSK=(0<<INT7)|(0<<INT6)|(1<<INT5)|(1<<INT4)|(1<<INT3)|(1<<INT2)|(0<<INT1)|(0 << INT0);
	
	EICRA =	  (1 << ISC31)
			| (0 << ISC30)
			| (1 << ISC21)
			| (0 << ISC20)
			| (1 << ISC11)
			| (0 << ISC10)
			| (1 << ISC01)
			| (0 << ISC00);
	
	EICRB =	  (1 << ISC71)
			| (0 << ISC70)
			| (1 << ISC61)
			| (0 << ISC60)
			| (1 << ISC51)
			| (0 << ISC50)
			| (1 << ISC41)
			| (0 << ISC40);
}

ISR(INT4_vect)
{
	transmit_data[0] = AMSSHUTDOWN;			// AMS Shutdown
	can_tx(MASTERID, 1);
}

ISR(INT5_vect)
{
	transmit_data[0] = IMDSHUTDOWN;			// IMDSHUTDOWN
	can_tx(MASTERID, 1);
}

#endif