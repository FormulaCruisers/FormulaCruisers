#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Data.h"
#include "CAN.h"
#include "Defines.h"
#include "Error.h"

volatile uint16_t* outvars[0xFF];
volatile uint8_t waitingon = 0;

void log_data(uint8_t node, uint16_t val)
{
	char dstr[16];
	snprintf(dstr, sizeof(dstr), "%x: %5d", node, val);
	log_println(dstr);
}

void log_print(char* str)
{
	//TODO: log data
}

void log_println(char* str)
{
	//TODO: log data
}
	
void data_request(uint16_t node, uint8_t num, uint16_t* outputvar, uint8_t wait)
{
	//If there's still a request on this node, wait for it
	uint8_t timeout = DATA_WAIT_TIMEOUT;
	while(outvars[num] != 0 && timeout-- > 1) ;
	
	//If there was a timeout, throw an error
	if(timeout <= 1) e_throw("Data wait timed out");
	
	//Add to the list
	waitingon++;
	outvars[num] = outputvar;
	data_send8(CAN_REQUEST_DATA, num, node);
	if(wait)
	{
		while(outvars[num] != 0) ;
	}
}

void data_waitforall()
{
	//Wait for the amount of current requests to become 0 (or timeout)
	uint8_t timeout = DATA_WAIT_TIMEOUT;
	while(waitingon > 0 && timeout-- > 1) ;
	
	//Throw error if timed out
	if(timeout <= 1) e_throw("Data wait timeout");
}

void data_send_ecu(uint8_t node, uint8_t data, uint8_t wait)
{	
	//If there's still a request on this node, wait for it
	uint8_t timeout = DATA_WAIT_TIMEOUT;
	while(outvars[node] != 0 && timeout-- > 1) ;
	
	//If there was a timeout, throw an error
	if(timeout <= 1) e_throw("Data waitall timeout");
	
	//Add to the list
	waitingon++;
	outvars[node] = (uint16_t*)-1;
	
	//Send two bytes in the form of {node, data} to the ECU
	TransmitData[0] = node;
	TransmitData[1] = data;
	can_tx(ECU2ID, 2);
	
	if(wait)
	{
		while(outvars[node] != 0) ;
	}
}

ISR(CANIT_vect)
{
	//Some necessary CAN setup stuff
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0001 = MOB1

	//Get message length and write message in ReceiveData
	uint8_t length = ( CANCDMOB & 0x0F );
	for ( int8_t i = 0; i < length; i++ ) ReceiveData[i] = CANMSG;
	
	uint16_t ReceiveAddress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
	
	if(ReceiveAddress == MASTERID)
	{
		//Get the output variable to write to
		volatile uint16_t *ovar = outvars[ReceiveData[0]];
		
		if(ovar != 0)
		{
			if(length == 1)			//ECU command returns
				{/*do nothing*/}
			else if(length == 2)	//ECU data request returns
				*(ovar) = ReceiveData[1];
			else if(length == 3)	//General data request returns
				*(ovar) = (ReceiveData[1] +  (ReceiveData[2] << 8));
			else
				e_throw("Unknown rx length");
				
			////This creates 600kb of data per minute per sensor if read out once every ms
			//log_data(ReceiveData[0], *(ovar));
			
			ovar = 0;
			
			if(waitingon > 0)
				waitingon--;
			else
				e_throw("More rx than tx");
		}
		else
		{
			e_throw("Got unexpected rx");
		}
	}
	
	CANSTMOB = 0x00; // Clear RXOK flag
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
}
