/*
 * ADC.c
 *
 * Created: 03/07/2019 15:51:58
 *  Author: Jeremy
 */ 

#include <avr/interrupt.h>
#include "ADC.h"

void int_ADC(void)
{
	DDRF	= 0x00;
	PORTF	= 0x0F;

	ADCSRA = (1<<ADEN) | (1<<ADIE) | 7;	
}

void req_ADC(uint8_t pin)
{
	ADMUX = (pin & 0x0f);
	ADCSRA |= (1<<ADSC) | (1<<ADEN) | (1<<ADIE);
}

ISR(ADC_vect)
{
	mcurrent = ADC;
}
