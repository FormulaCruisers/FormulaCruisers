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
void int_ExternalInterrupt(void)
{	
	if(FUNCTION == NODEID1){
		//PORTD	|= 1<<PD7; // Input 1   INT1   PullUp
		//PORTD	|= 1<<PD6; // Input 2   INT2   PullUp
		PORTE |= (1<<PE7) | (1<<PE6);
		
		TCCR1B |= ( 1 << CS12 ) | (1 << CS10); // 16000000 / 1024 = 15625 counts/second
		TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
		EIMSK=(1<<INT7)|(1<<INT6)|(0<<INT5)|(0<<INT4)|(0<<INT3)|(0<<INT2)|(0<<INT1)|(0 << INT0);
	}

	if(FUNCTION == NODEID2){
	}

	if(FUNCTION == NODEID3){
		PORTD	|= 0b00000100; // Input 2   INT2   PullUp
		TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
		EIMSK=(0<<INT7)|(0<<INT6)|(0<<INT5)|(0<<INT4)|(0<<INT3)|(1<<INT2)|(0<<INT1)|(0 << INT0);
	}

	if(FUNCTION == NODEID4){
		PORTD	|= 0b00000100; // Input 2   INT2   PullUp
		TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
		EIMSK=(0<<INT7)|(0<<INT6)|(0<<INT5)|(0<<INT4)|(0<<INT3)|(1<<INT2)|(0<<INT1)|(0 << INT0);
	}

	if(FUNCTION == ECU2ID){
		PORTE	|= 0b10000000; // Input 5   INT7   PullUp
		PORTE	|= 0b01000000; // Input 6   INT6   PullUp
		
		TCCR1B |= ( 1 << CS12 ) | (1 << CS10); // 16000000 / 1024 = 15625 counts/second
		TCCR3B |= ( 1 << CS32 ) | (1 << CS30); // 16000000 / 1024 = 15625 counts/second
		EIMSK=(1<<INT7)|(1<<INT6)|(0<<INT5)|(0<<INT4)|(0<<INT3)|(0<<INT2)|(0<<INT1)|(0 << INT0);
	}
	
	
	//PORTD	|= 0b00000010; // Input 1   INT1   PullUp
	//PORTD	|= 0b00000100; // Input 2   INT2   PullUp
	//PORTD	|= 0b00001000; // Input 3   INT3   PullUp
	//PORTE	|= 0b00010000; // Input 4   INT4   PullUp
	//PORTE	|= 0b10000000; // Input 5   INT7   PullUp
	//PORTE	|= 0b01000000; // Input 6   INT6   PullUp
	
	TCCR1B |= ( 1 << CS12 ); // 16000000 / 1024 = 15625 counts/second
	TCCR3B |= ( 1 << CS32 ); // 16000000 / 1024 = 15625 counts/second
	
	TIMSK1 |= ( 1 << TOIE1);
	TIMSK3 |= ( 1 << TOIE3);
	
	EICRA =	0xFF;
	
	EICRB =	0xFF;
}

ISR(INT0_vect){
	
}

ISR(INT6_vect){   //Should be INT6 on node1, INT1 everywhere else... Dangit rens
	
	
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT1L;
	InterruptPairTimerTemp += (TCNT1H << 8);
	
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	
	PulsePerSec[3] = 15625 * 60 / InterruptPairTimerTemp;
	
	//TransmitData[0] = (PulsePerSec[3] << 8);
	//TransmitData[1] = PulsePerSec[3];
	//can_tx(MASTERID, 2);
}

ISR(INT7_vect){  //Should be INT7 on node1, INT2 everywhere else... Dangit rens
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT3L;
	InterruptPairTimerTemp += (TCNT3H << 8);
	
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	PulsePerSec[2] = 15625 * 60 / InterruptPairTimerTemp;
	
	//TransmitData[0] = (PulsePerSec[2] << 8);
	//TransmitData[1] = PulsePerSec[2];
	//can_tx(MASTERID, 2);
}

ISR(INT3_vect){
	/*
	if (InterruptPairDirection[1]){
		
		InterruptPairTimer[1] = TCNT3L;
		InterruptPairTimer[1] += (TCNT3H << 8);
		
		TCNT3H = 0x00;
		TCNT3L = 0x00;
		
		PulsePerSec[1] = 15625 / InterruptPairTimer[1];
		Direction[1] = 1;
		
		InterruptPairDirection[1] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT3L;
		InterruptPairTimerTemp += (TCNT3H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[1]/2)){
			InterruptPairDirection[1] = 1;
			Direction[1] = 0;
		}
		else{
			InterruptPairDirection[1] = 0;
			Direction[1] = 1;
		}
	}
	//*/
}

ISR(INT4_vect){
	/*
	if (InterruptPairDirection[1]){
		
		InterruptPairTimer[1] = TCNT3L;
		InterruptPairTimer[1] += (TCNT3H << 8);
		
		TCNT3H = 0x00;
		TCNT3L = 0x00;
		
		PulsePerSec[1] = 15625 / InterruptPairTimer[1];
		Direction[1] = 0;
		
		InterruptPairDirection[1] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT3L;
		InterruptPairTimerTemp += (TCNT3H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[1]/2)){
			InterruptPairDirection[1] = 1;
			Direction[1] = 1;
		}
		else{
			InterruptPairDirection[1] = 0;
			Direction[1] = 0;
		}
	}
	//*/
}

ISR(INT1_vect){ //Should be INT1 on node1, INT6 everywhere else... Dangit rens
	if (InterruptPairDirection[0]){
		
		InterruptPairTimer[0] = TCNT1L;
		InterruptPairTimer[0] += (TCNT1H << 8);
		
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		
		PulsePerSec[0] = 15625 / InterruptPairTimer[0];
		Direction[0] = 1;
		
		InterruptPairDirection[0] = 0;
	}
	else{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT1L;
		InterruptPairTimerTemp += (TCNT1H << 8);
		
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

ISR(INT2_vect){ //Should be INT2 on node1, INT7 everywhere else... Dangit rens
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
	PulsePerSec[2] = 0;
	PulsePerSec[1] = 0;
	
	Direction[1] = 1;
}

#endif