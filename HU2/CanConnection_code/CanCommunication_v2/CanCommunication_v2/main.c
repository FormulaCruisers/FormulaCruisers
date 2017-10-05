
#define F_CPU 16000000UL	// Define processer clock speed for compiler

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

//This function is used to initialize the USART
//at a given UBRR value

void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

void can_tx (void);
void chip_init (void);
void can_init (void);
void Motor_adress_change( void );

volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];

int main( void )
{
   char data;

   USARTInit(51);    //UBRR = 51
   chip_init();	// Chip initialization
   can_init(); 	// Can initialization
   DDRE = 0x10;
   PORTE &= ~(1<<PORTE4);

   //Loop forever



   while(1)
   {
      data=USARTReadChar();

      USARTWriteChar('[');
      USARTWriteChar(data);
      USARTWriteChar(']');	  
	  USARTWriteChar('\n');
	  
	  
	  ZendData[0] = 0x3d;
	  ZendData[1] = 0x40z;
	  ZendData[2] = 0x00;
	  can_tx(); // transmit
	  
   }   
   return(0);
}


//***** Reception ISR **********************************
ISR(CANIT_vect){  				// use interrupts

  	int8_t length, savecanpage;
	
	savecanpage = CANPAGE;			// Save current MOB

	CANPAGE = CANHPMOB & 0xF0;		// Selects MOB with highest priority interrupt

	if ( CANSTMOB & ( 1 << RXOK) ){  	// Interrupt caused by receive finished	
		
		//CANSTMOB &= ~( 1 << RXOK); 		// Reset reason on selected channel					

  		length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received

  		for ( int8_t i = 0; i < length; i++ ){
			
	    	OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
			
			USARTWriteChar('[');
			USARTWriteChar(OntvangData[i]);
			USARTWriteChar(']');
			

	  	} // for
		  
		USARTWriteChar('\n');

		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception 11 bit IDE DLC8
		
		CANSTMOB &= ~( 1 << RXOK) ; 
		
	}
	
	//CANSTMOB = 0x00; 		// Reset reason on selected channel

	CANPAGE = savecanpage;		// Restore original MOB

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

   while(!(UCSR0A & (1<<RXC0)))
   {
      //Do nothing
   }

   //Now USART has got data from host
   //and is available is buffer

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

//***** CAN ialization *****************************************************
void can_init(void){     

	CANGCON = ( 1 << SWRES );   // Software reset
	

	CANTCON = 0x00;         // CAN timing prescaler set to 0;	
	
	
	CANBT1 = 0x0E;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x04;          // "
	CANBT3 = 0x13; 			// ""		
	
			
/*
	CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;          // "
	CANBT3 = 0x37; 			// ""

*/
	for ( int8_t mob=0; mob<14; mob++ ) {  

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-5

		CANCDMOB = 0x00;       		// Disable mob

		CANSTMOB = 0x00;     		// Clear mob status register;
	}

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1

	CANIE2 = ( 1 << IEMOB1 );   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission

	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 1 << ENTX );   // Enable interrupts on receive
	
	CANIDM1 = 0x00;   	// Clear Mask, let all IDs pass    

	CANIDM2 = 0x00; 	// ""

	CANIDM3 = 0x00; 	// ""

	CANIDM4 = 0x00; 	// ""    	

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
	
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read

	sei();

} 




//***** transmit *****************************************************
void can_tx(void) { 

	CANPAGE = 0x00;		// Select MOb0 for transmission
	
	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;    	// Clear mob status register
	
	uint16_t Adress = 202;
	
	CANIDT4 = 0x00;     		// 
	CANIDT3 = 0x00;				// 
	CANIDT2 = Adress << 5;		// 
	CANIDT1 = Adress >> 3;		// 
	
	/*
	CANIDT4 = Adress << 3;     	//
	CANIDT3 = Adress >> 5;		//
	CANIDT2 = 0x00;				//
	CANIDT1 = 0x00;				//
	
	*/
	for ( int8_t i = 0; i < 8; ++i ){

		CANMSG = ZendData[i];
  
	} // for
	
	CANCDMOB = ( 1 << CONMOB0 ) | ( 0 << IDE ) | ( 3 << DLC0 ); 	// Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))			

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
// todo: have this use interrupts

	//CANCDMOB = ( 1 << CONMOB1) | ( 0 << IDE ) | ( 8 << DLC0);  // Enable Reception | 11 bit | IDE DLC8
	CANCDMOB = 0x00;	// Disable Transmission

	CANSTMOB = 0x00;	// Clear TXOK flag
} 
