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
	
	ERROR_SHUTDOWN,			//When a shutdown is called while predischarging or driving
	
	ERROR_CAN_BIT,
	ERROR_CAN_STUFF,		//See AT90CAN128 datasheet page 261/262 for quick descriptions of these CAN errors
	ERROR_CAN_CRC,
	ERROR_CAN_FORM,
	ERROR_CAN_ACK,

	ERROR_SD_INIT_RESET,	//When the SD card could not be reset
	ERROR_SD_INIT_READY,	//When the SD card is not ready
	ERROR_SD_BLOCK,			//When a block error occurs
	ERROR_SD_NOTREADY,		//When the SD card is not ready
	ERROR_SD_READ,			//When an SD read error occurs
	ERROR_SD_WRITE,			//When an SD write error occurs
	ERROR_SD_SIZE,			//Writing too much data to SD card
	
	ERROR_UNKNOWN,			//For unknown errors
};

extern volatile uint16_t gas1, gas2, brake, gas1perc, gas2perc, brakeperc, flowleft, flowright;
extern volatile enum _error _errorcode;

char* get_error(enum _error e);
void e_checkranges();
void e_checkdiscrepancy();
void e_checksensors();
void e_checkflow();
void e_checkCAN();

#endif