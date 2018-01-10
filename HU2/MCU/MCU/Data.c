/* DATA.C
This file contains more specific data sending functions, as well as the interrupt for the CAN rx.
*/

#include <string.h>			//Only for memcpy
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Defines.h"
#include "Data.h"
#include "CAN.h"
#include "Error.h"

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
					//Should only happen with multiple-request responses
					for (uint8_t i = 3; i < length; i++)
						receive_data[i] = CANMSG;
				}
			}
		}
		
		if(ui_current_screen == SCREEN_TEST)
		{
			if(receive_data[0] != SHUTDOWN) //Ignore shutdown message
			{
				if(length == 2)			test_value = ((uint32_t)receive_data[0] << 8) + (receive_data[1]);
				else if(length == 3)	test_value = ((uint32_t)receive_data[0] << 16) + ((uint32_t)receive_data[1] << 8) + (receive_data[2]);
				else if(length >= 4)	test_value = ((uint32_t)receive_data[0] << 24) + ((uint32_t)receive_data[1] << 16) + ((uint32_t)receive_data[2] << 8) + (receive_data[3]);
				else					test_value = (receive_data[0]);
			}
		}
		else
		{
			test_value = ((uint32_t)length << 24) + ((uint32_t)receive_data[0] << 16) + ((uint32_t)receive_data[1] << 8) + (receive_data[2]);
			
			uint8_t i = 0;
			while(i < length)
			{
				switch(receive_data[i])
				{
					case GAS_1:
						gas1 = (receive_data[i+1] +  (receive_data[i+2] << 8));
			
						//Bound checking while fixing range
						gas1perc = (gas1 < GAS1MIN) ? 0 : ((gas1 > GAS1MAX) ? (GAS1MAX - GAS1MIN) : (gas1 - GAS1MIN));
						gas1eng = (gas1perc * engine_max_perc) / (double)(GAS1MAX - GAS1MIN);
						gas1perc = (gas1perc * 100) / (GAS1MAX - GAS1MIN);
						i+=3;
						break;

					case GAS_2:
						gas2 = (receive_data[i+1] + (receive_data[i+2] << 8));
						gas2perc = (gas2 < GAS2MIN) ? 0 : ((gas2 > GAS2MAX) ? (GAS2MAX - GAS2MIN) : (gas2 - GAS2MIN));
						gas2perc = (gas2perc * 100) / (GAS2MAX - GAS2MIN);
						i+=3;
						break;

					case BRAKE:
						brake = (receive_data[i+1] + (receive_data[i+2] << 8));
						brakeperc = (brake < BRAKEMIN) ? 0 : ((brake > BRAKEMAX) ? (BRAKEMAX - BRAKEMIN) : (brake - BRAKEMIN));
						brakeperc = (brakeperc * 100) / (BRAKEMAX - BRAKEMIN);
						i+=3;
						break;
			
					case SHUTDOWN:
						//rx_count = TCNT0;
						shutdownon = receive_data[i+1] ? 1 : 0;
						i+=2;
						break;
				
					case RPM_FRONT_LEFT:
						rpm_fl = (uint16_t)(500000.d / (double)(receive_data[i+1] + (receive_data[i+2] << 8)));
						if(rpm_fl > 10000) rpm_fl = 0;
						i+=3;
						break;
				
					case RPM_FRONT_RIGHT:
						rpm_fr = (uint16_t)(500000.d / (double)(receive_data[i+1] + (receive_data[i+2] << 8)));
						if(rpm_fr > 10000) rpm_fr = 0;
						i+=3;
						break;
				
					case RPM_BACK_LEFT:
						rpm_bl = (receive_data[i+1] + (receive_data[i+2] << 8));
						i+=3;
						break;
				
					case RPM_BACK_RIGHT:
						rpm_br = (receive_data[i+1] + (receive_data[i+2] << 8));
						i+=3;
						break;
				
					case STEERING_POS:
						steerpos = (receive_data[i+1] + (receive_data[i+2] << 8)) - STEER_MIDDLE;
						i+=3;
						break;
				
					case FLOW_LEFT:
						flowleft = (receive_data[i+1] + (receive_data[i+2] << 8));
						if(flowleft == 0xFFFF)	flowleft = 0;
						else					flowleft = (uint16_t)(500000.d / (double)flowleft);
						i+=3;
						break;
				
					case FLOW_RIGHT:
						flowright = (receive_data[i+1] + (receive_data[i+2] << 8));
						if(flowright == 0xFFFF)	flowright = 0;
						else					flowright = (uint16_t)(500000.d / (double)flowright);
						i+=3;
						break;
						
					case TEMP_LEFT:
						templeft = (receive_data[i+1] + (receive_data[i+2] << 8));
						i+=3;
						break;
					
					case TEMP_RIGHT:
						tempright = (receive_data[i+1] + (receive_data[i+2] << 8));
						i+=3;
						break;
				
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
		}
		CANSTMOB = 0x00; // Clear RXOK flag
		CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 8 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.
	}
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
}
