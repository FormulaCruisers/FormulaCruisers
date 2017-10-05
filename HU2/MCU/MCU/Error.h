#ifndef _ERROR_H
#define _ERROR_H

enum _error
{
	ERROR_NONE,				//When there isn't actually an error. Best case scenario

	ERROR_GAS_DISCREPANCY,	//When the two different gas sensors give wildly different values.
	ERROR_GASBRAKE,			//When both gas and brake are pressed at the same time(is this really an error?)

	ERROR_GAS1RANGE,		//When gas sensor 1 is far out of range
	ERROR_GAS2RANGE,		//When gas sensor 2 is far out of range
	ERROR_BRAKERANGE,		//When the brake sensor is far out of range
	
	ERROR_GAS1SENSOR,		//When gas sensor 1 is not giving a (correct) readout
	ERROR_GAS2SENSOR,		//When gas sensor 2 is not giving a (correct) readout
	ERROR_BRAKESENSOR,		//When the brake sensor is not giving a (correct) readout
	
	ERROR_PUMPFLOW,			//When the pump is on but there is no flow
	ERROR_PUMPTEMP,			//When the pump gets too hot
};

extern volatile uint16_t gas1, gas2, brake, gas1perc, gas2perc, brakeperc, flowleft, flowright;
extern volatile enum _error _errorcode;

char* get_error(enum _error e);
void e_checkranges();
void e_checkdiscrepancy();
void e_checksensors();
void e_checkflow();

#endif