/* DIFFERENTIAL.C
Some basic functions for calculating the electronic differential
 */ 

#include <math.h>
#include "Differential.h"

const double l2_over_w = 3;

//Steerangle should be positive when steering right, negative when steering left. It should also be in radians.
struct torques getTorques(double Tmid, double steerangle)
{
	struct torques ret;
	
	//To save time when there is no throttle
	if(Tmid < 0.5 && Tmid > -0.5)
	{
		ret.left_perc = 0;
		ret.right_perc = 0;
		return ret;
	}
	
	//To not get stupid values
	if(steerangle < 0.01 && steerangle > -0.01)
	{
		ret.left_perc = Tmid;
		ret.right_perc = Tmid;
		return ret;
	}
	
	double c = tan(steerangle);
	
	//Boundary fixing for c. Dont want to get weird negative values. (though these should never happen anyway)
	if(c > l2_over_w - 0.1) c = l2_over_w - 0.1;
	if(c < -l2_over_w + 0.1) c = -l2_over_w + 0.1;
	
	//This is where the magic happens
	double mul_l = (l2_over_w + c) / (l2_over_w - c);
	ret.left_perc = Tmid * mul_l;
	ret.right_perc = Tmid / mul_l;
	
	//Boundary fixing
	if(ret.left_perc > 100)
	{
		ret.right_perc -= ret.left_perc - 100;
		ret.left_perc = 100;
	}
	if(ret.right_perc > 100)
	{
		ret.left_perc -= ret.right_perc - 100;
		ret.right_perc = 100;
	}
	if(ret.left_perc < 0)
	{
		ret.right_perc -= ret.left_perc;
		ret.left_perc = 0;
	}
	if(ret.right_perc < 0)
	{
		ret.left_perc -= ret.right_perc;
		ret.right_perc = 0;
	}
	
	return ret;
}