/* CAN.C
Contains the basic CAN functions for receiving and sending data.
Also contains the interrupt for CAN rx.
*/

#ifndef _CANc_
#define _CANc_

//***** Defines ***********************************************
#include "Defines.h"

//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include "CAN.h"

uint8_t receive_data[8];
uint8_t transmit_data[8];

uint8_t is_enabled[16] = {0};
	
uint8_t sensors[4] = {0};

uint8_t sp = 0;

//***** Reception ISR **********************************
ISR(CANIT_vect)
{	
	DDRB=0xff;
	PORTB^=0xff;
	if (CANSTMOB & (1 << RXOK))
	{
	
		//Clear RXOK flag and re-enable reception
		CANSTMOB = 0x00;
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));
	}
}

ISR(TIMER2_COMP_vect)
{
	transmit_data[1]=0x69;
	can_tx(FUNCTION, 8);
}





//***** CAN ialization *****************************************************
void can_init(uint16_t Baud){
	DDRD = 0x80;
	PORTD &= ~(1<<PD7); // Enable Can-chip
	
	CANGCON = ( 1 << SWRES );   // Software reset
	
	CANTCON = 0x00;       // CAN timing prescaler set to 0;
	
	if (Baud == 1000)
	{
		CANBT1 = 0x00;    // Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;    // "
		CANBT3 = 0x36; 	  // ""
	}
	if (Baud == 500)
	{
		CANBT1 = 0x02;    // Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;    // "
		CANBT3 = 0x37; 	  // ""
	}
	if (Baud == 250)
	{
		CANBT1 = 0x0E;   // Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x04;   // "
		CANBT3 = 0x13; 	 // ""
	}
	if ( Baud == 125)
	{
		CANBT1 = 0x06;  // Set baud rate to 125kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;  // "
		CANBT3 = 0x37;	// ""
	}
	

	for ( int8_t mob=0; mob<14; mob++ )
	{
		CANPAGE = ( mob << 4 );  // Selects Message Object 0-14
		CANCDMOB = 0x00;    // Disable mob
		CANSTMOB = 0x00;    // Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));  // IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID)
{
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = CANMOB1
	
	CANIDT1 = NODE_ID >> 3; // Receive Address
	CANIDT2 = NODE_ID << 5; //
	CANIDT3 = 0x00; 		// ""
	CANIDT4 = 0x00; 		// ""
	
	CANIDM1 = 0b11111111;   // Receive Address
	CANIDM2 = 0b11100000;	//

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
}

//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Address, uint8_t DLC)
{
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;   // Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Address << 5;		//
	CANIDT1 = Address >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ )
	{
		CANMSG = transmit_data[i]; //CAN Data Message Register: setting the data in the message register
	}
	
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (DLC << DLC0) Set *DLC* Bytes in the data field of the message.

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) )
	{
		if(CANSTMOB & (1<<BERR)) PORTB++;
		//else PORTB = 0x00;
	}	// wait for TXOK flag set
	
	PORTB = 0x00;
	
	CANCDMOB = 0x00; //Clear CAN Mob Control and DLC Register

	CANSTMOB = 0x00; // Clear TXOK flag
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
}



#endif