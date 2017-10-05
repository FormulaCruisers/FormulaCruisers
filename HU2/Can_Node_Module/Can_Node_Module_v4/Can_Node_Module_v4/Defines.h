#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1
#define NODEID2		0x202 // NODE ID 2
#define NODEID3		0x203 // NODE ID 3
#define NODEID4		0x204 // NODE ID 4
#define NODEID5		0x205 // NODE ID 5
#define NODEID6		0x206 // NODE ID 6

#define BAUD 500

#define FUNCTION NODEID4

#endif