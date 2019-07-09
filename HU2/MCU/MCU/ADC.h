/*
 * ADC.h
 *
 * Created: 03/07/2019 15:49:05
 *  Author: Jeremy
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>
#include "Defines.h"

void int_ADC(void);
void req_ADC(uint8_t pin);

extern volatile uint16_t mcurrent;




#endif /* ADC_H_ */