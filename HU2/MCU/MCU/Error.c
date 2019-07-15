/* ERROR.C
This file contains basic error handling functions and error checking functions.
*/

#include "Defines.h"
#include "Error.h"

uint8_t disctimer = 0;
uint8_t cantimer = 0

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
		case ERROR_STEERRANGE:		return "Steer out of range";

		case ERROR_GAS1SENSOR:		return "Gas 1 sensor faulty";
		case ERROR_GAS2SENSOR:		return "Gas 2 sensor faulty";
		case ERROR_BRAKESENSOR:		return "Brake sensor faulty";
		case ERROR_STEERSENSOR:		return "Steer sensor faulty";
		
		case ERROR_PUMPFLOW:		return "No pump water flow";
		case ERROR_PUMPTEMP:		return "Pump temp too high";
		
		case ERROR_SHUTDOWN:		return "Shutdown!";
		
		case ERROR_CAN_BIT:			return "CAN bit tx error";
		case ERROR_CAN_STUFF:		return "CAN stuffing error";
		case ERROR_CAN_CRC:			return "CAN CRC check error";
		case ERROR_CAN_FORM:		return "CAN fixed form error";
		case ERROR_CAN_ACK:			return "CAN no ACK error";

		case ERROR_SD_INIT_RESET:	return "SD Not Reset error";
		case ERROR_SD_INIT_READY:	return "SD Not Ready error";
		case ERROR_SD_BLOCK:		return "SD Block error";
		case ERROR_SD_NOTREADY:		return "SD not ready";
		case ERROR_SD_READ:			return "SD Read error";
		case ERROR_SD_WRITE:		return "SD Write error";
		case ERROR_SD_SIZE:			return "SD Size error";
		
		case ERROR_SD_WRITE_IDLE:		return "SD Write Idle";
		case ERROR_SD_WRITE_ERASE_RST:	return "SD Write Erase Reset";
		case ERROR_SD_WRITE_ILLEGAL:	return "SD Write Illegal Cmd";
		case ERROR_SD_WRITE_CRC:		return "SD Write Command CRC";
		case ERROR_SD_WRITE_ERASE_SEQ:	return "SD Write Erase Sequence";
		case ERROR_SD_WRITE_ADDRESS:	return "SD Write Address";
		case ERROR_SD_WRITE_PARAMETER:	return "SD Write Parameter";
	
		case ERROR_UNKNOWN:
		default:			return "Unknown error?!";
	}
}

void e_checkranges()
{
	if(gas1 < GAS1MIN - RANGESLACK || gas1 > GAS1MAX + RANGESLACK) _errorcode = ERROR_GAS1RANGE;
	if(gas2 < GAS2MIN - RANGESLACK || gas2 > GAS2MAX + RANGESLACK) _errorcode = ERROR_GAS2RANGE;
	if(brake < BRAKEMAX - RANGESLACK || brake > BRAKEMAX + RANGESLACK) _errorcode = ERROR_BRAKERANGE;
	if(steerposm < STEER_MIN - RANGESLACK || steerposm > STEER_MAX + RANGESLACK) _errorcode = ERROR_BRAKERANGE;
}

void e_checkdiscrepancy()
{
	int8_t dif = gas1perc - gas2perc;
	if(dif < -10 || dif > 10)
	{
		//as per regulations, error should be raised when there is a discrepancy of more than 10% for more than 100ms continuously
		if(disctimer <= DISCREPANCY_TICKS) disctimer++;
	}
	else
	{
		disctimer = 0;
	}
	if(disctimer >= DISCREPANCY_TICKS) _errorcode = ERROR_GAS_DISCREPANCY;
}

void e_checksensors()
{
	if(gas1 == 0x0000 || gas1 == 0xFFFF) _errorcode = ERROR_GAS1SENSOR;
	if(gas2 == 0x0000 || gas2 == 0xFFFF) _errorcode = ERROR_GAS2SENSOR;
	if(brake == 0x0000 || brake == 0xFFFF) _errorcode = ERROR_BRAKESENSOR;
	if(steerposm == 0x0000 || steerposm == 0xFFFF) _errorcode = ERROR_STEERSENSOR;
}

void e_checkflow()
{
	if(flowleft < FLOWMIN || flowright < FLOWMIN) _errorcode = ERROR_PUMPFLOW;
}

void e_checkCAN()
{
	_error t_errorcode = ERROR_NONE;
	if(CANSTMOB & (1<<BERR)) t_errorcode = ERROR_CAN_BIT;
	if(CANSTMOB & (1<<SERR)) t_errorcode = ERROR_CAN_STUFF;
	if(CANSTMOB & (1<<CERR)) t_errorcode = ERROR_CAN_CRC;
	if(CANSTMOB & (1<<FERR)) t_errorcode = ERROR_CAN_FORM;
	if(CANSTMOB & (1<<AERR)) t_errorcode = ERROR_CAN_ACK;
	
	//avoid one-off CAN errors shutting down the whole car
	if(t_errorcode != ERROR_NONE)
	{
		cantimer++;
		if(cantimer>15) _errorcode = t_errorcode;
	}
}