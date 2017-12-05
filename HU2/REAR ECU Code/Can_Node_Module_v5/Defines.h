/* DEFINES.H
This is a central header file containing a lot of defined constants that are used in the program.
*/

#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define BAUD 500

#define CANTIMEOUT					100	// in milliseconds

#define _RIGHT				1
#define _LEFT				2
#define PD_SHUTDOWN			PD7

#define PREDISCHARGE_TIMER			10000 //10 seconds

/*************** EcU 2 ***************/
// Data request
#define RPM_LINKS_ACHTER			0x51
#define RPM_RECHTS_ACHTER			0x52
#define SHUTDOWN					0x61

// Commando's
#define RUN_ENABLE					0x55
#define MOTOR_CONTROLLER			0x56
#define BRAKELIGHT					0x57
#define PRE_DISCHARGE				0x58
#define PUMP						0x59
#define MAINRELAIS					0x60

#define IMDSHUTDOWN					0x99
#define AMSSHUTDOWN					0x98

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