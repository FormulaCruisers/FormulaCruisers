#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define BAUD 500

#define CANTIMEOUT					100	// in milliseconds

#define _RIGHT				1
#define _LEFT				2
#define PD_SHUTDOWN			PD7

/*************** Node 1 ***************/
#define STUURPOSITIE				0x11
#define RPM_VOOR_LINKS				0x12
#define RPM_VOOR_RECHTS				0x13

/*************** Node 2 ***************/
#define GAS_1						0x21
#define GAS_2						0x22
#define REM							0x23

/*************** Node 3 ***************/
#define FLOW_RICHTING_LINKS			0x31
#define TEMP_LINKS					0x32

/*************** Node 4 ***************/
#define FLOW_RICHTING_RECHTS		0x41
#define TEMP_RECHTS					0x42

/*************** EcU 2 ***************/
// Data request
#define RPM_LINKS_ACHTER			0x51
#define RPM_RECHTS_ACHTER			0x52
#define DRAAIRICHTING_LINKS_ACHTER	0x53
#define DRAAIRICHTING_RECHTS_ACHTER 0x54
#define SHUTDOWN					0x61

// Commando's
#define RUN_ENABLE					0x55
#define MOTOR_CONTROLLER			0x56
#define BRAKELIGHT					0x57
#define PRE_DISCHARGE				0x58
#define PUMP						0x59
#define MAINRELAIS					0x60

#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1	Steering Node
#define NODEID2		0x202 // NODE ID 2	Pedalbox Node
#define NODEID3		0x203 // NODE ID 3	Coolant  Node L
#define NODEID4		0x204 // NODE ID 4	Coolant  Node R
#define ECU2ID		0x205 // NODE ID 5  ECU 2
#define MCDL		0x210
#define MCDR		0x211

#define FUNCTION ECU2ID



#endif