#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define BAUD 500

#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1	Steering Node
#define NODEID2		0x202 // NODE ID 2	Pedalbox Node
#define NODEID3		0x203 // NODE ID 3	Coolant  Node L
#define NODEID4		0x204 // NODE ID 4	Coolant  Node R
#define ECU2ID		0x205 // NODE ID 5  ECU 2
#define MCDL		0x210
#define MCDR		0x211

#define FUNCTION NODEID1



#endif