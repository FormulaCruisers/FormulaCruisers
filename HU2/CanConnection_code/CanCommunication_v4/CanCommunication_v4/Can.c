


#include "Can.h"

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

volatile int8_t OntvangData[8];
volatile int8_t ZendData[8];

//***** transmit *****************************************************
void can_tx(void) { 

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = MOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;    	// Clear mob status register
	
	uint16_t Adress = 0x201;	//
	
	CANIDT4 = 0x00;     		// 
	CANIDT3 = 0x00;				// 
	CANIDT2 = Adress << 5;		// 
	CANIDT1 = Adress >> 3;		// 
	
	
	for ( int8_t i = 0; i < 8; ++i ){
		CANMSG = ZendData[i];  
	}
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( 3 << DLC0));

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00;

	CANSTMOB = 0x00;	// Clear TXOK flag
} 


//***** CAN ialization *****************************************************
void can_init(void){     

	CANGCON = ( 1 << SWRES );   // Software reset
	

	CANTCON = 0x00;         // CAN timing prescaler set to 0;	
	
	CANBT1 = 0x02;      	// Set baud rate to 500kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;          // "
	CANBT3 = 0x37; 			// ""

/*	
	CANBT1 = 0x0E;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x04;          // "
	CANBT3 = 0x13; 			// ""

	CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;          // "
	CANBT3 = 0x37; 			// ""
*/

	for ( int8_t mob=0; mob<14; mob++ ) {  

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-14

		CANCDMOB = 0x00;       		// Disable mob

		CANSTMOB = 0x00;     		// Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 1 << IEMOB0 ));   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission

	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 1 << ENTX );   // Enable interrupts on receive
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1

	CANIDM1 = 0x00;   	// Clear Mask, let all IDs pass
	CANIDM2 = 0x00; 	// ""
	CANIDM3 = 0x00; 	// ""
	CANIDM4 = 0x00; 	// ""

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
	
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read

	sei();

} 



//***** Reception ISR **********************************
ISR(CANIT_vect){  				// use interrupts

  	int8_t length, savecanpage;
	savecanpage = CANPAGE;			// Save current MOB
	CANPAGE = CANHPMOB & 0xF0;		// Selects MOB with highest priority interrupt

	if ( CANSTMOB & ( 1 << RXOK) ){  	// Interrupt caused by receive finished	
  		length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
  		for ( int8_t i = 0; i < length; i++ ){
	    	OntvangData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
			USARTWriteChar(OntvangData[i]);
	  	}
		  
		CANSTMOB = 0x00;
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));
	}

	if ( CANSTMOB & ( 1 << TXOK) ){
	}
		
	CANPAGE = savecanpage;		// Restore original MOB
}