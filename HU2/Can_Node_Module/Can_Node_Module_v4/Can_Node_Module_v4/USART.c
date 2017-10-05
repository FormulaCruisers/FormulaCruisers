#ifndef _USARTc_
#define _USARTc_

#include "Defines.h"
#include "USART.h"

#include <avr/io.h>

void USARTInit(uint16_t ubrr_value){

	//Set Baud rate
	UBRR0L = ubrr_value;
	UBRR0H = (ubrr_value>>8);

	/*	Set Frame Format:
	>> Asynchronous mode
	>> No Parity
	>> 1 StopBit
	>> char size 8		*/
	UCSR0C=(0<<UMSEL0)|(1<<UCSZ00)|(1<<UCSZ01);

	//Enable The receiver and transmitter
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
}


//This function is used to read the available data
//from USART. This function will wait until data is
//available.
char USARTReadChar( void )
{
	while(!(UCSR0A & (1<<RXC0)))	{}	// Wait until data is ready on the RS232

	USARTWriteChar(UDR0);
	//USARTWriteChar('\n');

	return UDR0;	// Return data from function
}


//This function writes the given "data" to
//the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
	while(!(UCSR0A & (1<<UDRE0))){}	//Wait until previous writing to rs232 is ready

	UDR0=data;	// Write data to RS232
}


#endif