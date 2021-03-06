/* UI.H
Header file for UI.c
Contains the definitions for all screens(i.e. states)
*/

#ifndef _UI_H
#define _UI_H

#include "Defines.h"
#include "AMS.h"
#include <stdbool.h>

char Linebuffer[4][21];

enum uiscreen
{
	//Normal screens
	SCREEN_ANIMATION,
	SCREEN_WELCOME,
	SCREEN_START,
	SCREEN_PREDISCHARGING,
	SCREEN_STATUS,
	SCREEN_DRIVING,
	
	//Setup screens
	SCREEN_SETTINGS,
	SCREEN_SAVING,
	SCREEN_CALIBRATE,
	
	//Testing screens
	SCREEN_TEST,
	SCREEN_DRIVETEST,
	
	//Error screens
	SCREEN_ERROR,
};

volatile enum uiscreen ui_current_screen;
volatile enum _error _errorcode;

extern volatile uint16_t gas1, gas2, gas1perc, gas2perc, brakeperc, predistimer, flowleft, flowright, templeft, tempright, rpm_fl, rpm_fr, rpm_bl, rpm_br;
extern volatile uint8_t acctmp1, acctmp2;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
extern volatile uint8_t selsetting, ischanging, vsettings[SETTINGS_COUNT];
extern volatile double steerpos, velocity, accel_gforce, battery_voltage;
extern volatile int16_t ccurrent;
extern volatile uint8_t dt_engv;
extern uint16_t boot_count;
extern bool brakelighton;
extern uint8_t disable_motor_braking, allow_turning_on;

extern volatile uint8_t test_mob;
extern volatile uint64_t test_value;

extern volatile uint32_t debugval;

extern volatile AMS_OVERALL amsd_overall;
extern volatile AMS_VOLTAGE amsd_voltage;

void lcd_refresh(void);
void lcd_quickrefresh(void);
void change_screen(enum uiscreen s);
void get_screen(char buffer[4][21], enum uiscreen s);

#endif
