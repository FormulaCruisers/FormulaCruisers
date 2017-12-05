#ifndef _UI_H
#define _UI_H

#include "Defines.h"

char Linebuffer[4][21];

enum uiscreen
{
	SCREEN_WELCOME,
	SCREEN_ERROR,
	SCREEN_PREDISCHARGING,
	SCREEN_START,
	SCREEN_DRIVING,
	SCREEN_STATUS,
	SCREEN_ANIMATION,
	
	SCREEN_SETTINGS,
	SCREEN_SAVING,
	
	SCREEN_TEST,
	SCREEN_DRIVETEST,
};

volatile enum uiscreen ui_current_screen;
volatile enum _error _errorcode;

extern volatile uint16_t gas1perc, gas2perc, brakeperc, predistimer, flowleft, flowright, templeft, tempright, rpm_fl, rpm_fr, rpm_bl, rpm_br;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
extern volatile uint8_t selsetting, ischanging, vsettings[SETTINGS_COUNT];
extern volatile int16_t steerpos;
extern volatile uint8_t dt_engv;
extern uint16_t boot_count;

extern volatile uint8_t test_sensor;
extern volatile uint32_t test_value;

void lcd_refresh();
void lcd_quickrefresh();
void change_screen(enum uiscreen s);
void get_screen(char buffer[4][21], enum uiscreen s);

#endif
