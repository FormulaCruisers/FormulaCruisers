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

#define TINT_DIV 15625

uint8_t InterruptPairDirection[2] = {0x00,0x00};
uint16_t InterruptPairTimer[2] = {0x0000,0x0000};

uint16_t PulsePerSec[4] = {0,0,0,0};
uint8_t Direction[2] = {1,1};


//***** ADC CODE ***********************************************
void int_ExternalInterrupt(void){
	PORTD	|= 0b00000100; // Input 2   INT2   PullUp
	PORTD	|= 0b00001000; // Input 3   INT3   PullUp
	PORTE	|= 0b00010000; // Input 4   INT4   PullUp
	PORTE	|= 0b00100000; // Input 5   INT5   PullUp
	
	TCCR1B |= ( 1 << CS12 ) | (1 << CS10); // 16000000 / 1024 = 15625 counts/second
	TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
	EIMSK=(0<<INT7)|(0<<INT6)|(1<<INT5)|(1<<INT4)|(1<<INT3)|(1<<INT2)|(0<<INT1)|(0 << INT0);
	
	TIMSK1 |= ( 1 << TOIE1);
	TIMSK3 |= ( 1 << TOIE3);
	
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
			| (0 << ISC51)
			| (0 << ISC50)
			| (1 << ISC41)
			| (0 << ISC40);
}

ISR(INT2_vect)
{
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT3L;
	InterruptPairTimerTemp += (TCNT3H << 8);
	
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	PulsePerSec[_LEFT] = TINT_DIV / InterruptPairTimerTemp;
	
	TransmitData[0] = (PulsePerSec[_LEFT] << 8);
	TransmitData[1] = PulsePerSec[_LEFT];
	can_tx(MASTERID, 2);
}

ISR(INT3_vect){
	if (InterruptPairDirection[1]){
		
		InterruptPairTimer[1] = TCNT3L;
		InterruptPairTimer[1] += (TCNT3H << 8);
		
		TCNT3H = 0x00;
		TCNT3L = 0x00;
		
		PulsePerSec[_RIGHT] = TINT_DIV / InterruptPairTimer[1];
		Direction[_RIGHT] = 1;
		
		InterruptPairDirection[1] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT3L;
		InterruptPairTimerTemp += (TCNT3H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[1]/2)){
			InterruptPairDirection[1] = 1;
			Direction[_RIGHT] = 0;
		}
		else{
			InterruptPairDirection[1] = 0;
			Direction[_RIGHT] = 1;
		}
	}
}

ISR(INT4_vect){
//	TransmitData[0] = AMSSHUTDOWN;			// AMS Shutdown
//	can_tx(MASTERID, 1);
}

ISR(INT5_vect){
//	TransmitData[0] = IMDSHUTDOWN;			// IMDSHUTDOWN
//	can_tx(MASTERID, 1);
}

ISR(TIMER3_OVF_vect)
{
	PulsePerSec[_RIGHT] = 0;
	PulsePerSec[_LEFT] = 0;
	
	Direction[_RIGHT] = 1;
}

#endif