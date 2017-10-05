#ifndef _EXTERNAL_INTERRUPTh_
#define _EXTERNAL_INTERRUPTh_
#include <avr/io.h>

void int_ExternalInterrupt(void);

extern uint16_t PulsePerSec[4];
extern uint8_t Direction[2];

uint8_t InterruptPairDirection[2];
uint16_t InterruptPairTimer[2];

#endif