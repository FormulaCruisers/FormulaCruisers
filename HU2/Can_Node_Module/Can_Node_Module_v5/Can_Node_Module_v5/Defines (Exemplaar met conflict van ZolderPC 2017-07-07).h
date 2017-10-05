#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define BAUD 500

/*************** Node 1 ***************/
#define STUURPOSITIE				0x09
#define RPM_VOOR_LINKS				0x10
#define RPM_VOOR_RECHTS				0x11

/*************** Node 2 ***************/
#define GAS_1						0x14
#define GAS_2						0x15
#define REM							0x16

/*************** Node 3 ***************/
#define FLOW_RICHTING_LINKS			0x05
#define TEMP_LINKS					0x07

/*************** Node 4 ***************/
#define FLOW_RICHTING_RECHTS		0x06
#define TEMP_RECHTS					0x08

/*************** Node 5 ***************/
#define RPM_LINKS_ACHTER			0x01
#define RPM_RECHTS_ACHTER			0x02
#define DRAAIRICHTING_LINKS_ACHTER	0x03
#define DRAAIRICHTING_RECHTS_ACHTER 0x04


#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1	Steering Node
#define NODEID2		0x202 // NODE ID 2	Pedalbox Node
#define NODEID3		0x203 // NODE ID 3	Coolant  Node L
#define NODEID4		0x204 // NODE ID 4	Coolant  Node R
#define NODEID5		0x205 // NODE ID 5  ECU 2

#define FUNCTION NODEID2


#endif