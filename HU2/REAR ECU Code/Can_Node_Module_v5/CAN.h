#ifndef _CANh_
#define _CANh_
#include <avr/io.h>
#include <stdbool.h>

void can_init (uint16_t Baud);
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Address, uint8_t DLC);

extern uint8_t ReceiveData[8];
extern uint8_t TransmitData[8];
extern uint32_t cantimer;
#endif