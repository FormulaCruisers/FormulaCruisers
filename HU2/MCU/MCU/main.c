#include "Defines.h"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/eeprom.h>

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

volatile uint32_t engine_max_perc = 100;

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

volatile uint8_t anim = 0;		//Animation frame counter
volatile uint8_t av = 0;		//Range normalized of the above
uint8_t animttt = 0;			//Timer for animation frame incrementation
uint16_t welcome_anim_ttt = 0;	//Welcome screen timer for when to show the animation

uint8_t errortimer = 0;			//Timer to turn off literally everything when an error occurs

///////////// Settings menu stuff
volatile uint8_t selsetting = 0;						//The index of the selected variable to change
volatile uint8_t vsettings[SETTINGS_COUNT] = {0};		//List of all variables
uint16_t EEMEM ee_MC_N_LIMIT = 100;						//N Limit (Drive 0x34)
uint16_t EEMEM ee_MC_CURRENT_MAXPK = 100;				//I Max Peak (Drive 0xC4)
uint16_t EEMEM ee_MC_CURRENT_CONEFF = 100;				//I Continuous Efficiency (Drive 0xC5)
uint16_t EEMEM ee_MC_MAX_VAL = 100;						//Engine percentage
volatile uint8_t ischanging = 0;						//Whether or not the cursor is on the bottom(changing value) or on the top(changing index)
int16_t stimer = 0;										//Timer for saving the settings individually

void debounce(uint8_t* btn, uint8_t val);

uint8_t ttt = 0; //Counter to make sure each node only gets one request at a time
//uint8_t ttt_drive = 0;

ISR(TIMER0_COMP_vect)
{
	TCNT0 = 0;
	
	data_send8(CAN_REQUEST_DATA, SHUTDOWN, ECU2ID);
	
	debounce(&btnblue, PIND & (1<<BUTTONBLUE));
	debounce(&btngreen, PIND & (1<<BUTTONGREEN));
	debounce(&btn1, PIND & (1<<BUTTON1)); //The button that is above the green button (i.e. left)
	debounce(&btn2, PIND & (1<<BUTTON2)); //The button that is above the blue button (i.e. right)
	
	if(!shutdownon || ams_shutdown || imd_shutdown)
	{
		if(ui_current_screen == SCREEN_PREDISCHARGING || ui_current_screen == SCREEN_DRIVING || ui_current_screen == SCREEN_STATUS)
		{
			_errorcode = ERROR_SHUTDOWN;
		}
	}
	
	//Request gas/brake values
	switch(ttt)
	{
		case 0:
			data_send8(CAN_REQUEST_DATA, RPM_FRONT_LEFT, NODEID1);
			data_send8(CAN_REQUEST_DATA, GAS_1, NODEID2);
			break;
		case 1:
			data_send8(CAN_REQUEST_DATA, RPM_FRONT_RIGHT, NODEID1);
			data_send8(CAN_REQUEST_DATA, GAS_2, NODEID2);
			break;
		case 2:
			data_send8(CAN_REQUEST_DATA, STEERING_POS, NODEID1);
			data_send8(CAN_REQUEST_DATA, BRAKE, NODEID2);
			break;
	}
	
	if(_errorcode != ERROR_NONE)
	{
		//Reset literally everything possible
		if(errortimer < 0xFF) errortimer++;
		if(errortimer == 1) data_send_ecu(RUN_ENABLE, _LOW);
		if(errortimer == 2) data_send_ecu(MAIN_RELAIS, _LOW);
		if(errortimer == 3) data_send_ecu(PREDISCHARGE, _LOW);
		if(errortimer == 4) data_send_ecu(MOTOR_CONTROLLER, _LOW);
		if(errortimer == 5) data_send_ecu(PUMP_ENABLE, _LOW);
		
		//Change into error screen
		change_screen(SCREEN_ERROR);
	}
	
	switch(ui_current_screen)
	{
		case SCREEN_ANIMATION:
			//Increase the animation timer faster when the animation car is actually moving
			animttt+= (anim>20) ? 1 : 2;
			if(animttt > 50 + (anim * 2)) // (speed increasing as time goes by!)
			{
				anim--;
				animttt = 0;
				av = (anim > 20) ? 20 : anim;
				lcd_refresh();
			}
			
			//At the end of the animation, switch back to the welcome screen
			if(anim == 0) change_screen(SCREEN_WELCOME);
			
			if(btnblue == 1)
			{
				data_send_ecu(MOTOR_CONTROLLER, _HIGH);
				//After enabling, the motor controller needs some time to start accepting messages. Because of this, add 200*(1000/500) = 400ms delay.
				stimer = -200;
				if(_errorcode == ERROR_NONE) change_screen(SCREEN_SAVING);
			}
			break;
		
		case SCREEN_WELCOME:
			//Animation timer
			welcome_anim_ttt++;
			if(welcome_anim_ttt > 2000)
			{
				//Start animation
				welcome_anim_ttt = 0;
				anim = 27;
				change_screen(SCREEN_ANIMATION);
			}
			
			if(btnblue == 1)
			{
				data_send_ecu(MOTOR_CONTROLLER, _HIGH);
				//After enabling, the motor controller needs some time to start accepting messages. Because of this, add 200*(1000/500) = 400ms delay.
				stimer = -200;
				if(_errorcode == ERROR_NONE) change_screen(SCREEN_SAVING);
			}
			break;
		
		case SCREEN_SAVING:
			if (stimer == 10)
			{
				change_screen(SCREEN_START);
				stimer = 0;
			}
			else
			{
				if(stimer == 1)      data_send_motor(MC_N_LIMIT, vsettings[0], 0x7FFF, MCDL);
				else if(stimer == 2) data_send_motor(MC_N_LIMIT, vsettings[0], 0x7FFF, MCDR);
				else if(stimer == 3) data_send_motor(MC_CURRENT_MAXPK, vsettings[1], 0x3FFF, MCDL);
				else if(stimer == 4) data_send_motor(MC_CURRENT_MAXPK, vsettings[1], 0x3FFF, MCDR);
				else if(stimer == 5) data_send_motor(MC_CURRENT_CONEFF, vsettings[2], 0x3FFF, MCDL);
				else if(stimer == 6) data_send_motor(MC_CURRENT_CONEFF, vsettings[2], 0x3FFF, MCDR);
				stimer++;
			}
			break;
		
		//settings screen
		case SCREEN_SETTINGS:
			if(ischanging == 0)
			{
				//Cursor on top; Changing the selected variable
				if(btnblue == 1) ischanging++;
				else if(btn2 == 1) selsetting = (selsetting == SETTINGS_COUNT-1) ? 0 : selsetting + 1;
				else if(btn1 == 1) selsetting = (selsetting == 0) ? SETTINGS_COUNT-1 : selsetting - 1;	
			}
			else if(ischanging == 1)
			{
				//Cursor on the bottom; Changing the value of the selected variable
				if(btnblue == 1) ischanging--;
				else if(btn2 == 1 || btn2 == 0xFF) vsettings[selsetting] = (vsettings[selsetting] == 100) ? 100 : vsettings[selsetting] + 1;
				else if(btn1 == 1 || btn1 == 0xFF) vsettings[selsetting] = (vsettings[selsetting] == 0) ? 0 : vsettings[selsetting] - 1;
			}
			
			if(btngreen == 1)
			{
				//Write values back to EEPROM
				eeprom_write_word(&ee_MC_N_LIMIT, vsettings[0]);
				eeprom_write_word(&ee_MC_CURRENT_MAXPK, vsettings[1]);
				eeprom_write_word(&ee_MC_CURRENT_CONEFF, vsettings[2]);
				eeprom_write_word(&ee_MC_MAX_VAL, vsettings[3]);
				engine_max_perc = vsettings[3] * 1000;
				
				//Send values to the motor controller
				stimer = 0;
				change_screen(SCREEN_SAVING);
			}
			
			break;
		
		//The screen that appears after closing the welcome screen
		case SCREEN_START:
			if(btn1 && btn2)
			{
				ischanging = 0;
				stimer = 0;
				change_screen(SCREEN_SETTINGS);
			}
			if(btnblue == 1)
			{
				e_checksensors();
				e_checkranges();
			
				if(_errorcode == ERROR_NONE)
				{
					predistimer = PREDISCHARGE_TIMER;
					data_send_ecu(PREDISCHARGE, _HIGH);
					change_screen(SCREEN_PREDISCHARGING);
				
					readybeep = RTDS_TIME;
					// TODO: Uncomment when beep should be implemented
					//PORTC |= 1 << RTDS
				
					data_send_ecu(PUMP_ENABLE, _HIGH);
				}
			}
			break;
		
		//10 seconds of this screen while predischarging.
		//Also checks the pumps to see if they have any flow, otherwise turn off!
		case SCREEN_PREDISCHARGING:
			predistimer -= 2;
			if(predistimer == 0)
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
		
		//The screen that appears after predischarging; Only one press of the green LAUNCH button to start driving. (run_enable)
		case SCREEN_STATUS:
			if(btngreen == 1)
			{
				data_send_ecu(RUN_ENABLE, _HIGH);
				change_screen(SCREEN_DRIVING);
			}
			break;
			
		//The screen that appears when actually driving.
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
			
			if(_errorcode == ERROR_NONE)
			{
				/*
				//Alternate sending data to the drivers; If sending both at the same time, neither work.
				//This *does* mean that one of the drivers will be delayed by 1000/500 = 2ms which could theoretically be a problem.
				if(ttt_drive == 0)
					data_send16(MC_SET_TORQUE, -gas1eng, MCDR); //Right driver should get a negative value to drive forward
				else if(ttt_drive == 1)
					data_send16(MC_SET_TORQUE, gas1eng, MCDL);
				
				ttt_drive = 1 - ttt_drive;*/
				
				data_send_motor(MC_SET_TORQUE, -gas1eng, ENGINE_MAX, MCDR); //Right driver should get a negative value to drive forward
				_delay_us(2);	//Experimental: 2 µs delay between drivers instead of using timer
				data_send_motor(MC_SET_TORQUE, gas1eng, ENGINE_MAX, MCDL);
			}
			break;
			
		case SCREEN_ERROR:
			if(btngreen == 1 && errortimer == 0xFF)
			{
				errortimer = 0;
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
	
	e_checkCAN();
}

void debounce(uint8_t* btn, uint8_t val)
{
	if(*btn >= DEBOUNCE_TIME)
	{
		if(!val) (*btn) = 0;
	}
	
	if(*btn > 0)
	{
		if(*btn < 0xFF) (*btn)++;
		else            (*btn) = 0xE0; //Reset to E0 so that hold-to-change-faster doesn't go way too fast.
	}
	else
	{
		if(val) (*btn) = 1;
	}
}

int main()
{
	vsettings[0] = eeprom_read_word(&ee_MC_N_LIMIT);
	vsettings[1] = eeprom_read_word(&ee_MC_CURRENT_MAXPK);
	vsettings[2] = eeprom_read_word(&ee_MC_CURRENT_CONEFF);
	vsettings[3] = eeprom_read_word(&ee_MC_MAX_VAL);
	
	//In case there is a weird value, reset to defaults.
	if(vsettings[0] > 100) vsettings[0] = 100;
	if(vsettings[1] > 100) vsettings[1] = 100;
	if(vsettings[2] > 100) vsettings[2] = 100;
	if(vsettings[3] > 100) vsettings[3] = 100;
	
	engine_max_perc = vsettings[3] * 1000;
	
	lcd_init(LCD_DISP_ON);
	change_screen(SCREEN_WELCOME);
	
	//Initialize timer0
	TCCR0A |= (1 << CS02);	//Prescaler
	TCNT0 = 0;							//Set initial counter value
	OCR0A = _TM0;
	TIMSK0 |= (1 << OCIE0A);					//Overflow Interrupt Enable
	
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