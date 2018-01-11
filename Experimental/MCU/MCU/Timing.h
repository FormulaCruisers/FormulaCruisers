/*
 * Timing.h
 *
 * Created: 2017-10-26 14:41:19
 *  Author: Jeremy
 */ 


#ifndef TIMING_H_
#define TIMING_H_

//TODO: actually make this timer 1024 hz
#define _TM0						193 //1024 hz

#define TPER_1024HZ 0
#define TPER_128HZ 3
#define TPER_16HZ 6
#define TPER_1HZ 10

void init_timers();

#endif /* TIMING_H_ */