/*
 * Temp acc node.c
 *
 * Created: 08/04/2019 09:55:18
 * Author : pieter van der Lingen, Rishi Harkoe
 
 Status: under development
 */ 

//#define F_CPU 1000000UL //IN DEFINES.H

//defenition 4051 multiplexer
#define Mplxch1A PA3
#define Mplxch1B PA4
#define Mplxch1C PA5

#define Mplxch2C PG2
#define Mplxch2B PC7
#define Mplxch2A PC6


// #define Mplxch2A PC6
// #define Mplxch2B PC7
// #define Mplxch2C PG2


//includes
#include "Defines.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>
#include "CAN.h"

//declarations
uint8_t setmplxch (int channel);
void setup_ADC(void);
void setup_mplx(void);
uint8_t ReadADC(void);
float VoltToTemp(float voltage);
uint8_t SendValue(float * temperature, uint8_t * mobloc, uint8_t len);


int main(void)
{
	//uint8_t measvalue[12];
	//float meastemp[12];
	
	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;				// no prescaler CLK 16Mhz
	
	
	can_init(BAUD); 		// Can initialization
	//can_rx(FUNCTION);	
	//setup_ADC();			// setup ADC
	//setup_mplx();			//setup multiplexers
	
	//start interrupts
	TCCR2A = (1 << WGM21) | (1 << CS20) | (1 << CS21) | (1 << CS22);	//1024 prescaler, CTC mode
	OCR2A = 200;														//30-40Hz
	TIMSK2 = (1 << OCIE2A);												//Enable compare match interrupt
	
	DDRB=0xff;
	PORTB=0x00;
	
	//sei();
	

    while (1)
	{
		_delay_ms(100);
		transmit_data[2]=0x42;
		can_tx(FUNCTION, 8);
		
		
		//_delay_ms(1000);
		//PORTB^=0xff;
	}
    /*{
		_delay_ms(100);              // 50ms delay
		
		PORTC &= !(1 << Mplxch2A);
		PORTC &= !(1 << Mplxch2B);
		PORTG |= (1 << Mplxch2C);
		//connect to channel 4 (multiplexer 1)

		PORTA |= (1 << Mplxch1A);
		PORTA &= !(1 << Mplxch1B);
		PORTA &= !(1 << Mplxch1C);
		//connect to channel 1
	
		
		for(int i = 0; i < 12; i++)
		{
			setmplxch(i);
			_delay_ms(5);
			measvalue[i] = ReadADC();
			_delay_ms(5);
			meastemp[i] = VoltToTemp(0.013 * measvalue[i]);
		}
		
		SendValue(&meastemp, &transmit_data, 12);
    }//*/
}

/*
Function name:	Setup_mplx
Author:			Pieter van der Lingen
Arguments:		-
Return value:	-
Description:	Performs the setup steps for the multiplexers


*/
void setup_mplx(void)
{
	//setting multiplexer channel select output pins
	DDRA |= (1 << Mplxch1A) | (1 << Mplxch1B) | (1 << Mplxch1C);
	DDRG |= (1 << Mplxch2C);
	DDRC |= (1 << Mplxch2B) | (1 << Mplxch2A);
	
}

/*
Function name:	setmplxch
Author:			Pieter van der Lingen
Arguments:		
				uint8_t channel
Return value:	uint8_t Function feedback, 0 is success
Description:	Selects the input channel of the multiplexer
				Selections go from 0-11
				0-1-2
				3-4-5
				6-7-8
				9-10-11
*/
uint8_t setmplxch (int channel)
{
	switch(channel)
	{
		case 0:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
			
			PORTA |= (1 << Mplxch1A);
			PORTA &= !(1 << Mplxch1B);
			PORTA &= !(1 << Mplxch1C);
			//connect to channel 1 
			break;
		case 1:
			PORTC |= (1 << Mplxch2A);
			PORTC |= (1 << Mplxch2B);
			PORTG &= !(1 << Mplxch2C);
			//connect to channel 3
			break;
		case 2:
			PORTC |= (1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG &= !(1 << Mplxch2C);
			//connect to channel 1
			
			break;
			
		case 3:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
			
			PORTA &= !(1 << Mplxch1A);
			PORTA &= !(1 << Mplxch1B);
			PORTA &= !(1 << Mplxch1C);
			//connect to channel 0
			break;
		case 4:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
		
			PORTA |= (1 << Mplxch1A);
			PORTA |= (1 << Mplxch1B);
			PORTA &= !(1 << Mplxch1C);
			//connect to channel 3
			break;
		case 5:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG &= !(1 << Mplxch2C);
			//connect to channel 0
			break;
		case 6:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
		
			PORTA &= !(1 << Mplxch1A);
			PORTA |= (1 << Mplxch1B);
			PORTA |= (1 << Mplxch1C);
			//connect to channel 6
			break;
		case 7:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
		
			PORTA |= (1 << Mplxch1A);
			PORTA &= !(1 << Mplxch1B);
			PORTA |= (1 << Mplxch1C);
			//connect to channel 5
			break;
		case 8: 
			PORTC |= (1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 5
			break;
		case 9:
			PORTC &= !(1 << Mplxch2A);
			PORTC &= !(1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 4 (multiplexer 1)
		
			PORTA &= !(1 << Mplxch1A);
			PORTA |= (1 << Mplxch1B);
			PORTA |= (1 << Mplxch1C);
			//connect to channel 6
			break;
		case 10:
			PORTC &= !(1 << Mplxch2A);
			PORTC |= (1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 6
			break;
		case 11:
			PORTC |= (1 << Mplxch2A);
			PORTC |= (1 << Mplxch2B);
			PORTG |= (1 << Mplxch2C);
			//connect to channel 7
			break;
		default:
			return -1;
		
			
	}
	return 0;
}

/*
Function name:	Setup ADC
Author:			Pieter van der Lingen, Rishi Harkoe
Arguments:		-
Return value:	-
Description:	Performs the setup steps for the ADC of pin PF3/ADC3 single read, external reference voltage


*/
void setup_ADC(void)
{
	ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);		//Setting prescaler for ADC
	ADCSRA &= !(1 << ADPS2);
	
	//ADMUX &= !(1 << REFS1) & !(1 << REFS0);					//Set reference voltage
	ADMUX = 0x23;												// AREF reference, ADC3 selected, ADjusted to left
	
	DIDR0 |= (1 << ADC3D);										//Disable pin digital input 

	ADCSRA|=(1<<ADEN);											//Turning on ADC
	
}

/*
Function name:	Read ADC
Author:			Rishi Harkoe, Pieter van der Lingen
Arguments:		-
Return value:	uint8_t voltage value
Description:	Reads the voltage from the ADC


*/
uint8_t ReadADC(void)
{
	if (!(ADCSRA & (1 << ADSC)))
	{
			//Start Single conversion
			//ADCSRA &= !(1 << ADIF);
			ADCSRA|=(1<<ADSC);
			
			//Wait for conversion to complete
			while(!(ADCSRA & (1<<ADSC)));
			//
			
			return(ADCH);
	}
	else return 0;	
}


/*
Function name:	VoltToTemp
Author:			Rishi Harkoe
Arguments:		float: voltage
Return value:	float: temperature
Description:	Converts the volt value to a temperature value according to the conversion table of the temperature sensors


*/
float VoltToTemp(float voltage)
{
	float temperature = (-271.84*pow(voltage, 6))+(1960.6*pow(voltage, 5))-(3872.2*pow(voltage, 4))-(3807*pow(voltage, 3))+(24267*pow(voltage, 2))-(32203*voltage)+14422;
	if ((temperature < -40.0) || (temperature > 120)) return -50.0;
	else return temperature;
}

/*
Function name:	SendValue
Author:			Pieter van der Lingen
Arguments:		float * tempeature		array of temperatures
				uint8_t * mesloc		location to be stored
				uint8_t len				length of message
Return value:	uint_8t success type
Description:	Stores the highest temperature in a spot and format in which it can be transmitted to the rest of the system

*/
uint8_t SendValue(float *temperature, uint8_t *mobloc, uint8_t len)
{
	uint8_t message = 0;
	
	for (int i = 0; i < len; i++)
	{
		if (message < (uint8_t) temperature[i] && (uint8_t) temperature[i] != 206)
		{
			message = (uint8_t) temperature[i]; //if temp is bigger than last & it is not unusable value (206)
		}
	}
	*mobloc = message;
	return message; 
}



