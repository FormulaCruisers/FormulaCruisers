/* CAN.C
This file contains several basic functions to transmit and receive data via CAN bus.
The CAN rx interrupt is enabled here, but not handled in this file. (See Data.c)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "CAN.h"

volatile uint32_t tx_count = 0;
volatile uint32_t rx_count = 0;

void data_send_arr(uint8_t header, uint8_t buffer[], uint16_t node, uint8_t bufferlen)
{
	transmit_data[0] = header;
	for(uint8_t i = 0; i < bufferlen; i++) transmit_data[i+1] = buffer[i];
	can_tx(node, bufferlen + 1);
}
void data_send8(uint8_t header, uint8_t data, uint16_t node)
{
	uint8_t d[] = {data};
	data_send_arr(header, d, node, 1);
}
void data_send16(uint8_t header, uint16_t data, uint16_t node)
{
	uint8_t d[] = {data & 255, /*<-- Low byte | High byte -->*/ (data >> 8) & 255};
	data_send_arr(header, d, node, 2);
}

void data_send_arr_nh(uint8_t buffer[], uint16_t node, uint8_t bufferlen)
{
	for(uint8_t i = 0; i < bufferlen; i++) transmit_data[i] = buffer[i];
	can_tx(node, bufferlen);
}
void data_send8_nh(uint8_t data, uint16_t node)
{
	uint8_t d[] = {data};
	data_send_arr_nh(d, node, 1);
}
void data_send16_nh(uint16_t data, uint16_t node)
{
	uint8_t d[] = {data & 255, /*<-- Low byte | High byte -->*/ (data >> 8) & 255};
	data_send_arr_nh(d, node, 2);
}
void data_send0(uint16_t addr)
{
	can_tx(addr, 0);
}

//CAN MOb setup:
// MOb 0 = Transmission MOb only
// MOb 1 = Interrupt on reception, set to address MASTERID only. Unused
// MOb 2 = Unused
// MOb 3 = Interrupt on reception, set to receive most AMS messages.
// MOb 4 = Receives data only on address NODEID1
// MOb 5 = Receives data only on address NODEID2
// MOb 6 = Receives data only on address NODEID3
// MOb 7 = Receives data only on address NODEID4
// MOb 8 = Receives data only on address ECU2ID
// MOb 9-14 = Unused


//***** CAN initalization *****************************************************
void can_init()
{	
	CANGCON = ( 1 << SWRES );   // Software reset
	
	CANTCON = 0x00;				// CAN timing prescaler set to 0;
	
	CANBT1 = 0x02;	// Set baud rate to 500kb (assuming 16Mhz IOclk)
	CANBT2 = 0x0C;
	CANBT3 = 0x37;

	for (int8_t mob=0; mob<14; mob++)
	{
		CANPAGE = ( mob << 4 ); // Selects Message Object 0-14
		CANCDMOB = 0x00;		// Disable mob
		CANSTMOB = 0x00;		// Clear mob status register;
	}
	
	CANIE2 = ((1 << IEMOB3) | ( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));  // Enable interrupts on MOb3 and MOb1
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
	
	//AMS messages on CAN page 3
	CANPAGE = (3 << 4);
	
	CANIDT1 = AMS_BASE >> 3;
	CANIDT2 = (AMS_BASE << 5) & 0xFF;
	CANIDT3 = 0;
	CANIDT4 = 0;
	
	CANIDM1 = 0b11111110; // Set mask to allow certain bits to be different (lowest 4 bits)
	CANIDM2 = 0b00000000;
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
	
	//Give each node its own MOb
	uint16_t NODES[5] = {NODEID1, NODEID2, NODEID3, NODEID4, ECU2ID};
	for(uint8_t i = 0; i < 5; i++)
	{
		CANPAGE = (i + 4) << 4;
		CANIDT1 = NODES[i] >> 3;
		CANIDT2 = (NODES[i] << 5) & 0xFF;
		CANIDT3 = 0;
		CANIDT4 = 0;
		CANIDM1 = 0b11111111;
		CANIDM2 = 0b11100000;
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
	}
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID)
{
	CANPAGE = ( 1 << MOBNB0 ); // Select message object 1
	
	CANIDT1 = NODE_ID >> 3; // Receive Address
	CANIDT2 = NODE_ID << 5; //
	CANIDT3 = 0x00; 		// ""
	CANIDT4 = 0x00; 		// ""
	
	CANIDM1 = 0b11111111;   // Receive Address mask
	CANIDM2 = 0b11100000;	//

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
}

//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Address, uint8_t DLC)
{
	tx_count++;
	
	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
	
	CANSTMOB = 0x00;   // Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Address << 5;		//
	CANIDT1 = Address >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ ) CANMSG = transmit_data[i]; //CAN Data Message Register: setting the data in the message register
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB0) = enable transmission. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (DLC << DLC0) Set *DLC* Bytes in the data field of the message.

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set	// JEROEN PAS NOU OP JONGE
	
	CANCDMOB = 0x00; //Clear CAN Mob Control and DLC Register

	CANSTMOB = 0x00; // Clear TXOK flag
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
}