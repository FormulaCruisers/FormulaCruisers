/* CAN.H
Header file for CAN.c
*/

#ifndef _CANh_
#define _CANh_
#include <avr/io.h>

void can_init (uint16_t Baud);
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Address, uint8_t DLC);

extern uint8_t receive_data[8];
extern uint8_t transmit_data[8];

extern uint8_t R_L, R_H;

#endif