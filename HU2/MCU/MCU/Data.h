#ifndef _DATA_H
#define _DATA_H

void data_send_ecu(uint8_t node, uint8_t data);
void wait_for_rx();

extern volatile int32_t gas1eng;
extern volatile uint16_t gas1, gas2, brake, gas1perc, gas2perc, brakeperc;
extern volatile uint16_t rpm_fl, rpm_fr, rpm_br, rpm_bl, steerpos, flowleft, flowright;
extern volatile uint8_t shutdownon, ams_shutdown, imd_shutdown;
extern volatile uint32_t ENGINE_MAX;

#endif
