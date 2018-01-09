/* UI.C
This file contains all of the code to do with displaying text on the LCD, using lcd.h
This includes all of the screen states.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Defines.h"
#include "lcd.h"
#include "UI.h"
#include "Error.h"
#include "sd_raw.h"

extern volatile uint8_t anim;	//Actual animation frame number
extern volatile uint8_t av;		//Normalized animation frame number

char* anim1 = "                      _    _                         ";
char* anim2 = "                      ]`../ |o_..__                  ";
char* anim3 = "                    `.,(_)-HU2--(_).>                ";
char* anim4 = "                    .`,(_)-HU2--(_).>                ";

char* fsettings[SETTINGS_COUNT] = {	"Speed limit %      ",
									"Max pkcurrent %    ",
									"Curr. con-eff %    ",
									"Max engine *1k(0-?)" };
char settingcursor[2] = "> ";	//Cursor for settings screen

extern volatile uint32_t rx_count, tx_count;

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
	
	//snprintf(Linebuffer[3], sizeof Linebuffer[3], "%8lu %8lu ", rx_count, tx_count);
	
	rx_count = 0;
	tx_count = 0;
	
	lcd_quickrefresh();
}

void lcd_quickrefresh()
{
	//Write the current line buffer into their respective LCD lines
	for(uint8_t y = 0; y < 4; y++)
	{
		lcd_gotoxy(0, y);
		lcd_puts(Linebuffer[y]);
#ifdef USE_SD_CARD
		sd_write(Linebuffer[y], 20);
		sd_write("0", 1);
#endif //USE_SD_CARD
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
		case SCREEN_ANIMATION:
			snprintf(buffer[0], sizeof buffer[0], "%s", &anim1[av]);
			snprintf(buffer[1], sizeof buffer[1], "%s", &anim2[av]);
			snprintf(buffer[2], sizeof buffer[2], "%s", (anim % 2 == 0) ? &anim3[av] : &anim4[av]);
			break;
		
		case SCREEN_WELCOME:
			snprintf(buffer[0], sizeof buffer[0], "        HU-2        ");
			snprintf(buffer[1], sizeof buffer[1], SOFTWARE_VERSION);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], " Press blue button  ");
			break;
			
		case SCREEN_START:
			snprintf(buffer[0], sizeof buffer[0], "Gas1:%3d%% Gas2:%3d%% ", gas1perc, gas2perc);
			snprintf(buffer[1], sizeof buffer[1], "Brake:%3d%%          ", brakeperc);
			snprintf(buffer[2], sizeof buffer[2], "Green = calibrate   ");
			snprintf(buffer[3], sizeof buffer[3], "%4d                ", (amsd_overall.N_Livecells_LSB + (amsd_overall.N_Livecells_MSB << 8)));
			//snprintf(buffer[3], sizeof buffer[3], "Press blue to begin ");
			break;

		case SCREEN_PREDISCHARGING:
			snprintf(buffer[0], sizeof buffer[0], " Precharging...     ");
			snprintf(buffer[1], sizeof buffer[1], " %5d              ", predistimer);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "Flow: L%5d  R%5d", flowleft, flowright);
			break;

		case SCREEN_STATUS:
			snprintf(buffer[0], sizeof buffer[0], "Temp: L%5u  R%5u", templeft, tempright);
			snprintf(buffer[1], sizeof buffer[1], "Rpm: FL%5u FR%5u", rpm_fl, rpm_fr);
			snprintf(buffer[2], sizeof buffer[2], "Gas1:%3u%% Gas2:%3u%% ", gas1perc, gas2perc);
			snprintf(buffer[3], sizeof buffer[3], "Steerpos: %5d     ", steerpos);
			break;

		case SCREEN_DRIVING:
			snprintf(buffer[0], sizeof buffer[0], "Gas1:%3d%% Gas2:%3d%% ", gas1perc, gas2perc);
			snprintf(buffer[1], sizeof buffer[1], "Brake:%3d%%          ", brakeperc);
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "Battery: NAN%%       ");
			break;
			
		
		
		
		
		

		case SCREEN_SETTINGS:
			snprintf(buffer[0], sizeof buffer[0], "%c%s", settingcursor[ischanging], fsettings[selsetting]);
			snprintf(buffer[1], sizeof buffer[1], "%c%5d             ", settingcursor[1-ischanging], vsettings[selsetting]);
			snprintf(buffer[2], sizeof buffer[2], "Blue = edit setting ");
			snprintf(buffer[3], sizeof buffer[3], "Green= save and exit");
			break;
			
		case SCREEN_SAVING:
			snprintf(buffer[0], sizeof buffer[0], "Applying settings...");
			snprintf(buffer[1], sizeof buffer[1], "                    ");
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "%5d               ", boot_count);
			break;
		
		case SCREEN_CALIBRATE:
			snprintf(buffer[0], sizeof buffer[0], "Press both pedals to");
			snprintf(buffer[1], sizeof buffer[1], "their maximum       ");
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "Then, press green   ");
			break;
		
		
		
		
		
		
		
			
		case SCREEN_TEST:
			snprintf(buffer[0], sizeof buffer[0], "Read from:0x%02x      ", test_sensor);
			snprintf(buffer[1], sizeof buffer[1], "Raw value:0x%08lx", test_value);
			snprintf(buffer[2], sizeof buffer[2], "Blue cycles nodes   ");
			snprintf(buffer[3], sizeof buffer[3], "Green closes menu   ");
			break;
			
		case SCREEN_DRIVETEST:
			snprintf(buffer[0], sizeof buffer[0], "Engine: %3d%%        ", dt_engv);
			snprintf(buffer[1], sizeof buffer[1], "Temp: L%5u  R%5u", templeft, tempright);
			snprintf(buffer[2], sizeof buffer[2], "Blue to stop motor  ");
			snprintf(buffer[3], sizeof buffer[3], "Green to apply      ");
			break;
			
		case SCREEN_PUMPTEST:
			snprintf(buffer[0], sizeof buffer[0], "Pump: %3d           ", pump_pwm);
			snprintf(buffer[1], sizeof buffer[1], "                    ");
			snprintf(buffer[2], sizeof buffer[2], "Blue to set to 0    ");
			snprintf(buffer[3], sizeof buffer[3], "Green to close      ");
			break;
			
			
			
			
			
			
			
			
			
			
			
		case SCREEN_ERROR:
			snprintf(buffer[0], sizeof buffer[0], "     !!!ERROR!!!    ");
			snprintf(buffer[1], sizeof buffer[1], "%20s", get_error(_errorcode));
			snprintf(buffer[2], sizeof buffer[2], "                    ");
			snprintf(buffer[3], sizeof buffer[3], "                    ");
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
