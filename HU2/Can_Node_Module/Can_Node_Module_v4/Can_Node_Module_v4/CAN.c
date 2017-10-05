#ifndef _CANc_
#define _CANc_

#include "Defines.h"
#include "CAN.h"
#include "USART.h"
#include "ADC.h"

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t ReceiveData[8];
uint8_t TransmitData[8];

//***** Reception ISR **********************************
ISR(CANIT_vect){  					// use interrupts
	PORTE &= ~(1 << PE4);

	int8_t length;
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0001 = MOB1

	length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	for ( int8_t i = 0; i < length; i++ ){
		ReceiveData[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
		//USARTWriteChar(ReceiveData[i]);
	}
	
	uint16_t ReceiveAdress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	//USARTWriteChar((ReceiveAdress & 0xFF00) >> 8);
	//USARTWriteChar(ReceiveAdress & 0x00FF);
	
	if(ReceiveAdress == MASTERID){ //Only receive if adress is MASTERID
		
		for (uint8_t i = 0; i < length; i = i ){
			if ((ReceiveData[i] & 0b00111111) == 0x01){
				USARTWriteChar(ReceiveData[i] & 0b00111111);
				USARTWriteChar((ReceiveData[i] & 0b11000000) >> 6);
				USARTWriteChar(ReceiveData[i + 1]);
				i += 2;
			}
			if ((ReceiveData[i] & 0b00111111) == 0x02){
				USARTWriteChar(ReceiveData[i] & 0b00111111);
				USARTWriteChar((ReceiveData[i] & 0b11000000) >> 6);
				USARTWriteChar(ReceiveData[i + 1]);
				i += 2;
			}
			if ((ReceiveData[i] & 0b00111111) == 0x03){
				USARTWriteChar(ReceiveData[i] & 0b00111111);
				USARTWriteChar((ReceiveData[i] & 0b11000000) >> 6);
				USARTWriteChar(ReceiveData[i + 1]);
				i += 2;
			}
			if ((ReceiveData[i] & 0b00111111) == 0x04){
				USARTWriteChar(ReceiveData[i] & 0b00111111);
				USARTWriteChar((ReceiveData[i] & 0b11000000) >> 6);
				USARTWriteChar(ReceiveData[i + 1]);
				i += 2;
			}
		}
	}
	
	
	if(ReceiveAdress == NODEID1){ //Only receive if adress is NODEID1
		if (ReceiveData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			uint8_t j = 0;
			for(uint8_t i = 1; i < length; i++){
				if (ReceiveData[i] == 0x01){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = 0x01 | ((ADCValue[0] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[0];
				}
				if (ReceiveData[i] == 0x02){ 
					TransmitData[j++] = 0x02 | ((ADCValue[1] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[1];
				}
				if (ReceiveData[i] == 0x03){
					TransmitData[j++] = 0x03 | ((ADCValue[2] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[2];
				}
				if (ReceiveData[i] == 0x04){
					TransmitData[j++] = 0x04 | ((ADCValue[3] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[3];
				}
			}
			can_tx(MASTERID, j); //Transmit data depending on the number of message received
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	

	if(ReceiveAdress == NODEID2){ //Only receive if adress is NODEID2
		if (ReceiveData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			uint8_t j = 0;
			for(uint8_t i = 1; i < length; i++){
				if (ReceiveData[i] == 0x01){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = 0x01 | ((ADCValue[0] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[0];
				}
				if (ReceiveData[i] == 0x02){
					TransmitData[j++] = 0x02 | ((ADCValue[1] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[1];
				}
				if (ReceiveData[i] == 0x03){
					TransmitData[j++] = 0x03 | ((ADCValue[2] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[2];
				}
				if (ReceiveData[i] == 0x04){
					TransmitData[j++] = 0x04 | ((ADCValue[3] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[3];
				}
			}
			can_tx(MASTERID, j); //Transmit data depending on the number of message received
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	
	if(ReceiveAdress == NODEID3){ //Only receive if adress is NODEID3
		if (ReceiveData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			uint8_t j = 0;
			for(uint8_t i = 1; i < length; i++){
				if (ReceiveData[i] == 0x01){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = 0x01 | ((ADCValue[0] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[0];
				}
				if (ReceiveData[i] == 0x02){
					TransmitData[j++] = 0x02 | ((ADCValue[1] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[1];
				}
				if (ReceiveData[i] == 0x03){
					TransmitData[j++] = 0x03 | ((ADCValue[2] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[2];
				}
				if (ReceiveData[i] == 0x04){
					TransmitData[j++] = 0x04 | ((ADCValue[3] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[3];
				}
			}
			can_tx(MASTERID, j); //Transmit data depending on the number of message received
		}
		else{
			PORTE |= (1<<PE4);
		}
	}
	
	if(ReceiveAdress == NODEID4){ //Only receive if adress is NODEID4
		if (ReceiveData[0] == 0x3D) {
			PORTE &= ~(1<<PE4);
			uint8_t j = 0;
			for(uint8_t i = 1; i < length; i++){
				if (ReceiveData[i] == 0x01){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = 0x01 | ((ADCValue[0] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[0];
				}
				if (ReceiveData[i] == 0x02){
					TransmitData[j++] = 0x02 | ((ADCValue[1] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[1];
				}
				if (ReceiveData[i] == 0x03){
					TransmitData[j++] = 0x03 | ((ADCValue[2] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[2];
				}
				if (ReceiveData[i] == 0x04){
					TransmitData[j++] = 0x04 | ((ADCValue[3] >> 2) & 0b11000000 ) ;
					TransmitData[j++]		= ADCValue[3];
				}
			}
			can_tx(MASTERID, j); //Transmit data depending on the number of message received
		}
		else{
			PORTE |= (1<<PE4);
		}
	}

	for (int8_t i = 0; i < 8; i++){
		ReceiveData[i] = 0;
		TransmitData[i] = 0;
	}


	CANSTMOB = 0x00; // Clear TXOK flag
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0

	USARTWriteChar(0xDD);
	
	PORTE |= (1 << PE4);
}



//***** CAN ialization *****************************************************
void can_init(uint16_t Baud){

	CANGCON = ( 1 << SWRES );   // Software reset
	
	CANTCON = 0x00;         // CAN timing prescaler set to 0;
	
	if (Baud == 500){
		CANBT1 = 0x02;      	// Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;          // "
		CANBT3 = 0x37; 			// ""
	}
	if (Baud == 250){
		CANBT1 = 0x0E;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x04;          // "
		CANBT3 = 0x13; 			// ""
	}
	if ( Baud == 125){
		CANBT1 = 0x06;      	// Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;          // "
		CANBT3 = 0x37; 			// ""
	}
	

	for ( int8_t mob=0; mob<14; mob++ ) {

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-14
		CANCDMOB = 0x00;       		// Disable mob
		CANSTMOB = 0x00;     		// Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));   	// IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );		// ENASTB = 1	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID){
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
	
	CANIDT1 = NODE_ID >> 3;   	// Receive adress
	CANIDT2 = NODE_ID << 5;		//
	CANIDT3 = 0x00; 			// ""
	CANIDT4 = 0x00; 			// ""
	
	CANIDM1 = 0b11111111;   // Receive adress
	CANIDM2 = 0b11100000;	//

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8

}


//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Adress, uint8_t DLC) {
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = MOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;    		// Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Adress << 5;		//
	CANIDT1 = Adress >> 3;		//
	
	for ( int8_t i = 0; i < 8; i++ ){
		CANMSG = TransmitData[i]; //CAN Data Message Register: setting the data in the message register
	}
	
	CANCDMOB = (( 1 << CONMOB0 ) | ( 0 << IDE ) | ( DLC << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (DLC << DLC0) Set *DLC* Bytes in the data field of the message.

	while ( ! ( CANSTMOB & ( 1 << TXOK ) ) );	// wait for TXOK flag set
	
	CANCDMOB = 0x00; //Clear CAN Mob Control and DLC Register

	CANSTMOB = 0x00; // Clear TXOK flag
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = MOB1
}

#endif