
#include "Defines.h"
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"


//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

//***** Initialization ***********************************************
void chip_init (void);

//***** MAIN CODE ***********************************************
int main( void )
{
	chip_init();			// Chip initialization
	can_init(BAUD); 		// Can initialization
	can_rx(FUNCTION);
	int_ADC();				// ADC initialization
	
	int_ExternalInterrupt();
	
	sei();					// enables interrupts
	
	if (FUNCTION == NODEID3){
		
		TCCR0A = (1<<CS02)|(0<<CS01)|(1<<CS00);
		OCR0A = 0x9C;
		TIMSK0 = (1<<OCIE0A);
		
	}
	
		while(1)
		{
			
		}
	return(0);
}


//***** chip Initialization **********************************
void chip_init(void){

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;				// no prescaler CLK 16Mhz
}



#define ADC1LOW		680
#define ADC2LOW		500
#define ADC3LOW		90

#define MAXSPEED	50


ISR(TIMER0_COMP_vect){
	TCNT0 = 0;
	
	uint8_t j = 0;
	
	getADC(0);
	if(ADCValue[0] > ADC1LOW){	ADCValue[0] = MAXSPEED * (ADCValue[0]-ADC1LOW);}
	else{						ADCValue[0] = 0;}
	
	TransmitData[j++] = 0x90;
	TransmitData[j++] = ADCValue[0];
	TransmitData[j++] = (ADCValue[0] >> 8);
	
	can_tx(0x0210, j);
	j = 0;
	
	TransmitData[j++] = 0x90;
	TransmitData[j++] = ADCValue[0];
	TransmitData[j++] = (ADCValue[0] >> 8);
	
	can_tx(0x0211, j);
	j = 0;
	
	
	
	/*
	getADC(1);
	if(ADCValue[1] > ADC2LOW){	ADCValue[1] = 1 * (ADCValue[1]-ADC2LOW);}
	else{						ADCValue[1] = 0;}
	
	
	TransmitData[j++] = 0x31;
	TransmitData[j++] = (ADCValue[1] >> 8);
	TransmitData[j++] = ADCValue[1];
	
	can_tx(0x0210, j);
	j = 0;
	*/
	
	/*
	getADC(2);
	ADCValue[2] = 1024 - ADCValue[2];
	
	if(ADCValue[2] > ADC3LOW){	ADCValue[2] = 1 * (ADCValue[2]-ADC3LOW);}
	else{						ADCValue[2] = 0;}
	
	
	TransmitData[j++] = 0x31;
	TransmitData[j++] = (ADCValue[2] >> 8);
	TransmitData[j++] = ADCValue[2];
	
	can_tx(0x0210, j);
	j = 0;
	*/
	
	
}