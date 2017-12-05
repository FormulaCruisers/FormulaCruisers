/*
 * MotorController.h
 *
 * Created: 2017-12-05 11:29:26
 *  Author: Jeremy
 */ 


#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

// Turning the wheel

#define MC_F_NOMINAL					0x05
#define MC_V_NOMINAL					0x06
#define MC_COS_PHI						0x07
#define MC_COMMAND_CUTOFF				0x1E
#define MC_I_ACTUAL						0x20
#define MC_ID_SET						0x21
#define MC_I_COMMAND_POST				0x22
#define MC_I_RAMP						0x25
#define MC_I_COMMAND_PRE				0x26
#define MC_IQ_ACTUAL_ACTIVE				0x27
#define MC_IQ_ACTUAL_IDLE				0x28
#define MC_AIN1_OFFSET_SCALE			0x2F	//Kinda special
#define MC_N_ACTUAL						0x30
#define MC_SET_SPEED					0x31		//Value between 0x0000 - 0x7FFF
#define MC_N_COMMAND_POST				0x32
#define MC_N_ERROR						0x33
#define MC_N_LIMIT						0x34		//Value between 0x0000 - 0x7FFF
#define MC_NM_RAMP_ACC					0x35	//Kinda special (L = N, H = M)
#define MC_COMMAND_MODE					0x36	//Super special
#define MC_N_LIMIT_NEGATIVE				0x3E
#define MC_N_LIMIT_POSITIVE				0x3F
#define MC_FB_OFFSET					0x44
#define MC_I_LIMIT_DIGITAL				0x46
#define MC_I_LIMIT_ACTUAL				0x48
#define MC_TEMP_MOTOR					0x49
#define MC_TEMP_AIR						0x4A
#define MC_TEMP_POWERSTAGE				0x4B
#define MC_I_MAX						0x4D
#define MC_I_NOM						0x4E
#define MC_M_POLE						0x4F
#define MC_AIN1_CUTOFF					0x50
#define MC_N_NOMINAL					0x59
#define MC_MOTOR_FLAGS					0x5A	//Super special
#define MC_N_COMMAND_PRE				0x5D
#define MC_I_ACTUAL_FILTERED			0x5F
#define MC_AIN_FILTER					0x60
#define MC_I_T							0x61
#define MC_SERIAL_NUM					0x62
#define MC_POWER_V_NOMINAL				0x64
#define MC_REGEN_RESISTOR				0x65	//Kinda special
#define MC_TYPE							0x67
#define MC_CAN_RX_ID					0x68
#define MC_CAN_TX_ID					0x69
#define MC_CAN_BAUD						0x73
#define MC_SET_TORQUE					0x90		//Value between 0x0000 - 0x7FFF
#define MC_M_TEMP						0xA3
#define MC_FEEDBACK_TYPE				0xA4	//Special
#define MC_DC_VOLTAGE_LINK				0xA5	//Kinda special
#define MC_FB_INCREMENTS_M				0xA6
#define MC_FB_POLE						0xA7
#define MC_N_ACTUAL_FILTERED			0xA8
#define MC_I_MAXPK_PERCENT				0xC4		//Value between 0x0000 - 0x3FFF
#define MC_I_CONEFF_PERCENT				0xC5		//Value between 0x0000 - 0x3FFF
#define MC_RAMP_LIMIT					0xC7
#define MC_FB_INCREMENT					0xCF	//Kinda special
#define MC_CAN_TIMEOUT					0xD0
#define MC_AIN2_OFFSET_SCALE			0xD7	//Kinda special
#define MC_NM_RAMP_DEC					0xED	//Kinda special(L = N, H = M)
#define MC_I_PEAK_TIMING				0xF0
#define MC_BRAKE_DEL					0xF1
#define MC_AXIS							0xF8


#endif /* MOTORCONTROLLER_H_ */