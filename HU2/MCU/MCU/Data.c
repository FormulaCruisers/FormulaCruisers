#include <string.h>			//Only for memcpy
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Defines.h"
#include "Data.h"
#include "CAN.h"
#include "Error.h"

volatile uint16_t waiting = 1;

void wait_for_rx()
{
	waiting = 1;
	while(waiting > 1 && waiting < RX_WAIT_LIMIT) waiting++;
}

void data_send_ecu(uint8_t node, uint8_t data)
{
	TransmitData[0] = node;
	TransmitData[1] = data;
	can_tx(ECU2ID, 2);
	
	wait_for_rx();
}

void data_send_ecu_a(uint8_t count, uint8_t ndarr[])
{
	memcpy(TransmitData, ndarr, count * 2);
	can_tx(ECU2ID, count * 2);
	
	wait_for_rx();
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
	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 1 << MOBNB0 ); // select CANMOB 0001 = MOB1

	uint16_t ReceiveAddress = (CANIDT1 << 3) | ((CANIDT2 & 0b11100000) >> 5);

	if(ReceiveAddress == MASTERID)
	{
		waiting = 0;
		
		uint8_t length = ( CANCDMOB & 0x0F );
		
		//for ( int8_t i = 0; i < length; i++ ) ReceiveData[i] = CANMSG;
		//Loop unrolling
		uint8_t ReceiveData[8];
		ReceiveData[0] = CANMSG;
		if(length > 1)
		{
			ReceiveData[1] = CANMSG;
			if(length > 2)
			{
				ReceiveData[2] = CANMSG;
				if(length > 3)
				{
					//Should only happen with multiple-request responses
					for (uint8_t i = 3; i < length; i++)
						ReceiveData[i] = CANMSG;
				}
			}
		}
		
		if(ui_current_screen == SCREEN_TEST)
		{
			if(length == 2)			test_value = ((uint32_t)ReceiveData[0] << 8) + (ReceiveData[1]);
			else if(length == 3)	test_value = ((uint32_t)ReceiveData[0] << 16) + ((uint32_t)ReceiveData[1] << 8) + (ReceiveData[2]);
			else if(length >= 4)	test_value = ((uint32_t)ReceiveData[0] << 24) + ((uint32_t)ReceiveData[1] << 16) + ((uint32_t)ReceiveData[2] << 8) + (ReceiveData[3]);
			else					test_value = (ReceiveData[0]);
		}
		else
		{
			uint8_t i = 0;
			while(i < length)
			{
				switch(ReceiveData[i])
				{
					case GAS_1:
						gas1 = (ReceiveData[i+1] +  (ReceiveData[i+2] << 8));
			
						//Bound checking while fixing range
						gas1perc = (gas1 < GAS1MIN) ? 0 : ((gas1 > GAS1MAX) ? (GAS1MAX - GAS1MIN) : (gas1 - GAS1MIN));
						gas1eng = (gas1perc * engine_max_perc) / (double)(GAS1MAX - GAS1MIN);
						gas1perc = (gas1perc * 100) / (GAS1MAX - GAS1MIN);
						i+=3;
						break;

					case GAS_2:
						gas2 = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						gas2perc = (gas2 < GAS2MIN) ? 0 : ((gas2 > GAS2MAX) ? (GAS2MAX - GAS2MIN) : (gas2 - GAS2MIN));
						gas2perc = (gas2perc * 100) / (GAS2MAX - GAS2MIN);
						i+=3;
						break;

					case BRAKE:
						brake = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						brakeperc = (brake < BRAKEMIN) ? 0 : ((brake > BRAKEMAX) ? (BRAKEMAX - BRAKEMIN) : (brake - BRAKEMIN));
						brakeperc = (brakeperc * 100) / (BRAKEMAX - BRAKEMIN);
						i+=3;
						break;
			
					case SHUTDOWN:
						shutdownon = ReceiveData[i+1] ? 1 : 0;
						i+=2;
						break;
				
					case RPM_FRONT_LEFT:
						rpm_fl = (uint16_t)(250000.d / (double)(ReceiveData[i+1] + (ReceiveData[i+2] << 8)));
						if(rpm_fl > 10000) rpm_fl = 0;
						i+=3;
						break;
				
					case RPM_FRONT_RIGHT:
						rpm_fr = (uint16_t)(250000.d / (double)(ReceiveData[i+1] + (ReceiveData[i+2] << 8)));
						if(rpm_fr > 10000) rpm_fr = 0;
						i+=3;
						break;
				
					case RPM_BACK_LEFT:
						rpm_bl = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						i+=3;
						break;
				
					case RPM_BACK_RIGHT:
						rpm_br = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						i+=3;
						break;
				
					case STEERING_POS:
						steerpos = (ReceiveData[i+1] + (ReceiveData[i+2] << 8)) - STEER_MIDDLE;
						i+=3;
						break;
				
					case FLOW_LEFT:
						flowleft = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						i+=3;
						break;
				
					case FLOW_RIGHT:
						test_value = ((uint32_t)length << 24) + ((uint32_t)ReceiveData[3] << 16) + ((uint32_t)ReceiveData[4] << 8) + (ReceiveData[5]);
						flowright = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						i+=3;
						break;
						
					case TEMP_LEFT:
						templeft = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
						i+=3;
						break;
					
					case TEMP_RIGHT:
						test_value = ((uint32_t)length << 24) + ((uint32_t)ReceiveData[3] << 16) + ((uint32_t)ReceiveData[4] << 8) + (ReceiveData[5]);
						tempright = (ReceiveData[i+1] + (ReceiveData[i+2] << 8));
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
	}
	
	CANSTMOB = 0x00; // Clear RXOK flag
	CANCDMOB = (( 1 << CONMOB1 ) | ( 0 << IDE ) | ( 3 << DLC0)); //CAN MOb Control and DLC Register: (1<<CONMOB1) = enable reception. (0<<IDE) = can standard rev 2.0A ( id length = 11 bits), (3 << DLC0) 3 Bytes in the data field of the message.

	CANPAGE = ( 0 << MOBNB3 ) | ( 0 << MOBNB2 ) | ( 0 << MOBNB1 ) | ( 0 << MOBNB0 ); // select 0000 = CANMOB0
}
