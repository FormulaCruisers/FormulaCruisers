#ifndef _DATA_H
#define _DATA_H
#include "UI.h"

void data_send_ecu(uint8_t node, uint8_t data);
void data_send_ecu_a(uint8_t count, uint8_t ndarr[]);
void data_send_motor(uint8_t header, uint8_t data, int32_t mul, uint16_t node);
void data_send_motor_d(uint8_t header, double data, int32_t mul, uint16_t node);
void wait_for_rx();

extern volatile double gas1eng;
extern volatile uint16_t gas1, gas2, brake, gas1perc, gas2perc, brakeperc;
extern volatile uint16_t rpm_fl, rpm_fr, rpm_br, rpm_bl, flowleft, flowright;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
extern volatile uint32_t engine_max_perc;
extern volatile int16_t steerpos;
extern volatile uint8_t test_sensor;
extern volatile uint32_t test_value;

extern volatile enum _error _errorcode;
extern volatile enum uiscreen ui_current_screen;

#endif
