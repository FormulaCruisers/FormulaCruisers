#ifndef _EXTERNAL_INTERRUPTc_
#define _EXTERNAL_INTERRUPTc_

//***** Defines ***********************************************
#include "Defines.h"

//***** Libraries *********************************************
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"

uint8_t InterruptPairDirection[2] = {0x00,0x00};
uint16_t InterruptPairTimer[2] = {0x0000,0x0000};

uint16_t PulsePerSec[4] = {0,0,0,0};
uint8_t Direction[2] = {1,1};


//***** ADC CODE ***********************************************
void int_ExternalInterrupt(void){
	PORTE	|= 0b00000000; // Input 5   INT7   PullUp
	PORTE	|= 0b00000000; // Input 6   INT6   PullUp
	
	TCCR1B |= ( 1 << CS12 ) | (1 << CS10); // 16000000 / 1024 = 15625 counts/second
	TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
	EIMSK=(0<<INT7)|(0<<INT6)|(1<<INT5)|(1<<INT4)|(0<<INT3)|(0<<INT2)|(0<<INT1)|(0 << INT0);
	
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

ISR(INT0_vect){
	
}

ISR(INT1_vect)
{
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT1L;
	InterruptPairTimerTemp += (TCNT1H << 8);
	
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	PulsePerSec[3] = 15625 / InterruptPairTimerTemp;
	
	TransmitData[0] = (PulsePerSec[3] << 8);
	TransmitData[1] = PulsePerSec[3];
	can_tx(MASTERID, 2);
}

ISR(INT2_vect)
{
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT3L;
	InterruptPairTimerTemp += (TCNT3H << 8);
	
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	PulsePerSec[_LEFT] = 15625 / InterruptPairTimerTemp;
	
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
		
		PulsePerSec[_RIGHT] = 15625 / InterruptPairTimer[1];
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

//ISR(INT4_vect){
//	TransmitData[0] = 0x99;			// AMS Shutdown
//	can_tx(MASTERID, 1);
//}

//ISR(INT5_vect){
//	TransmitData[0] = 0x98;			// IMDSHUTDOWN
//	can_tx(MASTERID, 1);
//}

ISR(INT6_vect){
	if (InterruptPairDirection[0]){
		
		InterruptPairTimer[0] = TCNT1;
		
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		
		PulsePerSec[0] = 15625 / InterruptPairTimer[0];
		Direction[0] = 1;
		
		InterruptPairDirection[0] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT1;
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[0]/2)){
			InterruptPairDirection[0] = 1;
			Direction[0] = 0;
		}
		else{
			InterruptPairDirection[0] = 0;
			Direction[0] = 1;
		}
	}
}

ISR(INT7_vect){
	if (InterruptPairDirection[0]){
		
		InterruptPairTimer[0] = TCNT1L;
		InterruptPairTimer[0] += (TCNT1H << 8);
		
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		
		PulsePerSec[0] = 15625 / InterruptPairTimer[0];
		Direction[0] = 0;
		
		InterruptPairDirection[0] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT1L;
		InterruptPairTimerTemp += (TCNT1H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[0]/2)){
			InterruptPairDirection[0] = 1;
			Direction[0] = 1;
		}
		else{
			InterruptPairDirection[0] = 0;
			Direction[0] = 0;
		}
	}
}


ISR(TIMER1_OVF_vect)
{
	PulsePerSec[0] = 0;
	PulsePerSec[3] = 0;
	
	Direction[0] = 1;
}


ISR(TIMER3_OVF_vect)
{
	PulsePerSec[_RIGHT] = 0;
	PulsePerSec[_LEFT] = 0;
	
	Direction[_RIGHT] = 1;
}

#endif