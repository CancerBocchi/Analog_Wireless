#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "inc.h"
#include "State_Control.h"
#include "Switch.h"
/*
* hrtimA  通道一控制电容充电buck 
          通道二控制电阻端buck
* hrtimB  控制 全桥
* ADC 通道0 采集电阻电压
*/
#define K_ADC_1 1.0112f
#define K_ADC_2 1.0112f
#define K_ADC_0 1.056f
#define Kcv (3.0f/4096.0f)*4.7522f
#define Krv (3.0f/4096.0f)*4.7378f
#define Kcc (3.0f/4096.0f)*1.9789f
#define CC_Offset -1.232f

typedef enum FAULT_CODE
{
    AfterCharging_OverCur = 0x00,
    OverVol_Fault = 0x01,
}Fault_Code;

typedef enum
{
    ResVolLoopRun = 0x00,
    ChaPowLoopRun = 0x01,
    ChaVolLoopRun = 0x02,
    NotInCharging = 0x03,
}Charging_State;

typedef enum SYSTEM_STATE
{
    System_Charging  = 0x00,
    System_Outputing = 0x01,
    System_Stopping   = 0x02,
    System_Fault     = 0x03,
}System_State;

typedef enum STATE
{
    ON  = 0x01,
    OFF = 0x00,
}State;

typedef struct 
{
    System_State Current_State; 
    State OutputPort_Bridge_State;
    State OutputPort_Resistor_Buck_State;
    State OutputPort_Charger_Buck_State;
}State_Flag;

typedef struct SYSYTEM_PI
{
    PID_t    OutputPort_Charger_Power_Control;
    PID_t    OutputPort_Resistor_Voltage_Control;
    PID_t    OutputPort_Charger_Voltage_Control;
    PID_t    OutputPort_Charger_OutPower_Control;
}System_PI;

typedef struct SYSTEM_ADC
{
    uint8_t  Sampling_Resistor;
    uint16_t Raw_ADC_Value[3];
    float    OutputPort_Charger_Current_Value;
    float    OutputPort_Charger_Voltage_Value;
    float    OutputPort_Resistor_Voltage_Value;
    float    OutputPort_Charging_Power;
}System_ADC;

typedef struct SYSTEM_DATA
{
    //ADC_Data
    System_ADC System_Sample;
    //PID_Data
    System_PI System_Control;
    //标志位与状态机
    State_Flag System_Flag;

}System_Data;

extern System_Data Data;
extern Fault_Code Fault_Monitor;
extern Charging_State Charging_Falg;

void System_Fault_Program_Start();
void System_Charging_Program_Start();
void Output_ResistorVoltage_LoopRun();
void Output_ChargerPower_LoopRun();
void ADC_Conversion_Program();
void System_Init();
void Output_ChargerVoltage_LoopRun();
void Output_ChargerOutPow_LoopRun();
void System_Outputing_Program_Start();

#endif // !__PROJECT_H__