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

uint16_t ADCValue[4] = {0x10,0x20,0x30,0x40};
	uint8_t ADCVALUEL = 0, ADCVALUEH = 0;
//***** ADC CODE ***********************************************
void int_ADC(void){
	
	if(FUNCTION == ECU2ID){
		
	}
	else{
		DDRC	= 0x0F;
		PORTC	= 0x0F;
		
		DDRF	= 0x00;
		PORTF	= 0x0F;
		
		ADCSRA	= ( 1 << ADPS2 )
		| ( 1 << ADPS1 )
		| ( 1 << ADPS0 );
	}
}

void getADC(uint8_t ADC_pin){
	
	//PORTC	= 0x0F;
	
	ADMUX	= (ADMUX & 0b11111100) | ((ADC_pin + 1 - 4 * (ADC_pin == 3)) & 0b00000011);
	
	ADCSRA |= ( 1 << ADSC) | ( 1 << ADEN  );// start ADC
	
	while(!(ADCSRA & (1 << ADIF))){
	}
	
	ADCSRA &= ~((1 << ADIF) | ( 1 << ADSC) | ( 1 << ADEN  )); // disable ADC
	//PORTC	= 0x00;
	
	uint16_t ReadValue = 0;
	
	ReadValue = ADCL;
	ReadValue = ReadValue | (ADCH << 8);
	
	ADCVALUEL = ADCL;
	ADCVALUEH = ADCH;
	
	ADCValue[ADC_pin & 0b00000011] = ReadValue;
}


ISR(ADC_vect){
	//uint16_t ReadValue = 0;
	//ReadValue = ADCL;
	//ReadValue = ReadValue | (ADCH << 8);
	
	//ADCValue[ADMUX & 0b00000011] = ReadValue;
	
}

#endif