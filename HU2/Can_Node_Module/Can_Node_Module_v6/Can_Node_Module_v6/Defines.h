/* DEFINES.H
This is a central header file containing a lot of defined constants that are used in the program.
*/

#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define BAUD 500

#define MASTERID	0x317 // Transmit Adress Master
#define NODEID1		0x201 // NODE ID 1	Steering Node
#define NODEID2		0x202 // NODE ID 2	Pedalbox Node
#define ACCTMPNODE1	0x206 // NODE ID accumulator temperature 1
#define ACCTMPNODE2	0x207 // NODE ID accumulator temperature 2
#define MCDL		0x210
#define MCDR		0x211

#define FUNCTION NODEID2

#endif