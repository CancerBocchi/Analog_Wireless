#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "inc.h"
#define Kv 1.0083f
#define B 13.578f
#define Sample_Voltage_Offset 1251.0f
#define Ki 0.9776f

#define ON  1
#define OFF 0
//电流单位为 A 电压单位为 mV 电阻单位为 m欧
typedef struct IN
{
    float bus_current;//总线电流大小
    float resistor_voltage;//电阻电压大小
    uint16_t raw_ADC_Value[2];//原始采样数据
    uint32_t R;//采样电阻
    PID_t Current_Controller;//PID控制器
    PID_t Voltage_Controller;
    int flag;
    float offset;//采样偏置
}Input_Side;

typedef enum STATE
{
    System_Charging = 0x00,
    System_Outputing = 0x01,
}System_State;

typedef struct FLAG
{
    /* data */
    bool Input_Current_Buck_Switch;
    bool Input_Resistor_Buck_Switch;
    bool Input_Bridge_Switch;
    System_State Current_State;
}System_Falg;

extern System_Falg Flag_Data;
extern Input_Side Data;

void Input_Init();
void ADC_Start();
void ADC_Stop();
void ADC_Conversion();
void Input_Buck_Switch(bool OnOff);
void Input_Bridge_Switch(bool OnOff);
void Input_Charging_Program();
void Input_Outputing_Program();

#endif // !__PROJECT_H__