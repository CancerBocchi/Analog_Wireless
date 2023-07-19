#include "project_1.h"
#include  "mylib.h"

Input_Side Data;
System_Falg Flag_Data;
/**
 * hrtimA 通道a作用于输入侧电流 
 *        通道b作用于电阻电压
 * hrtimB作用于输入测全桥
 * tim1 ch1 nch1 作用于输入侧全桥
 * tim8 ch1 为控制输入电流
 *      ch4 为控制采样时间
 */

void Input_Init()
{   
    
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,HRTIM_TIMERID_TIMER_A);
    HAL_HRTIM_WaveformCounterStart(&hhrtim1,HRTIM_TIMERID_TIMER_B);
    //定义PID结构体0
    PID_Init(&Data.Current_Controller,-10.0f,-320.0f,0.0f);
    Data.R = 15;
    Data.Current_Controller.Ref = 2.0f;//初始期望
    Data.Current_Controller.Value_I_Max = 5000.0f;
    Data.Current_Controller.Output_Max = hhrtim1.Instance->sTimerxRegs[0].PERxR * 0.75f;
    Data.Current_Controller.Output_Min = hhrtim1.Instance->sTimerxRegs[0].PERxR * 0.1f;

    PID_Init(&Data.Voltage_Controller,0.0f,-5.0f,0.0f);
    Data.Voltage_Controller.Ref = 1.8f;//初始期望
    Data.Voltage_Controller.Value_I_Max = 50000.0f;
    Data.Voltage_Controller.Output_Max = hhrtim1.Instance->sTimerxRegs[0].PERxR * 0.75f;
    Data.Voltage_Controller.Output_Min = hhrtim1.Instance->sTimerxRegs[0].PERxR * 0.2f;
    //配置初始占空比
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR = (int)((hhrtim1.Instance->sTimerxRegs[0].PERxR + 1)*0.2f);
    hhrtim1.Instance->sTimerxRegs[0].CMP4xR = 0.5f * hhrtim1.Instance->sTimerxRegs[0].CMP1xR;

    Flag_Data.Input_Current_Buck_Switch = false;
    Flag_Data.Input_Resistor_Buck_Switch = false;
    Flag_Data.Current_State = System_Charging;

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    ADC_Start();
}

void Input_Current_Buck_Switch(bool OnOff)
{
    if(OnOff == ON)
    {
        if(!Flag_Data.Input_Current_Buck_Switch)
        {
            Flag_Data.Input_Current_Buck_Switch = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TA1);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Flag_Data.Input_Current_Buck_Switch)
        {
            Flag_Data.Input_Current_Buck_Switch = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1);
        }
        return;
    }
}

void Input_Resistor_Buck_Switch(bool OnOff)
{
    if(OnOff == ON)
    {
        if(!Flag_Data.Input_Resistor_Buck_Switch)
        {
            Flag_Data.Input_Resistor_Buck_Switch = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TA2);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Flag_Data.Input_Resistor_Buck_Switch)
        {
            Flag_Data.Input_Resistor_Buck_Switch = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA2);
        }
        return;
    }
}

void Input_Bridge_Switch(bool OnOff)
{
    if(OnOff == ON)
    {
        if(!Flag_Data.Input_Bridge_Switch)
        {
            Flag_Data.Input_Bridge_Switch = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Flag_Data.Input_Bridge_Switch)
        {
            Flag_Data.Input_Bridge_Switch = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
        }
        return;
    }
}

void ADC_Start()
{
    HAL_ADC_Start_DMA(&hadc1,(uint32_t*)(Data.raw_ADC_Value),2);
}

void ADC_Stop()
{
    HAL_ADC_Stop_DMA(&hadc1);
}

void ADC_Conversion()
{

        float VoltageGet = (float)Data.raw_ADC_Value[0]*(3000.0f/4096.0f)*Kv+B;
        Data.bus_current = (VoltageGet - Sample_Voltage_Offset)/(50.0f*(float)Data.R)*Ki + 0.0008f;//电流单位为A 
        //Block_UART_printf(&huart4,"%f,%f\n",VoltageGet,Data.bus_current);

        Data.resistor_voltage = Data.raw_ADC_Value[1]*(3000.0f/4096.0f)*Kv;


}

void Input_Outputing_Program()
{
    Input_Bridge_Switch(OFF);
    Input_Current_Buck_Switch(OFF);
    Input_Resistor_Buck_Switch(ON);
    //PI控制
    hhrtim1.Instance->sTimerxRegs[0].CMP2xR = PID_Controller(&Data.Current_Controller,Data.bus_current);
    hhrtim1.Instance->sTimerxRegs[0].CMP4xR = 0.5f*hhrtim1.Instance->sTimerxRegs[0].CMP2xR;

}

void Input_Charging_Program()
{
    // HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    Input_Bridge_Switch(ON);
    Input_Current_Buck_Switch(ON);
    Input_Resistor_Buck_Switch(OFF);
    //PI控制
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR = PID_Controller(&Data.Current_Controller,Data.bus_current);
    hhrtim1.Instance->sTimerxRegs[0].CMP4xR = 0.5f*hhrtim1.Instance->sTimerxRegs[0].CMP1xR;
    //软启动代码
//     if(Data.Current_Controller.Ref <1.8f)
//     {
//         if(Data.bus_current<(Data.Current_Controller.Ref + 0.01f) 
//         && Data.bus_current>(Data.Current_Controller.Ref - 0.01f))
//         {
//             Data.Current_Controller.Ref += 0.001f;
//         }
//     }
}

