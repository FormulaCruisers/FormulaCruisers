/* DATA.H
Header file for Data.c
*/

#ifndef _DATA_H
#define _DATA_H
#include "UI.h"
#include "Error.h"

void data_send_ecu(uint8_t node, uint8_t data);
void data_send_ecu_a(uint8_t count, uint8_t ndarr[]);
void data_send_motor(uint8_t header, uint8_t data, int32_t mul, uint16_t node);
void data_send_motor_d(uint8_t header, double data, int32_t mul, uint16_t node);

uint16_t g(uint8_t node, uint8_t val);
void selectmob(uint8_t node);
uint64_t getrawmob(uint8_t node);
uint16_t getonmob(uint8_t num);
void waitonmob(uint8_t num)

extern volatile double gas1eng;
extern volatile uint16_t gas1, gas2, brake, gas1perc, gas2perc, brakeperc;
extern volatile uint16_t rpm_fl, rpm_fr, rpm_br, rpm_bl, flowleft, flowright, templeft, tempright;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
extern volatile uint32_t engine_max_perc;
extern volatile int16_t steerpos;
extern volatile uint8_t test_mob;
extern volatile uint64_t test_value;

extern uint16_t GAS1MIN, GAS1MAX, GAS2MIN, GAS2MAX, BRAKEMIN, BRAKEMAX;

extern volatile enum _error _errorcode;
extern volatile enum uiscreen ui_current_screen;

extern volatile AMS_OVERALL amsd_overall;
extern volatile AMS_DIAGNOSTIC amsd_diagnostic;
extern volatile AMS_VOLTAGE amsd_voltage;
extern volatile AMS_CELL_MODULE_TEMP amsd_cell_module_temp;
extern volatile AMS_CELL_TEMP amsd_cell_temp;
extern volatile AMS_CELL_BALANCING amsd_cell_balancing;
//extern volatile struct AMS_CONFIGURATION amsd_configuration;
//extern volatile struct AMS_LOGIN amsd_login;

#endif
