#ifndef _CAN_H
#define _CAN_H

#define CAN_REQUEST_DATA	0x3D
#define CAN_SEND_DATA		0x31

#include <avr/io.h>

void data_send_arr(uint8_t header, uint8_t buffer[], uint16_t node, uint8_t bufferlen);
void data_send8(uint8_t header, uint8_t data, uint16_t node);
void data_send16(uint8_t header, uint16_t data, uint16_t node);
void data_send_arr_nh(uint8_t buffer[], uint16_t node, uint8_t bufferlen);
void data_send8_nh(uint8_t data, uint16_t node);
void data_send16_nh(uint16_t data, uint16_t node);

void can_init();
void can_rx(uint16_t NODE_ID);
void can_tx(uint16_t Address, uint8_t DLC);

uint8_t ReceiveData[8];
uint8_t TransmitData[8];

#endif
