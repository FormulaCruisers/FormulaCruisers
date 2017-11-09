/*
 * Timing.c
 *
 * Created: 2017-10-26 14:40:54
 *  Author: Jeremy
 */ 

#include "Defines.h"
#include "Timing.h"

void (*funcs[0xFF])();
uint16_t timesper[0xFF] = {0};
uint8_t n_funcs = 0;
uint16_t tmr = 0;

void init_timers()
{
	//Initialize timer0
	TCCR0A |= (1 << CS02);					//256 Prescaler
	TCNT0 = _TM0;							//Set initial counter value
	TIMSK0 |= (1 << TOIE0);					//Overflow Interrupt Enable
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = _TM0;
	for(uint8_t i = 0; i < n_funcs; i++)
	{
		uint16_t tper = timesper[i];
		if(tper != 0)
		{
			if(tmr % tper == 0) (*funcs[i])();
		}
	}
	tmr++;
}


// Sets a function to loop on a specified timer, as specified by the first argument tper. Returns the looping function ID
// tper | ms delay
// 0      1	   (once per ms)
// 1      2
// 2      4
// 3      8
// 4      16
// ...    
// 10     1024 (once per second, but actually once per 1024 ms)
// 11     2048
// ...etc
uint8_t set_timefunc(uint8_t tper, void (*f)())
{
	funcs[n_funcs] = f;
	timesper[n_funcs] = (1<<tper);
	
	return n_funcs++;
}