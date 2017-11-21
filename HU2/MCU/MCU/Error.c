#include "Defines.h"
#include "Error.h"

char* get_error(enum _error e)
{
	switch(e)
	{
		case ERROR_NONE:		return "No error";

		case ERROR_GAS_DISCREPANCY:	return "Gas discrepancy";
		case ERROR_GASBRAKE:		return "Gas & brake pressed";

		case ERROR_GAS1RANGE:		return "Gas 1 out of range";
		case ERROR_GAS2RANGE:		return "Gas 2 out of range";
		case ERROR_BRAKERANGE:		return "Brake out of range";

		case ERROR_GAS1SENSOR:		return "Gas 1 sensor faulty";
		case ERROR_GAS2SENSOR:		return "Gas 2 sensor faulty";
		case ERROR_BRAKESENSOR:		return "Brake sensor faulty";
		
		case ERROR_PUMPFLOW:		return "No pump water flow";
		case ERROR_PUMPTEMP:		return "Pump temp too high";
		
		case ERROR_SHUTDOWN:		return "Shutdown!";
		
		case ERROR_CAN_BIT:			return "CAN bit tx error";
		case ERROR_CAN_STUFF:		return "CAN stuffing error";
		case ERROR_CAN_CRC:			return "CAN CRC check error";
		case ERROR_CAN_FORM:		return "CAN fixed form error";
		case ERROR_CAN_ACK:			return "CAN no ACK error";
	
		case ERROR_UNKNOWN:
		default:			return "Unknown error?!";
	}
}

void e_checkranges()
{
	if(gas1 < GAS1MIN - RANGESLACK || gas1 > GAS1MAX + RANGESLACK) _errorcode = ERROR_GAS1RANGE;
	if(gas2 < GAS2MIN - RANGESLACK || gas2 > GAS2MAX + RANGESLACK) _errorcode = ERROR_GAS2RANGE;
	//if(brake < BRAKEMAX - RANGESLACK || brake > BRAKEMAX + RANGESLACK) _errorcode = ERROR_BRAKERANGE;
}

void e_checkdiscrepancy()
{
	int8_t dif = gas1perc - gas2perc;
	if(dif < -10 || dif > 10) _errorcode = ERROR_GAS_DISCREPANCY;
}

void e_checksensors()
{
	if(gas1 == 0x0000 || gas1 == 0xFFFF) _errorcode = ERROR_GAS1SENSOR;
	if(gas2 == 0x0000 || gas2 == 0xFFFF) _errorcode = ERROR_GAS2SENSOR;
	if(brake == 0x0000 || brake == 0xFFFF) _errorcode = ERROR_BRAKESENSOR;
}

void e_checkflow()
{
	if(flowleft < FLOWMIN || flowright < FLOWMIN) _errorcode = ERROR_PUMPFLOW;
}

void e_checkCAN()
{
	if(CANSTMOB & (1<<BERR)) _errorcode = ERROR_CAN_BIT;
	if(CANSTMOB & (1<<SERR)) _errorcode = ERROR_CAN_STUFF;
	if(CANSTMOB & (1<<CERR)) _errorcode = ERROR_CAN_CRC;
	if(CANSTMOB & (1<<FERR)) _errorcode = ERROR_CAN_FORM;
	if(CANSTMOB & (1<<AERR)) _errorcode = ERROR_CAN_ACK;
}