/* EXTERNALINTERRUPT.C
Contains all interrupt based logic. Used for AMS and IMD shutdown. Also contains pseudo-PWM for pumps
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

uint8_t pump_pwm = 0x01;


//***** ADC CODE ***********************************************
void int_ExternalInterrupt(void)
{
	PORTD	|= 0b00000100; // Input 2   INT2   PullUp
	PORTD	|= 0b00001000; // Input 3   INT3   PullUp
	PORTE	|= 0b00010000; // Input 4   INT4   PullUp
	PORTE	|= 0b00100000; // Input 5   INT5   PullUp
	
	TCCR0A = (1 << CS01);
	OCR0A = pump_pwm;
	TIMSK0 = 0;
	
	EIMSK=(0<<INT7)|(0<<INT6)|(1<<INT5)|(1<<INT4)|(0<<INT3)|(0<<INT2)|(0<<INT1)|(0 << INT0);
	
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

ISR(TIMER0_COMP_vect)
{
	//On compare match, turn off pumps
	//if(pump_pwm != 0xFF) PORTC &= ~(1<<PC5);
}

ISR(TIMER0_OVF_vect)
{
	//On overflow, turn on pumps
	//PORTC |= 1<<PC5;
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