#ifndef _ADCc_
#define _ADCc_

//***** Defines ***********************************************
#include "Defines.h"

//***** Libraries *********************************************
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"

uint8_t R_L = 0, R_H = 0;

//***** ADC CODE ***********************************************
void int_ADC(void){
	
	if(FUNCTION == ECU2ID){
		
	}
	else{
		DDRC	= 0x0F;
		PORTC	= 0x0F;
		
		DDRF	= 0x00;
		PORTF	= 0x0F;
		
		ADCSRA = (1<<ADEN) | 7;
	}
}

void getADC(uint8_t ADC_pin){
	
	ADMUX = (ADC_pin & 0x0f);
	ADCSRA |= (1<<ADSC) | (1<<ADEN);                 
	while (ADCSRA & (1<<ADSC));
	R_L = ADCL;
	R_H = ADCH;
}

#endif