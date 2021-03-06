/* DEFINES.H
This is a central header file containing a lot of defined constants that are used in the program.
*/

#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL	// Define processor clock speed for compiler
#endif

#define SOFTWARE_VERSION			"Software: V2.200 @FS "  //Version should be 2.xxx where xxx is amount of commits(do "git rev-list HEAD --count" to check)

#define RTDS						PC7 //Loud ready beep
#define RTDS_TIME					1100 //1100 / 500 = 2.2 seconds    //Has to be between 1 and 3 seconds!

#define START_BRAKE_MINPERCENT		5	//Brake pedal has to be pushed at least 5% according to the calibration values to start the car
#define CUTPOWER_BRAKE_H			25	//Hard braking with APPS above 25% means that no more power should be sent to the motors
#define CUTPOWER_BRAKE_L			1	//After the above is triggered, only undo it once APPS goes below 5% and 0Nm desired torque, which actually means 0%
#define HARDBRAKING					50
#define DISCREPANCY_TICKS			17	//50 ticks at 500*3 hz is 105 ms

#define NODATA_TIMER				500 //1000ms without data from a CAN node means error

//Dashboard buttons and LEDs
#define BUTTON1						PD0
#define BUTTON2						PD1
#define BUTTONBLUE					PD2
#define BUTTONGREEN					PD3

#define DEBOUNCE_TIME				16 // 16*(1000/500) = 24 ms == Time to debounce a button
#define SHUTDOWN_TIME				64

#define _HIGH						0xFF
#define _LOW						0x00

#define PREDISCHARGE_TIMER			10000 //10 seconds

#define CANTIMEOUT					100	//100 ms
#define RX_WAIT_LIMIT				200 //200 cycles

#define SETTINGS_COUNT				5
#define SETTING_N_LIMIT		0
#define SETTING_CUR_MAXPK	1
#define SETTING_CUR_CONEFF	2
#define SETTING_MAX_VAL		3
#define SETTING_DIFF_FAC	4

#define ENGINE_MAX					0x7FFF //32767

//Brake light switch percentages
#define BL_SWITCHON					15
#define BL_SWITCHOFF				10

#define CALIB_SLACK					10
#define RANGESLACK					50

#define PUMP_TEMP_MAX				100
#define FLOWMIN						10

#define STEER_MAX					0x0DC
#define STEER_MIN					0x144
#define STEER_MIDDLE				(STEER_MAX+STEER_MIN)/2

//Log
#define LOG_DELAY					250 //250/500 = 0.5s per log

//Counter values
#define _TM0						100 //500 Hz
#define _TM2						192 //13.5 Hz?

//ADC defines
#define A_MCURRENT				0

//MOB defines
#define MOBCOUNT				5

#define MOB_STEERING_POS		0
#define MOB_RPM_FRONT_LEFT		1
#define MOB_RPM_FRONT_RIGHT		2

#define MOB_GAS1				0
#define MOB_GAS2				1
#define MOB_BRAKE				2

#define MOB_RPM_BACK_LEFT		0
#define MOB_FLOW_LEFT			1
#define MOB_TEMP_LEFT			2

#define MOB_RPM_BACK_RIGHT		0
#define MOB_FLOW_RIGHT			1
#define MOB_TEMP_RIGHT			2

#define MOB_ACCTEMP_MAX			0

#define MOB_SHUTDOWN			0

/*************** Node 1 ***************/
#define STEERING_POS				0x18 //ADC3
#define RPM_FRONT_LEFT				0x13 //PPS3
#define RPM_FRONT_RIGHT				0x12 //PPS2

/*************** Node 2 ***************/
#define GAS_1						0x2B //ADC2
#define GAS_2						0x2A //ADC1
#define BRAKE						0x29 //ADC0?

/*************** Node 3 ***************/
#define RPM_BACK_LEFT				0x30 //PPS0
#define FLOW_LEFT					0x32 //PPS2
#define TEMP_LEFT					0x39 //ADC1		//00a3 = 17 c

/*************** Node 4 ***************/
#define RPM_BACK_RIGHT				0x40 //PPS0
#define FLOW_RIGHT					0x42 //PPS2
#define TEMP_RIGHT					0x49 //ADC1		//0393 = 17 c


/*************** ECU 2  ***************/
// Data request
//#define RPM_BACK_LEFT				0x51
//#define RPM_BACK_RIGHT				0x52
#define SHUTDOWN					0x61

// Commands
#define RUN_ENABLE					0x55
#define MOTOR_CONTROLLER			0x56
#define BRAKELIGHT					0x57
#define PREDISCHARGE				0x58
#define PUMP_ENABLE					0x59
#define MAIN_RELAIS					0x60

#define IMDSHUTDOWN					0x99
#define AMSSHUTDOWN					0x98

// CAN node IDs
#define MASTERID	0x317 // Transmit Address Master
#define NODEID1		0x201 // NODE ID 1: Steering Node
#define NODEID2		0x202 // NODE ID 2: Pedalbox Node
#define NODEID3		0x203 // NODE ID 3: Coolant  Node L
#define NODEID4		0x204 // NODE ID 4: Coolant  Node R
#define ECU2ID		0x205 // NODE ID 5: ECU 2
#define MCDL		0x210 // Motor driver left
#define MCDR		0x211 // Motor driver right (Should get negative value to drive forward)

#define FUNCTION MASTERID



#endif