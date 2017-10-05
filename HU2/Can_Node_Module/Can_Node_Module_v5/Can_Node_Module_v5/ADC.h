#ifndef _ADCh_
#define _ADCh_
#include <avr/io.h>

void int_ADC(void);
void getADC(uint8_t ADC_pin);

extern uint16_t ADCValue[4];


#endif