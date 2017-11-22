#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Defines.h"
#include "lcd.h"
#include "UI.h"
#include "Error.h"

extern volatile uint8_t anim;	//Actual animation frame number
extern volatile uint8_t av;		//Normalized animation frame number

char* anim1 = "                      _    _                         ";
char* anim2 = "                      ]`../ |o_..__                  ";
char* anim3 = "                    `.,(_)______(_).>                ";
char* anim4 = "                    .`,(_)______(_).>                ";

char* fsettings[SETTINGS_COUNT] = {	"Speed limit %      ",
									"Max pkcurrent %    ",
									"Curr. con-eff %    ",
									"Max engine *1k(0-?)" };
char settingcursor[2] = "> ";	//Cursor for settings screen

ISR(TIMER2_OVF_vect) //8 Hz
{
	//Full refresh LCD
	lcd_refresh();
}

void lcd_refresh()
{
	TCNT2 = _TM2;
	get_screen(Linebuffer, ui_current_screen);

	/*	
	if(shutdownon)
	{
		Linebuffer[2][0] = 'S';
	}
	if(ams_shutdown)
	{
		Linebuffer[2][1] = 'A';
	}
	if(imd_shutdown)
	{
		Linebuffer[2][2] = 'I';
	}//*/
	
	lcd_quickrefresh();
}

void lcd_quickrefresh()
{
	//Write the current line buffer into their respective LCD lines
	for(uint8_t y = 0; y < 4; y++)
	{
		lcd_gotoxy(0, y);
		lcd_puts(Linebuffer[y]);
	}
}

void change_screen(enum uiscreen s)
{
	ui_current_screen = s;
	lcd_refresh();	
}

void get_screen(char buffer[4][21], enum uiscreen s)
{
	switch(s)
	{
		case SCREEN_WELCOME:
			snprintf(buffer[0], sizeof buffer[0], "        HU-2        ");
			snprintf(buffer[1], sizeof buffer[1], SOFTWARE_VERSION);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], " Press blue button  ");
			break;

		case SCREEN_ERROR:
			snprintf(buffer[0], sizeof buffer[0], "     !!!ERROR!!!    ");
			snprintf(buffer[1], sizeof buffer[1], "%20s", get_error(_errorcode));
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "                    ");
			break;

		case SCREEN_PREDISCHARGING:
			snprintf(buffer[0], sizeof buffer[0], " Precharging...     ");
			snprintf(buffer[1], sizeof buffer[1], " %5d              ", predistimer);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "Flow: L%5d  R%5d", flowleft, flowright);
			break;

		case SCREEN_START:
			snprintf(buffer[0], sizeof buffer[0], "Gas1:%3d%% Gas2:%3d%% ", gas1perc, gas2perc);
			snprintf(buffer[1], sizeof buffer[1], "Brake:%3d%%          ", brakeperc);
			snprintf(buffer[2], sizeof buffer[2], "Steerpos:%5d      ", steerpos);
			snprintf(buffer[3], sizeof buffer[3], "Rpm: FL%5d FR%5d", rpm_fl, rpm_fr);
			//snprintf(buffer[3], sizeof buffer[3], "Press blue to begin ");
			break;

		case SCREEN_DRIVING:
			snprintf(buffer[0], sizeof buffer[0], "Gas1:%3d%% Gas2:%3d%% ", gas1perc, gas2perc);
			snprintf(buffer[1], sizeof buffer[1], "Brake:%3d%%          ", brakeperc);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "                    ");
			break;

		case SCREEN_STATUS:
			snprintf(buffer[0], sizeof buffer[0], "Flow: L%5d  R%5d", flowleft, flowright);
			snprintf(buffer[1], sizeof buffer[1], "Rpm: FL%5d FR%5d", rpm_fl, rpm_fr);
			snprintf(buffer[2], sizeof buffer[2], "Gas1:%3d%% Gas2:%3d%% ", gas1perc, gas2perc);
			snprintf(buffer[3], sizeof buffer[3], "Steerpos: %5d     ", steerpos);
			break;
			
		case SCREEN_SAVING:
			snprintf(buffer[0], sizeof buffer[0], "Applying settings...");
			snprintf(buffer[1], sizeof buffer[1], "                    ");
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "                    ");
			break;
			
		case SCREEN_ANIMATION:
			snprintf(buffer[0], sizeof buffer[0], "%s", &anim1[av]);
			snprintf(buffer[1], sizeof buffer[1], "%s", &anim2[av]);
			snprintf(buffer[2], sizeof buffer[2], "%s", (anim % 2 == 0) ? &anim3[av] : &anim4[av]);
			break;

		case SCREEN_SETTINGS:
			snprintf(buffer[0], sizeof buffer[0], "%c%s", settingcursor[ischanging], fsettings[selsetting]);
			snprintf(buffer[1], sizeof buffer[1], "%c%5d             ", settingcursor[1-ischanging], vsettings[selsetting]);
			snprintf(buffer[2], sizeof buffer[2], "Blue = edit setting ");
			snprintf(buffer[3], sizeof buffer[3], "Green= save and exit");
			break;
			
		case SCREEN_TEST:
			snprintf(buffer[0], sizeof buffer[0], "Read from:0x%02x      ", test_sensor);
			snprintf(buffer[1], sizeof buffer[1], "Raw value:0x%08x", test_value);
			snprintf(buffer[2], sizeof buffer[2], "Blue cycles nodes   ");
			snprintf(buffer[3], sizeof buffer[3], "Green closes menu   ");
			break;

		default:
			snprintf(buffer[0], sizeof buffer[0], " Unknown screen ID  ");
			snprintf(buffer[1], sizeof buffer[1], "                    ");
			snprintf(buffer[2], sizeof buffer[2], " Tell Jeremy to fix ");
			snprintf(buffer[3], sizeof buffer[3], " his stuff          ");
			break;
			
		/*
		case SCREEN_:
			snprintf(buffer[0], sizeof buffer[0], "                    ");
			snprintf(buffer[1], sizeof buffer[1], "                    ");
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "                    ");
			break;
		*/
	}
}
