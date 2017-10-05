
#define F_CPU 16000000UL	// Define processer clock speed for compiler

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "Can.h"
#include <util/delay.h>		// include delays for _delay_ms();

//This function is used to initialize the USART
//at a given UBRR value

void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

//void can_tx (void);
//void chip_init (void);
//void can_init (void);

int main( void )
{
   char data;
   uint8_t error;
   uint8_t mobb;

   USARTInit(51);    //UBRR = 51
   chip_init();	// Chip initialization
   can_init(); 	// Can initialization
   DDRE = 0x10;
   PORTE &= ~(1<<PORTE4);

   //Loop forever

	error = CANSTMOB;

   while(1)
   {
      data=USARTReadChar();
	  
   }   
   return(0);
}



void USARTInit(uint16_t ubrr_value){

   //Set Baud rate

   UBRR0L = ubrr_value;
   UBRR0H = (ubrr_value>>8);

   /*Set Frame Format


   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit

   >> char size 8

   */

   UCSR0C=(0<<UMSEL0)|(1<<UCSZ00)|(1<<UCSZ01);


   //Enable The receiver and transmitter

   UCSR0B=(1<<RXEN0)|(1<<TXEN0);


}


//This function is used to read the available data
//from USART. This function will wait untill data is
//available.
char USARTReadChar( void )
{
   //Wait untill a data is available

   if(!(UCSR0A & (1<<RXC0)))
   {
	   return 0;
   }

   //Now USART has got data from host
   //and is available is buffer
   
   
   USARTWriteChar(UDR0);
   //USARTWriteChar('\n');
   
   ZendData[0] = 0x3D;
   ZendData[1] = 0x1B;
   ZendData[2] = 0x00;
   can_tx(); // transmit
   

   return UDR0;
}


//This fuction writes the given "data" to
//the USART which then transmit it via TX line
void USARTWriteChar(char data)
{
   //Wait untill the transmitter is ready

   while(!(UCSR0A & (1<<UDRE0)))
   {
      //Do nothing
   }

   //Now write the data to USART buffer

   UDR0=data;
}


//***** chip Initialization **********************************
void chip_init(void){

// If using crystal oscillator of 16 Mhz	
// Note - May want to set the CKDIV8 fuse if MCU is only supposed to run 
// at 8MHz so as not to exceed its operating conditions at startup.

	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable

	CLKPR = 0x00;				// Clock Division Factor of 2
	
// Note - CLKPR must be written within 4 clock cycles of setting CLKPCE	
	
	//DDRB = 0x00;	// PORT B to be outputs for LED monotoring

	//DDRC = 0x00;    // Inputs, not used

	//DDRD = 0x00; 	// Inputs, not used

	//DDRE = 0x10;	// stat led output

	//PORTB = 0x00;	// ALL LEDs OFF

	//PORTC = 0x00;	// Inputs, not used

	//PORTD = 0x00;	// Inputs, not used

	//PORTE = 0x00;	// Inputs, not used

	//PRR = 0x00;	// Individual peripheral clocks enabled 

}




