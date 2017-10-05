#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Defines.h"
#include "CAN.h"
#include "Data.h"

ISR(TIMER0_OVF_vect)
{
	TCNT0 = _TM0;
}

ISR(TIMER2_OVF_vect) //8 Hz
{
	TCNT2 = _TM2;
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