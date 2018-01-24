/* DATA.C
This file contains more specific data sending functions, the variable getting from MObs, as well as the interrupt for the CAN rx.
*/

#include <string.h>			//Only for memcpy
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Defines.h"
#include "Data.h"
#include "CAN.h"
#include "Error.h"
#include "sd_raw.h"

extern volatile uint32_t rx_count;

void data_send_ecu(uint8_t node, uint8_t data)
{
	transmit_data[0] = node;
	transmit_data[1] = data;
	can_tx(ECU2ID, 2);
}

void data_send_ecu_a(uint8_t count, uint8_t ndarr[])
{
	memcpy(transmit_data, ndarr, count * 2);
	can_tx(ECU2ID, count * 2);
}

void data_send_motor(uint8_t header, uint8_t data, int32_t mul, uint16_t node)
{
	int32_t val = (mul * data) / 100;
	data_send16(header, (uint16_t)val, node);
}
void data_send_motor_d(uint8_t header, double data, int32_t mul, uint16_t node)
{
	int32_t val = (mul * data) / 100;
	data_send16(header, (uint16_t)val, node);
}


//GETTER
uint16_t g(uint8_t node, uint8_t val)
{
	selectmob(node);	
	return getonmob(val);
}

void selectmob(uint8_t node)
{
	if(node == NODEID1 || node == 0) CANPAGE = 4 << 4;
	else if(node == NODEID2 || node == 1) CANPAGE = 5 << 4;
	else if(node == NODEID3 || node == 2) CANPAGE = 6 << 4;
	else if(node == NODEID4 || node == 3) CANPAGE = 7 << 4;
	else if(node == ECU2ID || node == 4) CANPAGE = 8 << 4;
}

uint64_t getrawmob(uint8_t node)
{
	selectmob(node);
	CANPAGE &= 0b11111000;				//Set index to 0

	uint64_t ret = 0;
	
	waitonmob(num);
	
	//Read CANMSGs into ret
	for(uint8_t i = 0; i < 8; i++)
	{
		ret <<= 8;
		ret |= CANMSG;
	}
	
	return ret;
}

uint16_t getonmob(uint8_t num)
{
	CANPAGE &= 0b11111000;				//Set index to 0
	CANPAGE |= (num * 2) & 0b00000111;	//OR index with the value requested	
	waitonmob(num);
	uint8_t lo = CANMSG;
	uint8_t hi = CANMSG;
	return (hi<<8) + lo;
}

// Wait for MOb to be free
void waitonmob(uint8_t num)
{
	while(CANGSTA & (1<<RXBSY));
	//if(num < 8)	{ while ( CANEN2 & (1 << num)); }
	//else		{ while ( CANEN1 & (1 << (num - 8))); }
}





ISR(CANIT_vect)
{	
	//AMS
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 1 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0011 = MOB3
	if (CANSTMOB & ( 1 << RXOK))
	{
		rx_count++;
		
		//Should only go here if AMS has sent a message
		uint16_t rx_addr = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);
		
		uint8_t* receive_data;
		
		if(rx_addr == AMS_MSG_OVERALL)					receive_data = (uint8_t*)&amsd_overall;
		else if(rx_addr == AMS_MSG_DIAGNOSTIC)			receive_data = (uint8_t*)&amsd_diagnostic;
		else if(rx_addr == AMS_MSG_VOLTAGE)				receive_data = (uint8_t*)&amsd_voltage;
		else if(rx_addr == AMS_MSG_CELL_MODULE_TEMP)	receive_data = (uint8_t*)&amsd_cell_module_temp;
		else if(rx_addr == AMS_MSG_CELL_TEMP)			receive_data = (uint8_t*)&amsd_cell_temp;
		else if(rx_addr == AMS_MSG_CELL_BALANCING)		receive_data = (uint8_t*)&amsd_cell_balancing;
		else
		{
			//I know using goto is evil but it's not that bad here
			goto skip;
		}
		
		//AMS always sends 8 bytes
		receive_data[0] = CANMSG;
		receive_data[1] = CANMSG;
		receive_data[2] = CANMSG;
		receive_data[3] = CANMSG;
		receive_data[4] = CANMSG;
		receive_data[5] = CANMSG;
		receive_data[6] = CANMSG;
		receive_data[7] = CANMSG;
		
skip:
		CANSTMOB = 0x00; // Clear RXOK flag
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.
	}
	
	//CAN nodes
	for(uint8_t i = 0; i < 5; i++)
	{
		CANPAGE = (i + 4) << 4;
		if (CANSTMOB & (1 << RXOK))
		{
			//Clear RXOK flag and re-enable reception
			CANSTMOB = 0x00;
			CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0));
		}
	}
	
	//AMS and IMD interrupt
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0001 = MOB1
	if (CANSTMOB & ( 1 << RXOK))
	{
		//rx_count = TCNT0;
		rx_count++;

		uint8_t length = ( CANCDMOB & 0x0F );
		
		//for ( int8_t i = 0; i < length; i++ ) ReceiveData[i] = CANMSG;
		//Loop unrolling
		uint8_t receive_data[8];
		receive_data[0] = CANMSG;
		if(length > 1)
		{
			receive_data[1] = CANMSG;
			if(length > 2)
			{
				receive_data[2] = CANMSG;
				if(length > 3)
				{
					for (uint8_t i = 3; i < length; i++)
						receive_data[i] = CANMSG;
				}
			}
		}
		
		uint8_t i = 0;
		while(i < length)
		{
			switch(receive_data[i])
			{
				case AMSSHUTDOWN:
					ams_shutdown = _HIGH;
					i++;
					break;
			
				case IMDSHUTDOWN:
					imd_shutdown = _HIGH;
					i++;
					break;
						
				default:
					i++;
					break;
			}
		}
		CANSTMOB = 0x00; // Clear RXOK flag
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.
	}
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
}
