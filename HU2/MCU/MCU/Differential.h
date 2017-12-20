/* DIFFERENTIAL.H
Header file for Differential.c
 */ 

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

#include <avr/io.h>

struct torques
{
	double left_perc;
	double right_perc;
	double factor;
};

struct slips
{
	uint8_t left;
	uint8_t right;
};

struct torques getDifferential(double Tmid, double steerangle);
struct slips detectSlip(double rpmleft, double rpmright, struct torques input);
struct torques solveSlip(struct slips slip, struct torques input);

#endif /* DIFFERENTIAL_H_ */