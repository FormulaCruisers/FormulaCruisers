/*
 * AMS.h
 *
 * Created: 2017-11-27 12:00:20
 *  Author: Jeremy
 */ 


#ifndef AMS_H_
#define AMS_H_

enum AMS_CHARGING_STAGE
{
	CS_Disconnected = 0,
	CS_Preheating = 1,
	CS_Precharging = 2,
	CS_Maincharging = 3,
	CS_Balancing = 4,
	CS_Charging_Finished = 5,
	CS_Charging_Error = 6,
};

enum AMS_CHARGING_ERROR
{
	CE_None = 0,
	CE_Comm_Lost_Precharge = 1,
	CE_No_Comm = 2,
	CE_Max_Charge_Duration_Expired = 3,
	CE_Comm_Lost_Charging = 4,
	CE_Cannot_Balance_Threshold = 5,
	CE_High_Temperature = 6,
	CE_Comm_Lost_Preheat = 7,
	CE_Num_Cells_Mismatch = 8,
	CE_Cell_Overvoltage = 9,
	CE_Cell_Protection = 10,
};

enum AMS_STATISTIC_DATATYPE
{
	SD_Statistic_Value = 0,
	SD_Additional_Value = 1,
	SD_Statistic_Timestamp = 2,
};

enum AMS_STATISTIC_TYPES
{	//See CAN message datasheet for more specific information on these
	ST_Total_Discharge = 0,						//S
	ST_Total_Charge = 1,						//S
	ST_Total_Dischange_Energy = 2,				//S
	ST_Total_Charge_Energy = 3,					//S
	ST_Total_Discharge_Time = 4,				//S
	ST_Total_Charge_Time = 5,					//S
	ST_Total_Distance = 6,						//S
	ST_Master_Clear_Count = 7,					// A
	ST_Dischange_Current_Max = 8,				//S T
	ST_Charge_Current_Max = 9,					//S T
	ST_Cell_Voltage_Min = 10,					//SAT
	ST_Cell_Voltage_Max = 11,					//SAT
	ST_Cell_Voltage_Diff_Max = 12,				//SAT
	ST_Pack_Voltage_Min = 13,					//S T
	ST_Pack_Voltage_Max = 14,					//S T
	ST_Cell_Module_Temp_Min = 15,				//SAT
	ST_Cell_Module_Temp_Max = 16,				//SAT
	ST_Cell_Module_Temp_Diff_Max = 17,			//SAT
	ST_BMS_Starts_Count = 18,					// AT
	ST_Undervolt_Prot_Count = 19,				// AT
	ST_Overvolt_Prot_Count = 20,				// AT
	ST_Discharge_Overcurrent_Prot_Count = 21,	// AT
	ST_Charge_Overcurrent_Prot_Count = 22,		// AT
	ST_Cell_Module_Overheat_Prot_Count = 23,	// AT
	ST_Leakage_Prot_Count = 24,					// AT
	ST_No_Cell_Comm_Prot_Count = 25,			// AT
	ST_Low_Voltage_Power_Reduction_Count = 26,	// AT
	ST_High_Current_Power_Reduction_Count = 27,	// AT
	ST_High_Cell_Module_Temp_Power_Reduction_Count = 28, // AT
	ST_Charger_Connect_Count = 29,				// A
	ST_Charger_Disconnect_Count = 30,			// A
	ST_Preheat_Stage_count = 31,				// A
	ST_Precharge_Stage_Count = 32,				// A
	ST_Maincharge_Stage_Count = 33,				// A
	ST_Balancing_Stage_Count = 34,				// A
	ST_Charging_Finished_Count = 35,			// A
	ST_Charging_Error_Count = 36,				// A
	ST_Charging_Retry_Count = 37,				// A
	ST_Trips_Count = 38,						// A
	ST_Charge_Restart_Count = 39,				// A
	
	ST_Cell_Overheat_Prot_Count = 45,			// AT
	ST_High_Cell_Temp_Power_Reduction_Count = 46,	// AT
	ST_Cell_Temp_Min = 47,						//SAT
	ST_Cell_Temp_Max = 48,						//SAT
	ST_Cell_Temp_Diff_Max = 49,					//SAT
};

enum AMS_EVENT_DATATYPE
{
	ED_Event_Info = 0,
	ED_Event_Timestamp = 1,
};

enum AMS_EVENT_TYPES
{
	ET_No_Event = 0,
	ET_BMS_Started = 1,
	ET_Lost_Cell_Comm = 2,
	ET_Established_Cell_Comm = 3,
	ET_Cell_Voltage_Low_Critical = 4,
	ET_Cell_Voltage_Low_Critical_Recovered = 5,
	ET_Cell_Voltage_High_Critical = 6,
	ET_Cell_Voltage_High_Critical_Recovered = 7,
	ET_Discharge_Current_High_Critical = 8,
	ET_Discharge_Current_High_Critical_Recovered = 9,
	ET_Charge_Current_High_Critical = 10,
	ET_Charge_Current_High_Critical_Recovered = 11,
	ET_Cell_Module_Temp_High_Critical = 12,
	ET_Cell_Module_Temp_High_Critical_Recovered = 13,
	ET_Leakage_Detected = 14,
	ET_Leakage_Recovered = 15,
	ET_Voltage_Low = 16,
	ET_Voltage_Low_Recovered = 17,
	ET_Voltage_High = 18,
	ET_Voltage_High_Recovered = 19,
	ET_Cell_Module_Temp_High = 20,
	ET_Cell_Module_Temp_High_Recovered = 21,
	ET_Charger_Connected = 22,
	ET_Charger_Disconnected = 23,
	ET_Preheating_Start = 24,
	ET_Precharging_Start = 25,
	ET_Main_Charging_Start = 26,
	ET_Balancing_Start = 27,
	ET_Charging_Finished = 28,
	ET_Charging_Error = 29,
	ET_Charging_Retrying = 30,
	ET_Charging_Restarting = 31,
	ET_Cell_Temp_High_Critical = 42,
	ET_Cell_Temp_High_Critical_Recovered = 43,
	ET_Cell_Temp_High = 44,
	ET_Cell_Temp_High_Recovered = 45,
};











struct AMS_OVERALL
{
	//INPUT SIGNALS
	uint8_t IS_Ignition_Key : 1;
	uint8_t IS_Charger_Mains : 1;
	uint8_t IS_Fast_Charge : 1;
	uint8_t IS_Leakage_Sensor : 1;
	uint8_t IS_Reserved1 : 4;
	
	//OUTPUT SIGNALS
	uint8_t OS_Charger_Enable : 1;
	uint8_t OS_Heater_Enable : 1;
	uint8_t OS_Battery_Contactor : 1;
	uint8_t OS_Battery_Fan : 1;
	uint8_t OS_Power_Reduction : 1;
	uint8_t OS_Charging_Interlock : 1;
	uint8_t OS_DCDC_Control : 1;
	uint8_t OS_Contactor_PreCharge : 1;
	
	//The rest
	uint8_t N_Livecells_MSB;
	AMS_CHARGING_STAGE Charging_Stage : 8;
	uint16_t N_Chargingduration;				//In minutes
	AMS_CHARGING_ERROR Last_Charging_Error : 8;
	uint8_t N_Livecells_LSB;
};

struct AMS_DIAGNOSTIC
{
	//Protection flags LSB
	uint8_t PF_Undervoltage : 1;
	uint8_t PF_Overvoltage : 1;
	uint8_t PF_Discharge_Overcurrent : 1;
	uint8_t PF_Charge_Overcurrent : 1;
	uint8_t PF_Cell_Module_Overheat : 1;
	uint8_t PF_Leakage : 1;
	uint8_t PF_No_Cell_Comm : 1;
	uint8_t PF_Reserved1 : 1;
	
	//Warning (reduction) flags
	uint8_t WF_Low_Voltage : 1;
	uint8_t WF_High_Current : 1;
	uint8_t WF_High_Temperature : 1;
	uint8_t WF_Reserved1 : 5;
	
	//Protection flags MSB
	uint8_t PF_Reserved2 : 3;
	uint8_t PF_Cell_Overheat : 1;
	uint8_t PF_No_Current_Sensor : 1;
	uint8_t PF_Pack_Undervoltage : 1;
	uint8_t PF_Reserved3 : 2;			//Document says that bit 4-7 are reserved, but bit 4 and 5 are used :thinking:
	
	//Battery status flags
	uint8_t BF_Cell_Voltages_Valid : 1;
	uint8_t BF_Cell_Module_Temp_Valid : 1;
	uint8_t BF_Cell_Balancing_Valid : 1;
	uint8_t BF_Num_Livecells_Valid : 1;
	uint8_t BF_Battery_Charging_Active : 1; //Only used when not charging via CAN
	uint8_t BF_Cell_Temp_Valid : 1;
	uint8_t BF_Reserved1 : 2;
	
	uint8_t RESERVED[4];
};

struct AMS_VOLTAGE
{
	uint8_t Cell_Voltage_Min;		//Multiplied by 100 and is 2V lower than the actual value
	uint8_t	Cell_Voltage_Max;		//"
	uint8_t Cell_Voltage_Average;	//"
	uint8_t Total_Voltage_b1;		//Multiplied by 100 (What is this ordering?)
	uint8_t Total_Voltage_b0;		//"
	uint8_t Total_Voltage_b3;		//"
	uint8_t Total_Voltage_b2;		//"

	uint8_t RESERVED[1];
};

struct AMS_CELL_MODULE_TEMP
{
	uint8_t Cell_Module_Temp_Min;		//In degrees Celsius, starting at 100. So a value of 115 would be 15 deg C.
	uint8_t Cell_Module_Temp_Max;		//"
	uint8_t Cell_Module_Temp_Average;	//"
	
	uint8_t RESERVED[5];
};

struct AMS_CELL_TEMP
{
	uint8_t Cell_Module_Temp_Min;		//In degrees Celsius, starting at 100. So a value of 115 would be 15 deg C.
	uint8_t Cell_Module_Temp_Max;		//"
	uint8_t Cell_Module_Temp_Average;	//"
	
	uint8_t RESERVED[5];
};

struct AMS_CELL_BALANCING 
{
	uint8_t Cell_Balancing_Min;			//From 0 to 255, where 255 is 100%
	uint8_t Cell_Balancing_Max;			//"
	uint8_t Cell_Balancing_Average;		//"
	
	uint8_t RESERVED[5];
};

struct AMS_CHARGE_STATE
{
	uint16_t Current;				//Multiplied by 10
	uint16_t Estimated_Charge;		//"
	uint8_t Estimated_Charge_State;	//In whole percents
	
	uint8_t RESERVED[3];
};

struct AMS_ENERGY
{
	uint16_t Estimated_Consumption;		//In Wh per KM
	uint16_t Estimated_Energy;			//In Wh divided by 10
	uint16_t Estimated_Distance_Left;	//In Wh multiplied by 10
	uint16_t Distance_Traveled;			//In KM multiplied by 10
};

struct AMS_STATISTIC
{
	uint8_t ID;
	AMS_STATISTIC_DATATYPE Datatype : 8;
	uint32_t Data;
	
	uint8_t RESERVED[2];
};

struct AMS_EVENT
{
	uint8_t ID;
	AMS_EVENT_DATATYPE Datatype : 8;
	uint32_t data;
	
	uint8_t RESERVED[2];
};

#endif /* AMS_H_ */