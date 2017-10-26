#ifndef _CANc_
#define _CANc_

//***** Defines ***********************************************
#include "Defines.h"

//***** Libraries *********************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "CAN.h"
#include "ADC.h"
#include "ExternalInterrupt.h"

uint8_t ReceiveData[8];
uint8_t TransmitData[8];

//***** Reception ISR **********************************
ISR(CANIT_vect){  	// use interrupts
	int8_t length;
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0001 = MOB1

	length = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	for ( int8_t i = 0; i < length; i++ ){
		ReceiveData[i] = CANMSG; // Get data, INDX auto increments CANMSG
	}
	
	uint16_t ReceiveAddress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	cantimer = 0;
	
	if(ReceiveAddress == ECU2ID){ //Only receive if Address is NODEID5
		
		for(uint8_t i = 0; i < length; i++){
			if (ReceiveData[i] == 0x3D) { //if first data received is 3D = General data request
				i++;
				uint8_t j = 0;
				if (ReceiveData[i] == RPM_LINKS_ACHTER){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = ReceiveData[i];
					TransmitData[j++] = (PulsePerSec[_LEFT] >> 8);
					TransmitData[j++] = PulsePerSec[_LEFT];
				}
				if (ReceiveData[i] == RPM_RECHTS_ACHTER){ //if Receive data 0x01, Transmit the following data:
					TransmitData[j++] = ReceiveData[i];
					TransmitData[j++] = (PulsePerSec[_RIGHT] >> 8);
					TransmitData[j++] = PulsePerSec[_RIGHT];
				}
				if (ReceiveData[i] == DRAAIRICHTING_LINKS_ACHTER){
					TransmitData[j++] = ReceiveData[i];
					TransmitData[j++] = Direction[_LEFT];
				}
				if (ReceiveData[i] == DRAAIRICHTING_RECHTS_ACHTER){
					TransmitData[j++] = ReceiveData[i];
					TransmitData[j++] = Direction[_RIGHT];
				}
				if (ReceiveData[i] == SHUTDOWN){
					DDRD &= ~(1<<PD7);
					if((PIND & (1 << PD7))){
						TransmitData[j++] = ReceiveData[i];
						TransmitData[j++] = 0xFF;
					}
					else{
						TransmitData[j++] = ReceiveData[i];
						TransmitData[j++] = 0x00;
					}
				}
				can_tx(MASTERID, j); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == RUN_ENABLE){
				i++;
				DDRC	|= (1 << PC0);
				if(ReceiveData[i]){					
					PORTC	|= (1 << PC0);
				}
				else{
					PORTC	&= ~(1 << PC0);
				}
				_delay_ms(1);
				
				TransmitData[0] = RUN_ENABLE;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == MOTOR_CONTROLLER){
				i++;
				DDRC	|= (1 << PC1);
				if(ReceiveData[i]){
					PORTC	|= (1 << PC1);
				}
				else{
					PORTC	&= ~(1 << PC1);
				}
				_delay_ms(1);
				TransmitData[0] = MOTOR_CONTROLLER;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == BRAKELIGHT){
				i++;
				DDRC	|= (1 << PC4);
				if(ReceiveData[i]){
					PORTC	|= (1 << PC4);
				}
				else{
					PORTC	&= ~(1 << PC4);
				}
				_delay_ms(1);
				TransmitData[0] = BRAKELIGHT;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == PRE_DISCHARGE){
				i++;
				DDRC |= (1 << PC3);
				//DDRC |= (1 << PC2);
				// DDRD &= ~(1<<PD7);
				
				if(ReceiveData[i]){
					predison = 1;
					PORTC	|= (1 << PC3);
				}
				else{
					PORTC	&= ~(1 << PC3);
				}
				_delay_ms(1);
				TransmitData[0] = PRE_DISCHARGE;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == MAINRELAIS){
				i++;
				DDRC	|= (1 << PC2);
				if(ReceiveData[i++] && predison == 1){
					PORTC	|= (1 << PC2);
				}
				else{
					PORTC	&= ~(1 << PC2);
				}
				_delay_ms(1);
				TransmitData[0] = MAINRELAIS;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
			if (ReceiveData[i] == PUMP){
				i++;
				DDRC	|= (1 << PC5);
				if(ReceiveData[i]){
					PORTC	|= (1 << PC5);
				}
				else{
					PORTC	&= ~(1 << PC5);
				}
				_delay_ms(1);
				TransmitData[0] = PUMP;
				can_tx(MASTERID, 1); //Transmit data depending on the number of message received
			}
		}
	}

	for (int8_t i = 0; i < 8; i++){
		ReceiveData[i] = 0; //Resetting Receive Data
		TransmitData[i] = 0; //Resetting Transmit Data
	}


	CANSTMOB = 0x00; // Clear RXOK flag
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
	
}



//***** CAN ialization *****************************************************
void can_init(uint16_t Baud){
	
	if(FUNCTION == ECU2ID){
		
	}
	else{
		DDRD = 0x80;
		PORTD &= ~(1<<PORTD7); // Enable Can-chip
		//PORTD |= (1<<PORTD7); // Disable Can-chip
	}
	
	CANGCON = ( 1 << SWRES );   // Software reset
	
	CANTCON = 0x00;       // CAN timing prescaler set to 0;
	
	if (Baud == 1000){
		CANBT1 = 0x00;    // Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;    // "
		CANBT3 = 0x36; 	  // ""
	}
	if (Baud == 500){
		CANBT1 = 0x02;    // Set baud rate to 500kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;    // "
		CANBT3 = 0x37; 	  // ""
	}
	if (Baud == 250){
		CANBT1 = 0x0E;   // Set baud rate to 250kb (assuming 16Mhz IOclk)
		CANBT2 = 0x04;   // "
		CANBT3 = 0x13; 	 // ""
	}
	if ( Baud == 125){
		CANBT1 = 0x06;  // Set baud rate to 125kb (assuming 16Mhz IOclk)
		CANBT2 = 0x0C;  // "
		CANBT3 = 0x37;	// ""
	}
	

	for ( int8_t mob=0; mob<14; mob++ ) {

		CANPAGE = ( mob << 4 );  // Selects Message Object 0-14
		CANCDMOB = 0x00;    // Disable mob
		CANSTMOB = 0x00;    // Clear mob status register;
	}
	
	CANIE2 = (( 1 << IEMOB1 ) | ( 0 << IEMOB0 ));  // IEMOB1 = MOB1 Enable interrupts on mob1 for reception and transmission
	CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 0 << ENTX );   // Enable interrupts on receive
	CANGCON |= ( 1 << 1 );	// Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
}

//***** CAN Creating RX *****************************************************
void can_rx(uint16_t NODE_ID){
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select 0001 = CANMOB1
	
	CANIDT1 = NODE_ID >> 3; // Receive Address
	CANIDT2 = NODE_ID << 5; //
	CANIDT3 = 0x00; 		// ""
	CANIDT4 = 0x00; 		// ""
	
	CANIDM1 = 0b11111111;   // Receive Address
	CANIDM2 = 0b11100000;	//

	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0));  // Enable Reception | 11 bit | IDE DLC8
}

//***** CAN Creating TX *****************************************************
void can_tx(uint16_t Address, uint8_t DLC) {
	
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0

	while ( CANEN2 & ( 1 << ENMOB0 ) ); // Wait for MOb 0 to be free
	
	CANSTMOB = 0x00;   // Clear mob status register
	
	CANIDT4 = 0x00;     		//
	CANIDT3 = 0x00;				//
	CANIDT2 = Address << 5;		//
	CANIDT1 = Address >> 3;		//
	
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