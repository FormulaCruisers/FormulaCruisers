#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define SOFTWARE_VERSION			"Software: V2.059    "  //Version should be 2.xxx where xxx is amount of commits(do "git rev-list HEAD --count" to check)

#define RTDS						PC7 //Loud ready beep
#define RTDS_TIME					2000 //2 seconds

//Dashboard buttons and LEDs
#define BUTTON1						PD0
#define BUTTON2						PD1
#define BUTTONBLUE					PD2
#define BUTTONGREEN					PD3
#define IMDLED						PE7
#define AMSLED						PE5

#define DEBOUNCE_TIME				8 // 8*(1000/500) = 16 ms == Time to debounce a button

#define _HIGH						0xFF
#define _LOW						0x00

#define PREDISCHARGE_TIMER			5000 //10 seconds

#define CANTIMEOUT					100	//100 ms
#define RX_WAIT_LIMIT				20000 //20000 cycles

#define SETTINGS_COUNT				4

#define ENGINE_MAX					0x7FFF //32767

//Brake light switch percentages
#define BL_SWITCHON					15
#define BL_SWITCHOFF				10

//Pedal min and max values(with some slack)
#define GAS1MAX						777
#define GAS1MIN						693
#define GAS2MAX						636
#define GAS2MIN						545
#define BRAKEMAX					100
#define BRAKEMIN					21
#define RANGESLACK					20

#define PUMP_TEMP_MAX				100
#define FLOWMIN						10

#define STEER_MAX					349
#define STEER_MIN					117
#define STEER_MIDDLE				(STEER_MAX+STEER_MIN)/2

//Counter values
#define _TM0						100 //500 Hz
#define _TM2						192 //8 Hz

/*************** Node 1 ***************/
#define STEERING_POS				0x11
#define RPM_FRONT_LEFT				0x12
#define RPM_FRONT_RIGHT				0x13

/*************** Node 2 ***************/
#define GAS_1						0x21
#define GAS_2						0x22
#define BRAKE						0x23

/*************** Node 3 ***************/
#define FLOW_LEFT					0x31
#define TEMP_LEFT					0x32

/*************** Node 4 ***************/
#define FLOW_RIGHT					0x41
#define TEMP_RIGHT					0x42

/*************** ECU 2  ***************/
// Data request
#define RPM_BACK_LEFT				0x51
#define RPM_BACK_RIGHT				0x52
#define WHEELDIR_BACK_LEFT			0x53
#define WHEELDIR_BACK_RIGHT			0x54
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

/*********** Motor controller *********/
// Turning the wheel
#define MC_SET_SPEED					0x31		//Value between 0x0000 - 0x7FFF
#define MC_SET_TORQUE					0x90		//Value between 0x0000 - 0x7FFF

// Settings
#define MC_N_LIMIT						0x34		//Value between 0x0000 - 0x7FFF
#define MC_CURRENT_MAXPK				0xC4		//Value between 0x0000 - 0x3FFF
#define MC_CURRENT_CONEFF				0xC5		//Value between 0x0000 - 0x3FFF


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