#include "Defines.h"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>

#include "lcd.h"
#include "CAN.h"
#include "UI.h"
#include "Data.h"
#include "Error.h"

volatile uint16_t gas1 = 0;
volatile uint16_t gas2 = 0;
volatile uint16_t brake = 0;
volatile uint16_t gas1perc = 0;
volatile uint16_t gas2perc = 0;
volatile uint16_t brakeperc = 0;
volatile int32_t gas1eng = 0;

volatile uint8_t shutdownon = 0;

volatile uint16_t rpm_fl = 0;
volatile uint16_t rpm_fr = 0;
volatile uint16_t rpm_bl = 0;
volatile uint16_t rpm_br = 0;
volatile uint16_t steerpos = 0;

volatile uint16_t flowleft = 0;
volatile uint16_t flowright = 0;

uint16_t readybeep = 0;

volatile uint16_t predistimer = PREDISCHARGE_TIMER;

uint8_t btnblue = 0, btngreen = 0, btn1 = 0, btn2 = 0;

volatile enum uiscreen ui_current_screen = SCREEN_WELCOME;
volatile enum _error _errorcode = ERROR_NONE;

volatile uint8_t ams_shutdown = _LOW;
volatile uint8_t imd_shutdown = _LOW;

void debounce(uint8_t* btn, uint8_t val);

uint8_t ttt = 0; //Counter to make sure each node only gets one request at a time

ISR(TIMER0_OVF_vect)
{
	data_send8(CAN_REQUEST_DATA, SHUTDOWN, ECU2ID);
	
	/*
	if(!shutdownon || ams_shutdown || imd_shutdown)
	{
		if(ui_current_screen == SCREEN_PREDISCHARGING || ui_current_screen == SCREEN_DRIVING || ui_current_screen == SCREEN_STATUS)
		{
			_errorcode = ERROR_SHUTDOWN;
		}
	}//*/
	
	debounce(&btnblue, PIND & (1<<BUTTONBLUE));
	debounce(&btngreen, PIND & (1<<BUTTONGREEN));
	debounce(&btn1, PIND & (1<<BUTTON1)); //The button that is above the green button(doesn't work right now?)
	debounce(&btn2, PIND & (1<<BUTTON2)); //The button that is above the blue button
	
	DDRE |= 0b11111111;
	
	if(btngreen || btnblue || btn1 || btn2)
	{
		PORTE |= 0b11111111;
	}
	else
	{
		PORTE &= ~(0b11111111);
	}
	
	//Request gas/brake values
	switch(ttt)
	{
		case 0:
			data_send8(CAN_REQUEST_DATA, RPM_FRONT_LEFT, NODEID1);
			data_send8(CAN_REQUEST_DATA, GAS_1, NODEID2);
			data_send8(CAN_REQUEST_DATA, RPM_BACK_LEFT, ECU2ID);
			break;
		case 1:
			data_send8(CAN_REQUEST_DATA, RPM_FRONT_RIGHT, NODEID1);
			data_send8(CAN_REQUEST_DATA, GAS_2, NODEID2);
			data_send8(CAN_REQUEST_DATA, RPM_BACK_RIGHT, ECU2ID);
			break;
		case 2:
			data_send8(CAN_REQUEST_DATA, STEERING_POS, NODEID1);
			data_send8(CAN_REQUEST_DATA, BRAKE, NODEID2);
			break;
	}
	
	if(_errorcode != ERROR_NONE)
	{
		//Reset literally everything possible
		data_send_ecu(MOTOR_CONTROLLER, _LOW);
		data_send_ecu(RUN_ENABLE, _LOW);
		data_send_ecu(PREDISCHARGE, _LOW);
		data_send_ecu(MAIN_RELAIS, _LOW);
		data_send_ecu(PUMP_ENABLE, _LOW);
		if(ui_current_screen == SCREEN_DRIVING)
		{
			data_send16(CAN_SEND_DATA, 0, MCDR);
			data_send16(CAN_SEND_DATA, 0, MCDL);
		}
		
		//Change into error screen
		change_screen(SCREEN_ERROR);
	}
	
	switch(ui_current_screen)
	{
		case SCREEN_WELCOME:
			if(btnblue == 1)
			{
				data_send_ecu(MOTOR_CONTROLLER, _HIGH);
				if(_errorcode == ERROR_NONE) change_screen(SCREEN_START);
			}
			break;
		
		case SCREEN_START:
			if(btnblue == 1)
			{
				e_checksensors();
				e_checkranges();
			
				if(_errorcode == ERROR_NONE)
				{
					data_send_ecu(PREDISCHARGE, _HIGH);
					change_screen(SCREEN_PREDISCHARGING);
				
					readybeep = RTDS_TIME;
					// TODO: Uncomment when beep should be implemented
					//PORTC |= 1 << RTDS
				
					data_send_ecu(PUMP_ENABLE, _HIGH);
				}
			}
			break;
		
		case SCREEN_PREDISCHARGING:
			if(predistimer-- == 0)
			{
				data_send_ecu(MAIN_RELAIS, _HIGH);
				change_screen(SCREEN_STATUS);
			}
			else if(predistimer == PREDISCHARGE_TIMER - 1900)
			{
				data_send8(CAN_REQUEST_DATA, FLOW_LEFT, NODEID3);
				data_send8(CAN_REQUEST_DATA, FLOW_RIGHT, NODEID4);
			}
			else if(predistimer == PREDISCHARGE_TIMER - 2000)
			{
				//e_checkflow();
			}
			break;
			
		case SCREEN_STATUS:
			if(btngreen == 1)
			{
				data_send_ecu(RUN_ENABLE, _HIGH);
				change_screen(SCREEN_DRIVING);
			}
			break;
		
		case SCREEN_DRIVING:
			if(ttt == 1)
			{
				data_send8(CAN_REQUEST_DATA, FLOW_LEFT, NODEID3);
				data_send8(CAN_REQUEST_DATA, FLOW_RIGHT, NODEID4);
			}
			if(ttt == 3)
			{
				//e_checkflow();
				e_checksensors();
				e_checkranges();
				e_checkdiscrepancy();	
			}
			
			//*
			if(_errorcode == ERROR_NONE)
			{
				//uint8_t wheel_diff = steerpos - STEER_MIDDLE + 100;
				//data_send16(CAN_SEND_DATA, (uint16_t)((gas1eng * wheel_diff) / 100), MCDR);
				//data_send16(CAN_SEND_DATA, (uint16_t)((gas1eng * 100) / wheel_diff), MCDL);	
				data_send16(CAN_SEND_DATA, (int16_t)gas1eng, MCDR);
				data_send16(CAN_SEND_DATA, (int16_t)gas1eng, MCDL);
			}//*/
			break;
			
		case SCREEN_ERROR:
			if(btngreen == 1)
			{
				ams_shutdown = 0;
				imd_shutdown = 0;
				_errorcode = ERROR_NONE;
				change_screen(SCREEN_WELCOME);
			}
			break;
		default:
			break;
	}
	
	ttt = (ttt + 1) % 3;
	
	if(readybeep > 1) readybeep--;
	else if(readybeep == 1)
	{
		// TODO: Uncomment when beep should be implemented
		//PORTC &= ~(1<<RTDS);
		readybeep = 0;
	}
	
	TCNT0 = _TM0;
}

ISR(TIMER2_OVF_vect) //8 Hz
{
	//Reset counter and full refresh LCD
	TCNT2 = _TM2;
	lcd_refresh();
}

void debounce(uint8_t* btn, uint8_t val)
{
	if(*btn >= DEBOUNCE_TIME)
	{
		if(!val) (*btn) = 0;
	}
	else if(*btn > 0)
	{
		(*btn)++;
	}
	else
	{
		if(val) (*btn) = 1;
	}
}

int main()
{
	lcd_init(LCD_DISP_ON);
	change_screen(SCREEN_WELCOME);
	
	//Initialize timer0
	TCCR0A |= (1 << CS02);// || (1 << CS01);	//Prescaler
	TCNT0 = _TM0;							//Set initial counter value
	TIMSK0 |= (1 << TOIE0);					//Overflow Interrupt Enable
	
	//Initialize timer2
	ASSR  = (1<< AS2);						//Enable asynchronous mode
	TCNT2 = _TM2;							//Set initial counter value
	TCCR2A |= (1 << CS01)|(1 << CS00);		//Prescaler
	TIFR2   = (1 << TOV2);					//Clear interrupt flags
	TIMSK2  = (1 << TOIE2);					//Overflow interrupt enable
	
	can_init();
	can_rx(MASTERID);
	
	//Set CPU into sleep mode(simultaneously enabling interrupts)
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();
	
	while(1);
}