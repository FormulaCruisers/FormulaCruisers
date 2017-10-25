
#include "Defines.h"
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"


//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();
#include <stdbool.h>

//***** Initialization ***********************************************
void chip_init (void);

uint32_t cantimer = 0;

bool predison = 0;
volatile uint32_t MainRelaisTimer = 1000;

void init_TIMER0(void)					// 1000Hz Clock Initialization (Timer Counter 0)
{
	TCCR0A |= (1 << CS02);					// 256 Prescaler
	TCNT0 = 193;							// Set initial counter value(1ms)
	TIMSK0 |= (1 << TOIE0);					// Overflow Interrupt Enable
}

//***** MAIN CODE ***********************************************
int main( void )
{
	chip_init();			// Chip initialization
	can_init(BAUD); 		// Can initialization
	can_rx(FUNCTION);
	int_ADC();				// ADC initialization
	int_ExternalInterrupt();
	init_TIMER0();
	
	sei();					// enables interrupts
	
	//DDRC = 0xFF;
	//PORTC = 0xFF;
	
	while(1){
	}
	return(0);
}


//***** chip Initialization **********************************
void chip_init(void){

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;					// no prescaler CLK 16Mhz
}

ISR(TIMER0_OVF_vect)					// Timer 0 Overflow Interrupt(1000Hz)
{
	if ((predison == 1) && (PIND & (1 << PD7)))
	{
		PORTC	|= (1 << PC3);		//Precharge on
		if (MainRelaisTimer-- > 30)
		{
			PORTC	|= (1 << PC2);	// Main relais on
		}
	}
	else
	{
		MainRelaisTimer = 1000;
		PORTC &= ~(1 << PC3);	// Turn pre charge off
		PORTC &= ~(1 << PC2);	// Turn main Relais off
	}
	
	
	/*
	if(cantimer <= CANTIMEOUT){			// CAN TIME OUT TIMER
		cantimer++;
		if (cantimer > CANTIMEOUT){
			PORTC	&= ~(1 << PC0);
		}
	}
	
	*/
	
	TCNT0 = 193;
}
