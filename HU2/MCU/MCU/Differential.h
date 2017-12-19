/* DIFFERENTIAL.H
Header file for Differential.c
 */ 

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

struct torques
{
	double left_perc;
	double right_perc;
};

struct torques getDifferential(double Tmid, double steerangle);

#endif /* DIFFERENTIAL_H_ */