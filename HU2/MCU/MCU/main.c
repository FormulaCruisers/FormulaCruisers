/* MAIN.C
This file contains the entry point of the program and the main loop.
The base state machine(by way of screens) is controlled in this file.
 */

//#define _NOCAN //Uncomment this to disable all CAN messages in the main loop
//#define USE_SD_CARD //Enable SD card
//#define REGULAR_LOG //Enable regularly logging

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
#include "ADC.h"

volatile uint16_t gas1 = 0;
volatile uint16_t gas2 = 0;
volatile uint16_t brake = 0;
volatile uint16_t gas1perc = 0;
volatile uint16_t gas2perc = 0;
volatile uint16_t brakeperc = 0;
volatile double velocity = 0;
volatile double accel_gforce = 0;
volatile double battery_voltage = 0;
volatile double gas1eng = 0;

volatile uint8_t shutdownon = 0;
volatile uint8_t shutdowntimer = 0;

volatile uint16_t rpm_fl = 0;
volatile uint16_t rpm_fr = 0;
volatile uint16_t rpm_bl = 0;
volatile uint16_t rpm_br = 0;
volatile double steerpos = 0;
volatile uint16_t steerposm = 0;

volatile uint16_t flowleft = 0;
volatile uint16_t flowright = 0;
volatile uint16_t templeft = 0;
volatile uint16_t tempright = 0;

volatile uint16_t mcurrent = 0; //Directly measured value on the ADC
volatile int16_t ccurrent = 0;  //Calculated to the amount of amps

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
volatile uint8_t test_mob = 0;
volatile uint64_t test_value = 0;

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

uint16_t logtimer = 0;

//Timer to set up sensors of can nodes
uint8_t sentimer = 0;


///////////// Settings menu stuff
volatile uint8_t selsetting = 0;						//The index of the selected variable to change
volatile uint8_t vsettings[SETTINGS_COUNT] = {0};		//List of all variables
volatile uint8_t ischanging = 0;						//Whether or not the cursor is on the bottom(changing value) or on the top(changing index)
int16_t stimer = 0;										//Timer for saving the settings individually

uint16_t EEMEM ee_MC_N_LIMIT = 100;						//N Limit (Drive 0x34)
uint16_t EEMEM ee_MC_CURRENT_MAXPK = 100;				//I Max Peak (Drive 0xC4)
uint16_t EEMEM ee_MC_CURRENT_CONEFF = 100;				//I Continuous Efficiency (Drive 0xC5)
uint16_t EEMEM ee_MC_MAX_VAL = 100;						//Engine percentage
uint16_t EEMEM ee_MC_DIFF_FAC = 0;						//Differential percentage

volatile uint32_t engine_max_perc = 100;
volatile uint8_t differential_perc = 100;

uint16_t disable_drive_timer = 0;
uint8_t disable_motor_braking = 0;
uint8_t allow_turning_on = 0;

ISR(TIMER0_COMP_vect)
{	
	//tx_count = TCNT0;
	TCNT0 = 0;

	//Read all MOb messages
#ifndef _NOCAN	

	if(sentimer == 1) data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){STEERING_POS, RPM_FRONT_LEFT, RPM_FRONT_RIGHT}, NODEID1, 3);
	if(sentimer == 2) data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){GAS_1, GAS_2, BRAKE}, NODEID2, 3);
	//if(sentimer == 3) data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){RPM_BACK_LEFT, FLOW_LEFT, TEMP_LEFT}, NODEID3, 3);
	//if(sentimer == 4) data_send_arr(CAN_REQUEST_DATA, (uint8_t[]){RPM_BACK_RIGHT, FLOW_RIGHT, TEMP_RIGHT}, NODEID4, 3);
	if(sentimer == 5) data_send0(AMS_MSG_VOLTAGE);
	
	if(sentimer % 20 == 0) req_ADC(A_MCURRENT);
	
	sentimer++;
	if(sentimer > 200) sentimer = 0;





	//Steering box
	steerposm = g(NODEID1, MOB_STEERING_POS);
	steerpos = (int16_t)(steerposm - STEER_MIDDLE); // NOT in degrees
	rpm_fl = 500000.d / (double)g(NODEID1, MOB_RPM_FRONT_LEFT);
	rpm_fr = 500000.d / (double)g(NODEID1, MOB_RPM_FRONT_RIGHT);
	
	//velocity and acceleration
	double pv = velocity;
	velocity = (rpm_fr > rpm_fl ? rpm_fr : rpm_fl) * 0.1014; // (1.69m * 3.6) / 60 =  0.1014 
	accel_gforce = (velocity - pv) * 14.158; // (500 / 3.6) / 9.81 = 14.15788878
	
	//AMS values
	battery_voltage = (amsd_voltage.Total_Voltage_LSW_H * 256 + amsd_voltage.Total_Voltage_LSW_L) * 0.01;
	
	//Pedalbox
	gas1 = g(NODEID2, MOB_GAS1);
	gas2 = g(NODEID2, MOB_GAS2);
	brake = g(NODEID2, MOB_BRAKE);
	
	/*
	//Node 3
	rpm_bl = 500000.d / (double)g(NODEID3, MOB_RPM_BACK_LEFT); //straal * 2pi * (rpn_br) / 60 * 3.6
	flowleft = g(NODEID3, MOB_FLOW_LEFT);
	templeft = g(NODEID3, MOB_TEMP_LEFT);
	
	templeft = (templeft - 65570);// * 0.26;		//temp right in voltage 0-255 (0-5 volt) 
	templeft = templeft*0.26;
	templeft = 1.96*templeft;						// 0-255 to 0 to 5 volt *100
	templeft = (templeft*-.2344)+ 91.622;			//linealisering for temp calculations
	//tempright = tempright
	
	
	//Node 4
	rpm_br = 500000.d / (double)g(NODEID4, MOB_RPM_BACK_RIGHT); //straal * 2pi * (rpn_br) / 60 
	flowright = g(NODEID4, MOB_FLOW_RIGHT);
	tempright= g(NODEID4, MOB_TEMP_RIGHT);
	
	tempright = (tempright - 65570);				//temp right in voltage 0-255 (0-5 volt) 
	tempright = (tempright*-0.11945024) + 91.622;	//linealisering for temp calculations
	//tempright = tempright*/

	//tempright = -4.7037*((tempright*(5/255))*(tempright*(5/255))*(tempright*(5/255))) + 38.992*((tempright*(5/255))*(tempright*(5/255))) - 117.24*(tempright*(5/255)) + 148.4;		// temp in graden

	
	shutdownon = g(ECU2ID, MOB_SHUTDOWN) ? 0 : 1;
	if(shutdowntimer < SHUTDOWN_TIME || !shutdownon) shutdowntimer = (shutdowntimer + shutdownon) * shutdownon;

	//Processing of all variables
	//Gas and brake percentages
	gas1perc = (gas1 < GAS1MIN) ? 0 : ((gas1 > GAS1MAX) ? (GAS1MAX - GAS1MIN) : (gas1 - GAS1MIN));
	gas1perc = (gas1perc * 100) / (GAS1MAX - GAS1MIN);
	gas2perc = (gas2 < GAS2MIN) ? 0 : ((gas2 > GAS2MAX) ? (GAS2MAX - GAS2MIN) : (gas2 - GAS2MIN));
	gas2perc = (gas2perc * 100) / (GAS2MAX - GAS2MIN);
	brakeperc = (brake < BRAKEMIN) ? 0 : ((brake > BRAKEMAX) ? (BRAKEMAX - BRAKEMIN) : (brake - BRAKEMIN));
	brakeperc = (brakeperc * 100) / (BRAKEMAX - BRAKEMIN);
	
	//RPM
	if(rpm_fl > 10000 || rpm_fl < 8) rpm_fl = 0;
	if(rpm_fr > 10000 || rpm_fr < 8) rpm_fr = 0;
	if(rpm_bl > 10000 || rpm_bl < 8) rpm_bl = 0;
	if(rpm_br > 10000 || rpm_br < 8) rpm_br = 0;
	
	//Flow vars
	if(flowleft == 0xFFFF)	flowleft = 0;
	else					flowleft = (uint16_t)(500000.d / (double)flowleft);
	if(flowright == 0xFFFF)	flowright = 0;
	else					flowright = (uint16_t)(500000.d / (double)flowright);
	
	ccurrent = (int16_t)((mcurrent - 512) * 0.5132);
#endif
	
#ifdef USE_SD_CARD
#ifdef REGULAR_LOG
	if(logtimer++ > LOG_DELAY)
	{
		logtimer = 0;
		char lbuf[49];
		snprintf(lbuf, sizeof(lbuf), "%1d%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x%04x", shutdownon, gas1, gas2, brake, rpm_fl, rpm_fr, rpm_bl, rpm_br, steerpos, templeft, tempright, flowleft, flowright,velocity);
		sd_log("DATA:", (uint8_t*)lbuf, sizeof(lbuf));
	}
#endif
#endif
	
	//*
	debounce(&btnblue, PIND & (1<<BUTTONBLUE));
	debounce(&btngreen, PIND & (1<<BUTTONGREEN));
	debounce(&btn1, PIND & (1<<BUTTON1)); //The button that is above the green button (i.e. left)
	debounce(&btn2, PIND & (1<<BUTTON2)); //The button that is above the blue button (i.e. right)
	
	if(shutdownon >= SHUTDOWN_TIME || ams_shutdown || imd_shutdown)
	{
		if(ui_current_screen == SCREEN_PREDISCHARGING || ui_current_screen == SCREEN_DRIVING || ui_current_screen == SCREEN_STATUS || ui_current_screen == SCREEN_DRIVETEST)
		{
			_errorcode = ERROR_SHUTDOWN;
		}
	}
	
	//Request gas/brake values
#ifndef _NOCAN
	switch(ttt)
	{
		case 0:
			data_send0(AMS_MSG_OVERALL);
			break;
		case 1:
			data_send0(AMS_MSG_VOLTAGE);
			break;
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

#ifdef USE_SD_CARD		
		if(errortimer == 1) sd_log_s("[ERROR]", get_error(_errorcode));
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
				
				//Go to screen_saving before starting to grab values from EEPROM and send them to the MCs
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
					//data_send_ecu(PUMP_ENABLE, _HIGH);
#endif
					change_screen(SCREEN_PREDISCHARGING);
				
					
				}
			}
			
			//Calibration
			if(btngreen == 1)
			{
				GAS1MIN = gas1 + CALIB_SLACK;
				GAS2MIN = gas2 + CALIB_SLACK;
				BRAKEMIN = brake + CALIB_SLACK;
				change_screen(SCREEN_CALIBRATE);
			}
			
			break;
		
		//5 seconds of this screen while predischarging.
		//Also checks the pumps to see if they have any flow, otherwise turn off!
		case SCREEN_PREDISCHARGING:
			
			predistimer -= 2;
			if(predistimer == 0)
			{
				
#ifndef _NOCAN
				//data_send_ecu(MAIN_RELAIS, _HIGH);
				data_send_ecu(PUMP_ENABLE, _HIGH);
#endif
				change_screen(SCREEN_STATUS);
			}
			break;
		
		//The screen that appears after predischarging; Only one press of the green LAUNCH button to start driving. (run_enable)
		case SCREEN_STATUS:
			if(allow_turning_on)
			{
				if(btngreen == 1)
				{
#ifndef _NOCAN
					data_send_ecu(RUN_ENABLE, _HIGH);
#endif
					//ready to drive sound
					readybeep = RTDS_TIME;
					PORTC |= 1 << RTDS;
				
					change_screen(SCREEN_DRIVING);
				}
				if(btn1 && btn2)
				{
#ifndef _NOCAN
					data_send_ecu(RUN_ENABLE, _HIGH);
#endif
					//ready to drive sound
					readybeep = RTDS_TIME;
					PORTC |= 1 << RTDS;
				
					change_screen(SCREEN_DRIVETEST);
				}
			}
			//data_send_ecu(PUMP_ENABLE, _HIGH); //put on the pumps after predischarge (against voltage drop)
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
				struct torques tq;
				if(disable_motor_braking)
				{
					tq.right_perc = 0;
					tq.left_perc = 0;
				}
				else
				{
					//tq = getDifferential(gas1perc, steerpos, vsettings[SETTING_DIFF_FAC]);
					
					//struct slips sp = detectSlip(rpm_bl, rpm_br, tq);	
					//tq = solveSlip(sp, tq);
					tq.right_perc = gas1perc;
					tq.left_perc = gas1perc;
				}
				

#ifndef _NOCAN					
				data_send_motor_d(MC_SET_TORQUE, -tq.right_perc, ENGINE_MAX, MCDR); //Right driver should get a negative value to drive forward
				_delay_us(2);	//Experimental: 2 µs delay between drivers instead of using timer
				data_send_motor_d(MC_SET_TORQUE, tq.left_perc, ENGINE_MAX, MCDL);
#endif
			}
			
			//Turn off vehicle by pressing top buttons together for more than 1 second
			if((btn1 == 1 && btn2 > 0) || (btn1 > 0 && btn2 == 1))
			{
				disable_drive_timer++;
				if(disable_drive_timer > 500)
				{
#ifndef _NOCAN
					data_send_ecu(RUN_ENABLE, _LOW);
#endif
					change_screen(SCREEN_STATUS);
				}
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
				eeprom_write_word(&ee_MC_N_LIMIT, vsettings[SETTING_N_LIMIT]);
				eeprom_write_word(&ee_MC_CURRENT_MAXPK, vsettings[SETTING_CUR_MAXPK]);
				eeprom_write_word(&ee_MC_CURRENT_CONEFF, vsettings[SETTING_CUR_CONEFF]);
				eeprom_write_word(&ee_MC_MAX_VAL, vsettings[SETTING_MAX_VAL]);
				eeprom_write_word(&ee_MC_DIFF_FAC, vsettings[SETTING_DIFF_FAC]);
					
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
				engine_max_perc = vsettings[SETTING_MAX_VAL];
				differential_perc = vsettings[SETTING_DIFF_FAC];
#ifndef _NOCAN
				if(stimer == 1)      data_send_motor(MC_N_LIMIT, vsettings[SETTING_N_LIMIT], 0x7FFF, MCDL);
				else if(stimer == 2) data_send_motor(MC_N_LIMIT, vsettings[SETTING_N_LIMIT], 0x7FFF, MCDR);
				else if(stimer == 3) data_send_motor(MC_I_MAXPK_PERCENT, vsettings[SETTING_CUR_MAXPK], 0x3FFF, MCDL);
				else if(stimer == 4) data_send_motor(MC_I_MAXPK_PERCENT, vsettings[SETTING_CUR_MAXPK], 0x3FFF, MCDR);
				else if(stimer == 5) data_send_motor(MC_I_CONEFF_PERCENT, vsettings[SETTING_CUR_CONEFF], 0x3FFF, MCDL);
				else if(stimer == 6) data_send_motor(MC_I_CONEFF_PERCENT, vsettings[SETTING_CUR_CONEFF], 0x3FFF, MCDR);
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
				change_screen(SCREEN_START);
			}
			break;
		
		
		
		
		
		
		
		
		
		
		
		//sensor testing screen
		case SCREEN_TEST:
			if(btn2 == 1 || btn2 == 0xFF) test_mob = (test_mob == MOBCOUNT - 1) ? 0 : test_mob + 1;
			else if(btn1 == 1 || btn1 == 0xFF) test_mob = (test_mob == 0) ? MOBCOUNT - 1 : test_mob - 1;
		
			//Return button
			if(btngreen == 1) change_screen(SCREEN_WELCOME);
			
			test_value = (uint64_t)getrawmob(test_mob);
			
			break;
		
		//Drive test screen, used for setting a specific motor value and keeping it there
		case SCREEN_DRIVETEST:
			if(_errorcode == ERROR_NONE)
			{
				if(btn2 == 1 || btn2 == 0xFF) dt_engv = ((dt_engv == 100) ? 100 : dt_engv + 1);
				if(btn1 == 1 || btn1 == 0xFF) dt_engv = ((dt_engv == 0) ? 0 : dt_engv - 1);
				
				struct torques tq;
				if(disable_motor_braking)
				{
					tq.right_perc = 0;
					tq.left_perc = 0;
				}
				else
				{
					//tq = getDifferential(dt_engv, steerpos, vsettings[SETTING_DIFF_FAC]);
					tq.right_perc = dt_engv;
					tq.left_perc = dt_engv;
				}
				
#ifndef _NOCAN
				if(btnblue == 1) data_send_motor_d(MC_SET_TORQUE, 0, ENGINE_MAX, MCDR);
				if(btnblue == 2) data_send_motor_d(MC_SET_TORQUE, 0, ENGINE_MAX, MCDL);
				
				if(btngreen == 1) data_send_motor_d(MC_SET_TORQUE, -tq.right_perc, ENGINE_MAX, MCDR);
				if(btngreen == 2) data_send_motor_d(MC_SET_TORQUE, tq.left_perc, ENGINE_MAX, MCDL);
#endif
			}
			break;
			
			
			
			
		
		
		
		
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
	
	//As per regulations, cut motors (i.e. only and immediately send the value 0) when braking is over 25% and re-enable when it's under 5%
	if(brakeperc > CUTPOWER_BRAKE_H)
	{
		disable_motor_braking = 1;
	}
	if(brakeperc < CUTPOWER_BRAKE_L)
	{
		disable_motor_braking = 0;
	}
	//As per regulations, only allow turning on the car if the brake is pressed
	allow_turning_on = (brakeperc > START_BRAKE_MINPERCENT);
	
	
	
	
	//mod-2 timer increase
	ttt = 1 - ttt;
	
	if(readybeep > 1) 
	{
		//delay(500);
		readybeep--;
	}
	else if(readybeep <= 1)
	{
		// TODO: Uncomment when beep should be implemented
		PORTC &= ~(1<<RTDS);
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
		//increase if button is registered
		if(val)
		{
			if(*btn < 0xFF) (*btn)++;
			else            (*btn) = 0xE0; //Reset to E0 so that hold-to-change-faster doesn't go way too fast.
		}
	}
	else
	{
		if(val) (*btn) = 1;
	}
}

int main(void)
{
	//Disable JTAG debugging to use the JTAG pins as analog inputs
	//To disable, you have to write 1 to bit 7 of register MCUSR within 4 cycles. Because = is faster than |=, we use the former to be certain it will work.
	MCUSR = (1<<JTD);
	MCUSR = (1<<JTD);
	
	
	vsettings[SETTING_N_LIMIT] = eeprom_read_word(&ee_MC_N_LIMIT);
	vsettings[SETTING_CUR_MAXPK] = eeprom_read_word(&ee_MC_CURRENT_MAXPK);
	vsettings[SETTING_CUR_CONEFF] = eeprom_read_word(&ee_MC_CURRENT_CONEFF);
	vsettings[SETTING_MAX_VAL] = eeprom_read_word(&ee_MC_MAX_VAL);
	vsettings[SETTING_DIFF_FAC] = eeprom_read_word(&ee_MC_DIFF_FAC);
	
	//In case there is a weird value, reset to defaults.
	if(vsettings[SETTING_N_LIMIT] > 100) vsettings[SETTING_N_LIMIT] = 100;
	if(vsettings[SETTING_CUR_MAXPK] > 100) vsettings[SETTING_CUR_MAXPK] = 100;
	if(vsettings[SETTING_CUR_CONEFF] > 100) vsettings[SETTING_CUR_CONEFF] = 100;
	if(vsettings[SETTING_MAX_VAL] > 100) vsettings[SETTING_MAX_VAL] = 100;
	if(vsettings[SETTING_DIFF_FAC] > 100) vsettings[SETTING_DIFF_FAC] = 100;
	
	boot_count = eeprom_read_word(&ee_boot_count);
	eeprom_write_word(&ee_boot_count, boot_count+1);
	
	GAS1MIN = eeprom_read_word(&ee_Gas1_min);
	GAS1MAX = eeprom_read_word(&ee_Gas1_max);
	GAS2MIN = eeprom_read_word(&ee_Gas2_min);
	GAS2MAX = eeprom_read_word(&ee_Gas2_max);
	BRAKEMIN = eeprom_read_word(&ee_Brake_min);
	BRAKEMAX = eeprom_read_word(&ee_Brake_max);
	
	
	int_ADC();
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
