/* CAN.H
Header file for CAN.c
*/

#define CAN_REQUEST_DATA	0x3D

#ifndef _CANh_
#define _CANh_
#include <avr/io.h>
#include <stdbool.h>

void can_init (uint16_t Baud);
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Address, uint8_t DLC);

extern uint8_t receive_data[8];
extern uint8_t transmit_data[8];
extern uint8_t pump_pwm;
uint32_t cantimer;
#endif