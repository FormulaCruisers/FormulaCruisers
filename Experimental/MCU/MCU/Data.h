#ifndef _DATA_H
#define _DATA_H

void log_data(uint8_t node, uint16_t val);
void log_print(char* str);
void log_println(char* str);
void data_request(uint16_t node, uint8_t num, uint16_t* outputvar, uint8_t wait);
void data_send_ecu(uint8_t node, uint8_t data, uint8_t wait);

#endif
