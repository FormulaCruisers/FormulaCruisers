/* AMS.H
This file contains several definitions to be used when communicating to the Emus AMS via CAN.
 */ 

#ifndef AMS_H_
#define AMS_H_

#define AMS_BASE					0x1B5

#define AMS_MSG_OVERALL				AMS_BASE + 0
#define AMS_MSG_DIAGNOSTIC			AMS_BASE + 7
#define AMS_MSG_VOLTAGE				AMS_BASE + 1
#define AMS_MSG_CELL_MODULE_TEMP	AMS_BASE + 2
#define AMS_MSG_CELL_TEMP			AMS_BASE + 8
#define AMS_MSG_CELL_BALANCING		AMS_BASE + 3
#define AMS_MSG_CONFIGURATION		AMS_BASE + 128
#define AMS_MSG_LOGIN				AMS_BASE + 130
//TODO: complete list
	
//void to_struct(void* structure, uint8_t data[8])
//{
//	memcpy(structure, data, 8);
//}

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

enum AMS_PARAMETER
{
	 PM_Num_Overall_Cells = 0x0100,
	 PM_Capacity = 0x0102, //Coded in 0.1Ah
	PM_Min_Cell_Volt = 0x0004, //Coded in 0.01V with offset 200(2V)
	PM_Max_Cell_Volt = 0x0005, //Coded in 0.01V with offset 200(2V)
	PM_Max_Cell_Module_Volt = 0x0006, //Coded in 1C with offset -100
	PM_Max_Balance_Current = 0x0007, //Range of 0-255
	PM_Low_Cell_Volt = 0x0008, //Coded in 0.01V with offset 200(2V)
	PM_Min_Charge_Temp = 0x0009, //Coded in 1C with offset -100
	PM_DCDC_Ctrl_Voltage_Active = 0x000A, //Coded in 0.01V with offset 200(2V)
	PM_Full_Charge_Volt = 0x000B, //Coded in 0.01V with offset 200(2V)
	PM_Allowed_Disbalance = 0x000C, //Coded in 0.01V
	PM_Precharge_Voltage = 0x000D, //Coded in 0.01V with offset 200(2V)
	PM_Early_Balancing_Threshold = 0x000E, //Coded in 0.01V with offset 200(2V)
	PM_Balancing_Range = 0x000F, //Coded in 0.01V with offset 200(2V)
	PM_Data_Transmission_Active = 0x0010, //Coded in 0.1s
	 PM_Data_Transmission_Sleep = 0x0111, //Coded in 0.1s
	PM_Num_Cell_Strings = 0x0013,
	PM_Fan_On_Temp = 0x0014, //Coded in 1C with offset -100
	PM_High_Cell_Module_Temp = 0x0015, //Coded in 1C with offset -100
	PM_Climate_Control_Normal_Temp = 0x0016, //Coded in 1C with offset -100
	PM_Climate_Control_Charging_Temp = 0x0017, //Coded in 1C with offset -100
	 PM_Climate_Control_Min_SOC = 0x0118, //Coded in 0.01%
	PM_Charge_Restart_Voltage = 0x001A, //Coded in 0.01V with offset 200(2V)
	PM_SOC_Low_Volt = 0x0022,
	PM_Cell_Undervolt_Activate_Delay = 0x0025, //Coded in 0.1s
	PM_Cell_Undervolt_Deactivate_Delay = 0x0026,
	PM_Cell_Overvolt_Activate_Delay = 0x0027,
	PM_Cell_Overvolt_Deactivate_Delay = 0x0028,
	PM_Cell_Module_Overheat_Activation_Delay = 0x0029,
	PM_Cell_Module_Overheat_Deactivation_Delay = 0x002A,
	PM_Discharge_Overcurrent_Activation_Delay = 0x002B,
	PM_Discharge_Overcurrent_Deactivation_Delay = 0x002C,
	PM_Charge_Overcurrent_Activation_Delay = 0x002D,
	PM_Charge_Overcurrent_Deactivation_Delay = 0x002E,
	PM_No_Cell_Comm_Activation_Delay = 0x002F,
	PM_No_Cell_Comm_Deactivation_Delay = 0x0030,
	PM_Cell_Undervolt_Deactivate_Threshold = 0x0031,
	PM_Cell_Overvolt_Deactivate_Threshold = 0x0032,
	PM_Cell_Module_Overheat_Deactivate_Threshold = 0x0033,
	PM_Low_Cell_Volt_Activation_Delay = 0x0034,
	PM_Low_Cell_Volt_Deactivation_Delay = 0x0035,
	PM_High_Cell_Module_Temp_Activation_Delay = 0x0036,
	PM_High_Cell_Module_Temp_Deactivation_Delay = 0x0037,
	PM_High_Discharge_Current_Activation_Delay = 0x0038,
	PM_High_Discharge_Current_Deactivation_Delay = 0x0039,
	PM_Low_Cell_Volt_Deactivate_Threshold = 0x003A,
	PM_High_Cell_Module_Temp_Deactivate = 0x003B,
	PM_Contactor_Precharge_Duration = 0x003C,
	PM_Climate_Control_Max_Duration_No_Charge = 0x0048,
	PM_Insulation_Fault_Activation_Delay = 0x0049,
	PM_Insulation_Fault_Deactivation_Delay = 0x004A,
	PM_Cell_Overheat_Activate_Threshold = 0x004B,
	PM_Cell_Overheat_Deactivate_Threshold = 0x004C,
	PM_Cell_Overheat_Activation_Delay = 0x004D,
	PM_Cell_Overheat_Deactivation_Delay = 0x004E,
	PM_High_Cell_Temp_Activate_Threshold = 0x004F,
	PM_High_Cell_Temp_Deactivate_Threshold = 0x0050,
	PM_High_Cell_Temp_Activation_Delay = 0x0051,
	PM_High_Cell_Temp_Deactivation_Delay = 0x0052,
	PM_DCDC_Ctrl_Voltage_Passive = 0x0060,
	PM_No_Current_Sensor_Activation_Delay = 0x0061,
	PM_No_Current_Sensor_Deactivation_Delay = 0x0062,
	PM_Cell_Comm_Restore_Duration = 0x0063,
	PM_Distance_Estimate_Safety_Margin = 0x0400,
	 PM_Pulses_Per_Distance_Unit = 0x0501,
	 PM_Distane_Unit_Name = 0x503,
	PM_Min_SOC_Output = 0x0405,
	PM_Max_SOC_Output = 0x0406,
	
	//...Some pin functions...
	
	 PM_CAN_ID_Base = 0x0D00,
	PM_CAN_Speed = 0x0C02,
	 PM_Device_CAN_ID_Base = 0x0D03,
	 
	//...Lots of CAN Cell stuff that shouldnt be touched...
	
	 PM_Current_Sensor_L_Calibration = 0x1100,
	PM_Current_Sensor_L_Reference = 0x1002,
	 PM_Current_Sensor_H_Calibration = 0x1103,
	 PM_Current_Sensor_H_Reference = 0x1105, //Is actually a signed int16?
	PM_Current_Sensor_Reverse_Dir = 0x1007,
	PM_Current_Sensor_Deadzone = 0x1008,
	PM_Current_Sensor_Type = 0x1009, //=0 if Gen 1, =1 if Gen 2
	  PM_Function_Flags_0 = 0x1600,
	  PM_Function_Flags_1 = 0x1604,
	PM_Charger_Type = 0x1800,
	 PM_Fast_Charging_Current = 0x1901,
	 PM_Slow_Charging_Current = 0x1903,
	PM_Precharge_Current = 0x1805,
	 PM_Charge_Finished_Current = 0x1806, //Odd one out that is 16 bit
	 PM_Max_Precharge_Duration = 0x1907,
	 PM_Max_Main_Charge_Duration = 0x1909,
	 PM_Max_Balancing_Duration = 0x190B,
	 PM_Charger_Overcurrent_Activate_Threshold = 0x190D,
	 PM_High_Discharge_Current_Activate_Threshold = 0x190F,
	 PM_Discharge_Overcurrent_ACtivate_Threshold = 0x1911,
	PM_Num_Chargers = 0x1815,
	 PM_Charger_CAN_ID_Base = 0x1916,
	 PM_Max_PWM_Output = 0x191A,
	 PM_Min_PWM_Output = 0x191C,
};

//uint8_t is16bit(enum AMS_PARAMETER param) { return (param == 0x1806 || (param & 0x0100) > 0); }//The rule of & 0x0100 for 16 bit is not true for param 0x1806... this one is actually 16 bit but would return as 8.
//uint8_t is32bit(enum AMS_PARAMETER param) { return (param & 0x0200) > 0; }


typedef struct AMS_OVERALL
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
	uint8_t N_Livecells_H;
	enum AMS_CHARGING_STAGE Charging_Stage : 8;
	uint8_t N_Chargingduration_H;				//In minutes
	uint8_t N_Chargingduration_L;				//In minutes
	enum AMS_CHARGING_ERROR Last_Charging_Error : 8;
	uint8_t N_Livecells_L;
} AMS_OVERALL;

typedef struct AMS_DIAGNOSTIC
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
} AMS_DIAGNOSTIC;

typedef struct AMS_VOLTAGE
{
	uint8_t Cell_Voltage_Min;		//Multiplied by 100 and is 2V lower than the actual value
	uint8_t	Cell_Voltage_Max;		//"
	uint8_t Cell_Voltage_Average;	//"
	uint8_t Total_Voltage_LSW_H;	//Multiplied by 100 (weird byte ordering...)
	uint8_t Total_Voltage_LSW_L;	//"
	uint8_t Total_Voltage_MSW_H;	//"
	uint8_t Total_Voltage_MSW_L;	//"

	uint8_t RESERVED[1];
} AMS_VOLTAGE;

typedef struct AMS_CELL_MODULE_TEMP
{
	uint8_t Cell_Module_Temp_Min;		//In degrees Celsius, starting at 100. So a value of 115 would be 15 deg C.
	uint8_t Cell_Module_Temp_Max;		//"
	uint8_t Cell_Module_Temp_Average;	//"
	
	uint8_t RESERVED[5];
} AMS_CELL_MODULE_TEMP;

typedef struct AMS_CELL_TEMP
{
	uint8_t Cell_Module_Temp_Min;		//In degrees Celsius, starting at 100. So a value of 115 would be 15 deg C.
	uint8_t Cell_Module_Temp_Max;		//"
	uint8_t Cell_Module_Temp_Average;	//"
	
	uint8_t RESERVED[5];
} AMS_CELL_TEMP;

typedef struct AMS_CELL_BALANCING 
{
	uint8_t Cell_Balancing_Min;			//From 0 to 255, where 255 is 100%
	uint8_t Cell_Balancing_Max;			//"
	uint8_t Cell_Balancing_Average;		//"
	
	uint8_t RESERVED[5];
} AMS_CELL_BALANCING;

typedef struct AMS_CHARGE_STATE
{
	uint8_t Current_H;				//Multiplied by 10
	uint8_t Current_L;				//"
	uint8_t Estimated_Charge_H;		//"
	uint8_t Estimated_Charge_L;		//"
	uint8_t Estimated_Charge_State;	//In whole percents
	
	uint8_t RESERVED[3];
} AMS_CHARGE_STATE;

typedef struct AMS_ENERGY
{
	uint8_t Estimated_Consumption_H;	//In Wh per KM
	uint8_t Estimated_Consumption_L;	//"
	uint8_t Estimated_Energy_H;			//In Wh divided by 10
	uint8_t Estimated_Energy_L;			//"
	uint8_t Estimated_Distance_Left_H;	//In Wh multiplied by 10
	uint8_t Estimated_Distance_Left_L;	//"
	uint8_t Distance_Traveled_H;		//In KM multiplied by 10
	uint8_t Distance_Traveled_L;		//"
} AMS_ENERGY;

typedef struct AMS_STATISTIC
{
	uint8_t ID;
	enum AMS_STATISTIC_DATATYPE Datatype : 8;
	uint8_t data_3;
	uint8_t data_2;
	uint8_t data_1;
	uint8_t data_0;
	
	uint8_t RESERVED[2];
} AMS_STATISTIC;

typedef struct AMS_EVENT
{
	uint8_t ID;
	enum AMS_EVENT_DATATYPE Datatype : 8;
	uint8_t data_3;
	uint8_t data_2;
	uint8_t data_1;
	uint8_t data_0;
	
	uint8_t RESERVED[2];
} AMS_EVENT;

#endif /* AMS_H_ */