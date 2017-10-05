#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Defines.h"
#include "CAN.h"
#include "Data.h"
#include "lcd.h"

uint16_t gas1val = 0;
char buffer[4][21];

ISR(TIMER0_OVF_vect)
{
	
}

ISR(TIMER2_OVF_vect) //8 Hz
{
	TCNT2 = _TM2;
	
	//test stuff
	data_request(NODEID2, GAS_1, &gas1val, TRUE);
	
	//test print to lcd
	snprintf(buffer[0], sizeof buffer[0], "gas:%5d           ", gas1val);
	snprintf(buffer[1], sizeof buffer[1], "                    ");
	snprintf(buffer[2], sizeof buffer[2], "                    ");
	snprintf(buffer[3], sizeof buffer[3], "                    ");
	for(uint8_t y = 0; y < 4; y++)
	{
		lcd_gotoxy(0, y);
		lcd_puts(buffer[y]);
	}
}

int main()
{
	//Initialize timer0
	TCCR0A |= (1 << CS02);					//256 Prescaler
	TCNT0 = _TM0;							//Set initial counter value
	TIMSK0 |= (1 << TOIE0);					//Overflow Interrupt Enable
	
	//Initialize timer2
	ASSR  = (1<< AS2);						//Enable asynchronous mode
	TCNT2 = _TM2;							//Set initial counter value
	TCCR2A |= (1 << CS01)|(1 << CS00);		//128 Prescaler
	TIMSK2  = (1 << TOIE2);					//Overflow interrupt enable
	
	can_init();
	can_rx(MASTERID);
	
	lcd_init(LCD_DISP_ON);
	
	//Set CPU into sleep mode(simultaneously enabling interrupts)
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();
	
	while(1);
}