#ifndef _UI_H
#define _UI_H

char Linebuffer[4][21];

enum uiscreen
{
	SCREEN_WELCOME,
	SCREEN_ERROR,
	SCREEN_PREDISCHARGING,
	SCREEN_START,
	SCREEN_DRIVING,
	SCREEN_STATUS
};

volatile enum uiscreen ui_current_screen;
volatile enum _error _errorcode;

extern volatile uint16_t gas1perc, gas2perc, brakeperc, predistimer, flowleft, flowright, rpm_fl, rpm_fr, steerpos, gas1, gas2, brake;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
void lcd_refresh();
void lcd_quickrefresh();
void change_screen(enum uiscreen s);
void get_screen(char buffer[4][21], enum uiscreen s);

#endif
