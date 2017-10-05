
#include "Defines.h"
#include "USART.h"
#include "CAN.h"
#include "ADC.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>		// include delays for _delay_ms();

void chip_init (void);

int main( void )
{
	USARTInit(51);	// UBRR = 51  19200 Baud
	chip_init();	// Chip initialization
	can_init(250); 	// Can initialization
	int_ADC();
	sei();
	
	
	uint16_t toggle = 0;
	
	can_rx(FUNCTION);
	if(FUNCTION == MASTERID){
		while(1){
			if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
				if(toggle == 0){
					
					TransmitData[0] = 0x3D;
					TransmitData[1] = 0x03;
					TransmitData[2] = 0x01;
					TransmitData[3] = 0x02;
					can_tx(NODEID1, 4);
				}
				toggle = 1;
			}
			else{							// Else if not button "But" is pressed(High) then:
				if(toggle == 1){
					TransmitData[0] = 0x00;
					TransmitData[1] = 0x01;
					TransmitData[2] = 0x02;
					TransmitData[3] = 0x03;
					can_tx(NODEID1, 4);
				}
				toggle = 0;
			}
		}
	}
	
	else{
		while(1){
			
			if ((PINE & (1<<PE5)) == 0){	// If button "But" is pressed (Grounded) then:
				if(toggle == 0){
					
					TransmitData[0] = 0x3D;
					TransmitData[1] = 0x01;
					TransmitData[2] = 0x02;
					can_tx(MASTERID, 3);
					getADC(0);
				}
				toggle = 1;
			}
			else{							// Else if not button "But" is pressed(High) then:
				if(toggle == 1){
					TransmitData[0] = 0x00;
					TransmitData[1] = 0x03;
					TransmitData[2] = 0x04;
					can_tx(MASTERID, 3);
				}
				toggle = 0;
			}
			
		}
	}
	
	return(0);
}


//***** chip Initialization **********************************
void chip_init(void){
	
	
	CLKPR = ( 1 << CLKPCE );  		// Set Clock Prescaler change enable
	CLKPR = 0x00;				// no prescaler CLK 16Mhz
	
	DDRE = 0x10;	// stat led output
	PORTE |= (1<<PE4);
}
