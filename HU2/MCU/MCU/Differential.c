/* DIFFERENTIAL.C
Some basic functions for calculating the electronic differential (i.e. torque vectoring)
 */ 

#include <math.h>
#include "Differential.h"

const double l2_over_w = (2 * 150) / 130;

//Steerangle should be positive when steering right, negative when steering left.
struct torques getDifferential(double Tmid, double steerpos)
{
	struct torques ret;
	
	//To save time when there is no throttle
	if(Tmid < 0.5 && Tmid > -0.5)
	{
		ret.left_perc = 0;
		ret.right_perc = 0;
		return ret;
	}
	
	double steerangle = steerpos * 0.008; //TODO: Calibrate this to get an accurate calculation
	
	//To not get stupid values
	if(steerangle < 0.01 && steerangle > -0.01)
	{
		ret.left_perc = Tmid;
		ret.right_perc = Tmid;
		return ret;
	}
	
	double c = tan(steerangle);
	
	//Boundary fixing for c. Don't want to get weird negative values. (though these should never happen anyway)
	if(c > l2_over_w - 0.1) c = l2_over_w - 0.1;
	if(c < -l2_over_w + 0.1) c = -l2_over_w + 0.1;
	
	//This is where the magic happens
	double mul_l = (l2_over_w + c) / (l2_over_w - c);
	ret.factor = mul_l;
	ret.left_perc = Tmid * mul_l;
	ret.right_perc = Tmid / mul_l;
	
	//Boundary fixing
	if(ret.left_perc > 100)
	{
		ret.right_perc *= (100 / ret.left_perc);
		ret.left_perc = 100;
	}
	if(ret.right_perc > 100)
	{
		ret.left_perc *= (100 / ret.right_perc);
		ret.right_perc = 100;
	}
	
	//If one of them hits zero or lower, stop.
	if(ret.left_perc <= 0 || ret.right_perc <= 0)
	{
		ret.right_perc = 0;
		ret.left_perc = 0;
	}
	
	return ret;
}

const double slack = 1.1;
struct slips detectSlip(double rpmleft, double rpmright, struct torques input)
{
	struct slips ret;
	if(rpmleft > 100 || rpmright > 100)
	{
		//Only check for slippage if the rear wheels are spinning at all
		ret.right = (rpmleft * slack < rpmright * input.factor * input.factor);
		ret.left = (rpmleft > rpmright * input.factor * input.factor * slack);
	}
	else
	{
		//If the rear wheels are not spinning, don't detect slip by default
		ret.right = 0;
		ret.left = 0;
	}
	return ret;
}


struct torques solveSlip(struct slips slip, struct torques input)
{
	//If neither are slipping, immediately return
	if(!slip.left && !slip.right) return input;
	
	//Set output torque to 0 if a wheel is slipping
	struct torques ret = input;
	if(slip.left) ret.left_perc = 0;
	if(slip.right) ret.right_perc = 0;
	
	return ret;
}