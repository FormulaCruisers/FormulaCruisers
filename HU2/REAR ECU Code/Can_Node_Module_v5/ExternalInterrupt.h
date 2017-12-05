/* EXTERNALINTERRUPT.H
Header file for ExternalInterrupt.c
*/

#ifndef _EXTERNAL_INTERRUPTh_
#define _EXTERNAL_INTERRUPTh_
#include <avr/io.h>

void int_ExternalInterrupt(void);

extern uint16_t pulsetime[4];

#endif