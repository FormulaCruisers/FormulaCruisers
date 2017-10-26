#ifndef _Definesh_
#define _Definesh_
#include <avr/io.h>

#define F_CPU 16000000UL	// Define processor clock speed for compiler

#define SOFTWARE_VERSION			"Software: V0.02"

#define DATA_WAIT_TIMEOUT			0xFF //255 cycles
#define CANTIMEOUT					100	//100 ms

#define TRUE						0xFF
#define FALSE						0x00

#define MASTERID					0x0317

#endif