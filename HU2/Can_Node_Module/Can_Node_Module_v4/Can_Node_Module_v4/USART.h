#ifndef _USARTh_
#define _USARTh_
#include <avr/io.h>

void USARTInit(uint16_t ubrr_value);
char USARTReadChar( void );
void USARTWriteChar(char data);

#endif