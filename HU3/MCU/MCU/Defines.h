#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define SOFTWARE_VERSION			"Software: V0.01"

#define DATA_WAIT_TIMEOUT			0xFF //255 cycles
#define CANTIMEOUT					100	//100 ms

//Counter values
#define _TM0						193 //1 kHz
#define _TM2						192 //8 Hz

#define TRUE						0xFF
#define FALSE						0x00


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


// CAN node IDs
#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1: Steering Node
#define NODEID2		0x202 // NODE ID 2: Pedalbox Node
#define NODEID3		0x203 // NODE ID 3: Coolant  Node L
#define NODEID4		0x204 // NODE ID 4: Coolant  Node R
#define ECU2ID		0x205 // NODE ID 5: ECU 2

#define MCDR		0x211 // Motor driver right
#define MCDL		0x210 // Motor driver left

#define FUNCTION MASTERID



#endif