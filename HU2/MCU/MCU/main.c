/* MAIN.C
This file contains the entry point of the program and the main loop.
The base state machine(by way of screens) is controlled in this file.
 */

//#define _NOCAN //Uncomment this to disable all CAN messages in the main loop

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
#include "sd_raw.h"
#include "MotorController.h"
#include "AMS.h"
#include "Differential.h"

volatile uint16_t gas1 = 0;
volatile uint16_t gas2 = 0;
volatile uint16_t brake = 0;
volatile uint16_t gas1perc = 0;
volatile uint16_t gas2perc = 0;
volatile uint16_t brakeperc = 0;
volatile double gas1eng = 0;

volatile uint32_t engine_max_perc = 100;

volatile uint8_t shutdownon = 0;

volatile uint16_t rpm_fl = 0;
volatile uint16_t rpm_fr = 0;
volatile uint16_t rpm_bl = 0;
volatile uint16_t rpm_br = 0;
volatile int16_t steerpos = 0;

volatile uint16_t flowleft = 0;
volatile uint16_t flowright = 0;
volatile uint16_t templeft = 0;
volatile uint16_t tempright = 0;

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

bool brakelighton = false;

//Boot counter for data logging
uint16_t EEMEM ee_boot_count = 1;
uint16_t boot_count;

//SD card variables
uint8_t sdbuffer[512] = {0xff};
uint16_t sd_current_pos = 0;

//test screen variables
volatile uint8_t dt_engv = 0;
volatile uint8_t pump_pwm = 0;
volatile uint8_t test_sensor = 0x10;
volatile uint32_t test_value = 0x00000000;

//Calibration values
uint16_t EEMEM ee_Gas1_min = 0x301;
uint16_t EEMEM ee_Gas1_max = 0x339;
uint16_t EEMEM ee_Gas2_min = 0x1F1;
uint16_t EEMEM ee_Gas2_max = 0x238;
uint16_t EEMEM ee_Brake_min = 0x014;
uint16_t EEMEM ee_Brake_max = 0x030;

uint16_t GAS1MIN = 0x301;
uint16_t GAS1MAX = 0x339;
uint16_t GAS2MIN = 0x1F1;
uint16_t GAS2MAX = 0x238;
uint16_t BRAKEMIN = 0x014;
uint16_t BRAKEMAX = 0x030;


//AMS data
volatile AMS_OVERALL amsd_overall;
volatile AMS_DIAGNOSTIC amsd_diagnostic;
volatile AMS_VOLTAGE amsd_voltage;
volatile AMS_CELL_MODULE_TEMP amsd_cell_module_temp;
volatile AMS_CELL_TEMP amsd_cell_temp;
volatile AMS_CELL_BALANCING amsd_cell_balancing;
//volatile struct AMS_CONFIGURATION amsd_configuration;
//volatile struct AMS_LOGIN amsd_login;


//Debug value
extern volatile uint32_t tx_count;

ISR(TIMER0_COMP_vect)
{	
	//tx_count = TCNT0;
	TCNT0 = 0;

#ifndef _NOCAN	
	data_send8(CAN_REQUEST_DATA, SHUTDOWN, ECU2ID);
#endif
	
	//*
	debounce(&btnblue, PIND & (1<<BUTTONBLUE));
	debounce(&btngreen, PIND & (1<<BUTTONGREEN));
	debounce(&btn1, PIND & (1<<BUTTON1)); //The button that is above the green button (i.e. left)
	debounce(&btn2, PIND & (1<<BUTTON2)); //The button that is above the blue button (i.e. right)
	
	if(!shutdownon || ams_shutdown || imd_shutdown)
	{
		if(ui_current_screen == SCREEN_PREDISCHARGING || ui_current_screen == SCREEN_DRIVING || ui_current_screen == SCREEN_STATUS || ui_current_screen == SCREEN_DRIVETEST)
		{
			_errorcode = ERROR_SHUTDOWN;
		}
	}
	
	//Request gas/brake values
#ifndef _NOCAN
	if(ui_current_screen != SCREEN_TEST)
	{
		switch(ttt)
		{
			case 0:
				//data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){BRAKE}, NODEID2, 1); //Not connected anymore
				data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){RPM_FRONT_RIGHT, RPM_FRONT_LEFT}, NODEID1, 2);
				data_send0(AMS_MSG_OVERALL);
				break;
			case 1:
				data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){STEERING_POS}, NODEID1, 1);
				data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){GAS_1, GAS_2}, NODEID2, 2);
				data_send0(AMS_MSG_VOLTAGE);
				break;
		}
	}
#endif
	
	if(_errorcode != ERROR_NONE)
	{
		//Reset literally everything possible
		if(errortimer < 0xFF) errortimer++;
#ifndef _NOCAN
		if(errortimer == 1) data_send_ecu(RUN_ENABLE, _LOW);
		if(errortimer == 2) data_send_ecu(MAIN_RELAIS, _LOW);
		if(errortimer == 3) data_send_ecu(PREDISCHARGE, _LOW);
		if(errortimer == 4) data_send_ecu(MOTOR_CONTROLLER, _LOW);
		if(errortimer == 5) data_send_ecu(PUMP_ENABLE, _LOW);
		if(errortimer == 6) data_send_ecu(BRAKELIGHT, _LOW);
#endif
		
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
			
			//Purposefully fall through to the next label. Do not add a break here.
		
		case SCREEN_WELCOME:
			//Animation timer
			if(anim == 0)
			{
				welcome_anim_ttt++;
				if(welcome_anim_ttt > 2000)
				{
					//Start animation
					welcome_anim_ttt = 0;
					anim = 27;
					change_screen(SCREEN_ANIMATION);
				}
			}
			
			if(btnblue == 1)
			{
#ifndef _NOCAN
				data_send_ecu(MOTOR_CONTROLLER, _HIGH);
#endif
				//After enabling, the motor controller needs some time to start accepting messages. Because of this, add 200*(1000/500) = 400ms delay.
				stimer = -200;
				if(_errorcode == ERROR_NONE) change_screen(SCREEN_SAVING);
			}
						
			if(btn1 && btn2)
			{
				//data_send_ecu(PUMP_ENABLE, _HIGH);
				change_screen(SCREEN_TEST);
			}
			break;
		
		//The screen that appears after closing the welcome screen
		case SCREEN_START:
			//e_checksensors();
			//e_checkranges();
			//e_checkdiscrepancy();
			
			//Settings
			if(btn1 && btn2)
			{
				ischanging = 0;
				stimer = 0;
				change_screen(SCREEN_SETTINGS);
			}
			
			//Start predischarging
			if(btnblue == 1)
			{
				if(_errorcode == ERROR_NONE)
				{
					predistimer = PREDISCHARGE_TIMER;
#ifndef _NOCAN
					data_send_ecu(PREDISCHARGE, _HIGH);
					data_send_ecu(PUMP_ENABLE, _HIGH);
#endif
					change_screen(SCREEN_PREDISCHARGING);
				
					readybeep = RTDS_TIME;
					// TODO: Uncomment when beep should be implemented
					//PORTC |= 1 << RTDS
				}
			}
			
			//Calibration
			if(btngreen == 1)
			{
				GAS1MIN = gas1 + CALIB_SLACK;
				GAS2MIN = gas2 + CALIB_SLACK;
				BRAKEMIN = brake + CALIB_SLACK;
			}
			break;
		
		//5 seconds of this screen while predischarging.
		//Also checks the pumps to see if they have any flow, otherwise turn off!
		case SCREEN_PREDISCHARGING:
			predistimer -= 2;
			if(predistimer == 0)
			{
#ifndef _NOCAN
				data_send_ecu(MAIN_RELAIS, _HIGH);
#endif
				change_screen(SCREEN_STATUS);
			}
			break;
		
		//The screen that appears after predischarging; Only one press of the green LAUNCH button to start driving. (run_enable)
		case SCREEN_STATUS:
			if(btngreen == 1)
			{
#ifndef _NOCAN
				data_send_ecu(RUN_ENABLE, _HIGH);
#endif
				change_screen(SCREEN_DRIVING);
			}
			if(btn1 && btn2)
			{
#ifndef _NOCAN
				data_send_ecu(RUN_ENABLE, _HIGH);
#endif
				change_screen(SCREEN_DRIVETEST);
			}
			break;
		
		//The screen that appears when actually driving.
		case SCREEN_DRIVING:
			if(ttt == 3)
			{
				//e_checkflow();
				e_checksensors();
				e_checkranges();
				e_checkdiscrepancy();
			}
				
			if(_errorcode == ERROR_NONE)
			{
				struct torques tq = getDifferential(gas1eng, steerpos);
					
				struct slips sp = detectSlip(rpm_bl, rpm_br, tq);
				tq = solveSlip(sp, tq);

#ifndef _NOCAN					
				data_send_motor_d(MC_SET_TORQUE, -tq.right_perc, ENGINE_MAX, MCDR); //Right driver should get a negative value to drive forward
				_delay_us(2);	//Experimental: 2 µs delay between drivers instead of using timer
				data_send_motor_d(MC_SET_TORQUE, tq.left_perc, ENGINE_MAX, MCDL);
#endif
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
					
				//Send values to the motor controller
				stimer = 0;
				change_screen(SCREEN_SAVING);
			}
			break;
				
		//Saving for the settings of the motor controller
		case SCREEN_SAVING:
			if (stimer == 10)
			{
				change_screen(SCREEN_START);
				stimer = 0;
			}
			else
			{
				engine_max_perc = vsettings[3];
#ifndef _NOCAN
				if(stimer == 1)      data_send_motor(MC_N_LIMIT, vsettings[0], 0x7FFF, MCDL);
				else if(stimer == 2) data_send_motor(MC_N_LIMIT, vsettings[0], 0x7FFF, MCDR);
				else if(stimer == 3) data_send_motor(MC_I_MAXPK_PERCENT, vsettings[1], 0x3FFF, MCDL);
				else if(stimer == 4) data_send_motor(MC_I_MAXPK_PERCENT, vsettings[1], 0x3FFF, MCDR);
				else if(stimer == 5) data_send_motor(MC_I_CONEFF_PERCENT, vsettings[2], 0x3FFF, MCDL);
				else if(stimer == 6) data_send_motor(MC_I_CONEFF_PERCENT, vsettings[2], 0x3FFF, MCDR);
#endif
				stimer++;
			}
			break;
				
		//Calibration screen
		case SCREEN_CALIBRATE:
			if(btngreen == 1)
			{
				GAS1MAX = gas1 - CALIB_SLACK;
				GAS2MAX = gas2 - CALIB_SLACK;
				BRAKEMAX = brake;
					
				//Write to EEPROM
				eeprom_write_word(&ee_Gas1_min, GAS1MIN);
				eeprom_write_word(&ee_Gas1_max, GAS1MAX);
				eeprom_write_word(&ee_Gas2_min, GAS2MIN);
				eeprom_write_word(&ee_Gas2_max, GAS2MAX);
				eeprom_write_word(&ee_Brake_min, BRAKEMIN);
				eeprom_write_word(&ee_Brake_max, BRAKEMAX);
			}
			break;
		
		
		
		
		
		
		
		
		
		
		
		//sensor testing screen
		case SCREEN_TEST:
			if(btnblue == 1 || btnblue == 0xFF) test_sensor += 0x10;
			else if(btn2 == 1 || btn2 == 0xFF) test_sensor = ((test_sensor + 0x01) & 0x0F) + (test_sensor & 0xF0);
			else if(btn1 == 1 || btn1 == 0xFF) test_sensor = ((test_sensor - 0x01) & 0x0F) + (test_sensor & 0xF0);
			
			if((btn1 == 1 && btn2 > 0) || (btn1 > 0 && btn2 == 1)) change_screen(SCREEN_PUMPTEST);
		
			//Skip 4-7 and 12-15 because those are never used
			if((test_sensor & 0x04) > 0)
			{
				if(test_sensor & 0x01)	test_sensor ^= 0b00000100;
				else					test_sensor ^= 0b00001100;
			}
		
			//Return button
			if(btngreen == 1) change_screen(SCREEN_WELCOME);
		
			//Figure out which node to send it to
			uint8_t tnode = (test_sensor & 0xF0);
			uint16_t actualnode;
			if(tnode == 0x10) actualnode = NODEID1;
			else if(tnode == 0x20) actualnode = NODEID2;
			else if(tnode == 0x30) actualnode = NODEID3;
			else if(tnode == 0x40) actualnode = NODEID4;
			else break;
			
#ifndef _NOCAN
			data_send8(CAN_REQUEST_DATA, test_sensor, actualnode);
#endif
			
			break;
		
		//Drive test screen, used for setting a specific motor value and keeping it there
		case SCREEN_DRIVETEST:
			if(_errorcode == ERROR_NONE)
			{
				if(btn2 == 1 || btn2 == 0xFF) dt_engv = ((dt_engv == 100) ? 100 : dt_engv + 1);
				if(btn1 == 1 || btn1 == 0xFF) dt_engv = ((dt_engv == 0) ? 0 : dt_engv - 1);
				
				struct torques tq = getDifferential(dt_engv, steerpos);
				
#ifndef _NOCAN
				if(btnblue == 1) data_send_motor_d(MC_SET_TORQUE, 0, ENGINE_MAX, MCDR);
				if(btnblue == 2) data_send_motor_d(MC_SET_TORQUE, 0, ENGINE_MAX, MCDL);
				
				if(btngreen == 1) data_send_motor_d(MC_SET_TORQUE, -tq.right_perc, ENGINE_MAX, MCDR);
				if(btngreen == 2) data_send_motor_d(MC_SET_TORQUE, tq.left_perc, ENGINE_MAX, MCDL);
#endif
			}
			break;
			
		//Pump test screen
		case SCREEN_PUMPTEST:
			if(btn2 == 1 || btn2 == 0xFF) pump_pwm = ((pump_pwm == 0xFF) ? 0xFF : pump_pwm + 1);
			if(btn1 == 1 || btn1 == 0xFF) pump_pwm = ((pump_pwm == 0) ? 0 : pump_pwm - 1);
			if(btnblue == 1 || btngreen == 1) pump_pwm = 0;

#ifndef _NOCAN
			data_send_ecu(PUMP_ENABLE, pump_pwm);
#endif
			
			if(btngreen == 1) change_screen(SCREEN_WELCOME);
			
			
			
			
		
		
		
		
		//Error screen	
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
	
	
	
	
	
	//Brakelight logic
	if(brakeperc >= BL_SWITCHON && !brakelighton)
	{
		brakelighton = true;
#ifndef _NOCAN
		data_send_ecu(BRAKELIGHT, _HIGH);
#endif
	}
	else if(brakeperc < BL_SWITCHOFF && brakelighton)
	{
		brakelighton = false;
#ifndef _NOCAN
		data_send_ecu(BRAKELIGHT, _LOW);
#endif
	}

	//mod-2 timer increase
	ttt = 1 - ttt;
	
	if(readybeep > 1) readybeep--;
	else if(readybeep == 1)
	{
		// TODO: Uncomment when beep should be implemented
		//PORTC &= ~(1<<RTDS);
		readybeep = 0;
	}
	
	//Check for any CAN errors at the end of the loop
	e_checkCAN();
	//*/
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
	
	boot_count = eeprom_read_word(&ee_boot_count);
	eeprom_write_word(&ee_boot_count, boot_count+1);
	
	GAS1MIN = eeprom_read_word(&ee_Gas1_min);
	GAS1MAX = eeprom_read_word(&ee_Gas1_max);
	GAS2MIN = eeprom_read_word(&ee_Gas2_min);
	GAS2MAX = eeprom_read_word(&ee_Gas2_max);
	BRAKEMIN = eeprom_read_word(&ee_Brake_min);
	BRAKEMAX = eeprom_read_word(&ee_Brake_max);
	
	
	
	lcd_init(LCD_DISP_ON);
	change_screen(SCREEN_WELCOME);
	
	//Initialize timer0
	TCCR0A |= (1 << CS02);					//Prescaler
	TCNT0 = 0;								//Set initial counter value
	OCR0A = _TM0;
	TIMSK0 |= (1 << OCIE0A);				//Overflow Interrupt Enable
	
	//Initialize timer2
	ASSR  = (1<< AS2);						//Enable asynchronous mode
	TCNT2 = _TM2;							//Set initial counter value
	TCCR2A |= (1 << CS01)|(1 << CS00);		//Prescaler
	TIFR2   = (1 << TOV2);					//Clear interrupt flags
	TIMSK2  = (1 << TOIE2);					//Overflow interrupt enable
	
	can_init();
	can_rx(MASTERID);
#ifdef USE_SD_CARD
	sd_raw_init();
#endif
	
	//Set CPU into sleep mode(simultaneously enabling interrupts)
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();
	
	while(1);
}
