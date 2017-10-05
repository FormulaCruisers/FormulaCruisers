#ifndef _ADCc_
#define _ADCc_

#include "Defines.h"

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "CAN.h"
#include "USART.h"
#include "ADC.h"

uint16_t ADCValue[4] = {0x10,0x20,0x30,0x40};

void int_ADC(void){
	DDRF	= 0x00;
	PORTF	= 0x00;
	
	ADMUX	= ( 1 << REFS0 )
			| ( 1 << REFS1 )
			| ( 0 << ADLAR )
			| ( 0 << MUX4 )
			| ( 0 << MUX3 )
			| ( 0 << MUX2 )
			| ( 0 << MUX1 )
			| ( 0 << MUX0 );
			
	ADCSRA	= ( 1 << ADEN )
			| ( 0 << ADSC )
			| ( 0 << ADATE)
			| ( 0 << ADIF )
			| ( 1 << ADIE )
			| ( 1 << ADPS2 )
			| ( 1 << ADPS1 )
			| ( 1 << ADPS0 );
}

void getADC(uint8_t ADC_pin){
	ADMUX	= (ADMUX & 0b11111100) | (ADC_pin & 0b00000011);
	ADCSRA |= ( 1 << ADSC);
}


ISR(ADC_vect){
	uint16_t ReadValue = 0;
	ReadValue = ADCL;
	ReadValue = ReadValue | (ADCH << 8);
	
	ADCValue[ADMUX & 0b00000011] = ReadValue;
	
	//USARTWriteChar((i & 0xFF00) >> 8);
	//USARTWriteChar(i & 0x00FF);
	USARTWriteChar(ReadValue >> 8);
	USARTWriteChar(ReadValue);
}




#endif