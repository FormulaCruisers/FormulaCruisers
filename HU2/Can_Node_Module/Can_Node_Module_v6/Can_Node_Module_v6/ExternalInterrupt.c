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
	TCCR1B |= ( 1 << CS11 ) | (1 << CS10); // 16000000 / 64 = 250000 counts/second
	TCCR3B |= ( 1 << CS31 ) | (1 << CS30); // 16000000 / 64 = 250000 counts/second
	TIMSK1 |= ( 1 << TOIE1);
	TIMSK3 |= ( 1 << TOIE3);
	EICRA =	0xFF;
	EICRB =	0xFF;
}

ISR(INT0_vect)
{
	
}

ISR(INT1_vect)	
{	
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT1L;
	InterruptPairTimerTemp += (TCNT1H << 8);
	
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	
	PulsePerSec[3] = InterruptPairTimerTemp;
}

ISR(INT2_vect)
{
	uint16_t InterruptPairTimerTemp;
	
	InterruptPairTimerTemp = TCNT3L;
	InterruptPairTimerTemp += (TCNT3H << 8);
	
	TCNT3H = 0x00;
	TCNT3L = 0x00;
	
	PulsePerSec[2] = InterruptPairTimerTemp;
}

ISR(INT3_vect)
{
	if (InterruptPairDirection[1])
	{
		InterruptPairTimer[1] = TCNT3L;
		InterruptPairTimer[1] += (TCNT3H << 8);
		
		TCNT3H = 0x00;
		TCNT3L = 0x00;
		
		PulsePerSec[1] = InterruptPairTimer[1];
		Direction[1] = 1;
		
		InterruptPairDirection[1] = 0;
	}
	else
	{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT3L;
		InterruptPairTimerTemp += (TCNT3H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[1]/2))
		{
			InterruptPairDirection[1] = 1;
			Direction[1] = 0;
		}
		else
		{
			InterruptPairDirection[1] = 0;
			Direction[1] = 1;
		}
	}
}

ISR(INT4_vect)
{
	if (InterruptPairDirection[1])
	{
		InterruptPairTimer[1] = TCNT3L;
		InterruptPairTimer[1] += (TCNT3H << 8);
		
		TCNT3H = 0x00;
		TCNT3L = 0x00;
		
		PulsePerSec[1] = InterruptPairTimer[1];
		Direction[1] = 0;
		
		InterruptPairDirection[1] = 0;
	}
	else
	{
		uint16_t InterruptPairTimerTemp;
		
		InterruptPairTimerTemp = TCNT3L;
		InterruptPairTimerTemp += (TCNT3H << 8);
		
		if(InterruptPairTimerTemp < (InterruptPairTimer[1]/2))
		{
			InterruptPairDirection[1] = 1;
			Direction[1] = 1;
		}
		else
		{
			InterruptPairDirection[1] = 0;
			Direction[1] = 0;
		}
	}
}

ISR(INT6_vect)
{
	InterruptPairTimer[0] = TCNT1L;
	InterruptPairTimer[0] += (TCNT1H << 8);
		
	TCNT1H = 0x00;
	TCNT1L = 0x00;
		
	PulsePerSec[0] = InterruptPairTimer[0];
	Direction[0] = 1;
		
	InterruptPairDirection[0] = 0;
}

ISR(INT7_vect)
{
	InterruptPairTimer[0] = TCNT1L;
	InterruptPairTimer[0] += (TCNT1H << 8);
		
	TCNT1H = 0x00;
	TCNT1L = 0x00;
		
	PulsePerSec[0] = InterruptPairTimer[0];
	Direction[0] = 0;
		
	InterruptPairDirection[0] = 0;
}


ISR(TIMER1_OVF_vect)
{
	PulsePerSec[0] = -1;
	PulsePerSec[3] = -1;
	
	Direction[0] = 1;
}


ISR(TIMER3_OVF_vect)
{
	PulsePerSec[2] = -1;
	PulsePerSec[1] = -1;
	
	Direction[1] = 1;
}

#endif