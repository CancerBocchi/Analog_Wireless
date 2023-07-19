#include "project_2.h"

#define C_Current_Get(ADC_Value) ((float)ADC_Value*Kcc+CC_Offset)
#define C_Voltage_Get(ADC_Value) ((float)ADC_Value*Kcv)
#define R_Voltage_Get(ADC_Value) ((float)ADC_Value*Krv)
System_Data Data;
Fault_Code Fault_Monitor;
/*
* hrtimA  通道一控制电容充电buck 
          通道二控制电阻端buck
* hrtimB  控制 全桥
* ADC 通道0 采集电阻电压
*/
void System_Init()
{
    //开启时钟基准 于中断 开启ADC
    HAL_HRTIM_WaveformCounterStart(&hhrtim1,HRTIM_TIMERID_TIMER_B);
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1,HRTIM_TIMERID_TIMER_A);

    HAL_ADCEx_Calibration_Start(&hadc1,  ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1,(uint32_t*)Data.System_Sample.Raw_ADC_Value,3);

    //PID data 初始化
    PID_Init(&Data.System_Control.OutputPort_Charger_Power_Control,15.0f,0.0f,0.0f);
    Data.System_Control.OutputPort_Charger_Power_Control.Output_Max  = 6000.0f;
    Data.System_Control.OutputPort_Charger_Power_Control.Output_Min  = -6000.0f;
    Data.System_Control.OutputPort_Charger_Power_Control.Value_I_Max = 50.0f;
    Data.System_Control.OutputPort_Charger_Power_Control.Ref         = 4.0f;

    PID_Init(&Data.System_Control.OutputPort_Resistor_Voltage_Control,700.0f,350.0f,0.0f);
    Data.System_Control.OutputPort_Resistor_Voltage_Control.Output_Max  = 54000.0f;
    Data.System_Control.OutputPort_Resistor_Voltage_Control.Output_Min  = 12000.0f;
    Data.System_Control.OutputPort_Resistor_Voltage_Control.Value_I_Max = 150.0f;
    Data.System_Control.OutputPort_Resistor_Voltage_Control.Ref         = 10.0f;

    PID_Init(&Data.System_Control.OutputPort_Charger_Voltage_Control,500.0f,0.0f,0.0f);
    Data.System_Control.OutputPort_Charger_Voltage_Control.Output_Max  = 6000.0f;
    Data.System_Control.OutputPort_Charger_Voltage_Control.Output_Min  = -6000.0f;
    Data.System_Control.OutputPort_Charger_Voltage_Control.Value_I_Max = 500.0f;
    Data.System_Control.OutputPort_Charger_Voltage_Control.Ref     = 8.1f;

    PID_Init(&Data.System_Control.OutputPort_Charger_OutPower_Control,0.0f,0.0f,0.0f);
    Data.System_Control.OutputPort_Charger_OutPower_Control.Output_Max  = 6000.0f;
    Data.System_Control.OutputPort_Charger_OutPower_Control.Output_Min  = -6000.0f;
    Data.System_Control.OutputPort_Charger_OutPower_Control.Value_I_Max = 500.0f;
    Data.System_Control.OutputPort_Charger_OutPower_Control.Ref     = -2.9f;

    //ADC Data init
    Data.System_Sample.OutputPort_Charger_Current_Value = 0.0f;
    Data.System_Sample.OutputPort_Charger_Voltage_Value = 0.0f;
    Data.System_Sample.OutputPort_Resistor_Voltage_Value = 0.0f;
    Data.System_Sample.OutputPort_Charging_Power = 0.0f;
    Data.System_Sample.Raw_ADC_Value[0] = 0;
    Data.System_Sample.Raw_ADC_Value[1] = 0;
    Data.System_Sample.Raw_ADC_Value[2] = 0;
    Data.System_Sample.Sampling_Resistor = 15.0f;
    //状态机置位
    Data.System_Flag.Current_State = System_Charging;
    Data.System_Flag.OutputPort_Bridge_State = OFF;
    Data.System_Flag.OutputPort_Charger_Buck_State = OFF;
    Data.System_Flag.OutputPort_Resistor_Buck_State = OFF;

    Charging_Falg = ResVolLoopRun;

    hhrtim1.Instance->sTimerxRegs[0].CMP1xR = 0.1f*hhrtim1.Instance->sTimerxRegs[0].PERxR;
    hhrtim1.Instance->sTimerxRegs[0].CMP2xR = 0.5f*hhrtim1.Instance->sTimerxRegs[0].PERxR;
    hhrtim1.Instance->sTimerxRegs[0].CMP4xR = 0.5f*hhrtim1.Instance->sTimerxRegs[0].CMP1xR;
}

void ADC_Conversion_Program()
{
    Data.System_Sample.OutputPort_Resistor_Voltage_Value = R_Voltage_Get(Data.System_Sample.Raw_ADC_Value[0]);
    Data.System_Sample.OutputPort_Charger_Current_Value  = C_Current_Get(Data.System_Sample.Raw_ADC_Value[1]);
    Data.System_Sample.OutputPort_Charger_Voltage_Value  = C_Voltage_Get(Data.System_Sample.Raw_ADC_Value[2]);

    Data.System_Sample.OutputPort_Charging_Power = Data.System_Sample.OutputPort_Charger_Current_Value*
                                                   Data.System_Sample.OutputPort_Charger_Voltage_Value;

    // static int flag = 0;
    // if(flag < 100)
    // {
        //   Block_UART_printf(&huart1,"%.3f,%.3f,%.3f,%.3f,%d\n",Data.System_Sample.OutputPort_Charging_Power
        //                                               ,Data.System_Sample.OutputPort_Charger_Voltage_Value
        //                                               ,Data.System_Sample.OutputPort_Charger_Current_Value,
        //                                               Data.System_Sample.OutputPort_Resistor_Voltage_Value,
        //                                               hhrtim1.Instance->sTimerxRegs[0].CMP1xR);
    //     flag ++;
    // }
    
    
}

void Output_ChargerPower_LoopRun()
{
    int PID_Value = (int)PID_Controller(&Data.System_Control.OutputPort_Charger_Power_Control
                                        ,Data.System_Sample.OutputPort_Charging_Power);

    
    //输出限幅
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR 
    = (hhrtim1.Instance->sTimerxRegs[0].CMP1xR > hhrtim1.Instance->sTimerxRegs[0].PERxR*0.81f)
    ? (hhrtim1.Instance->sTimerxRegs[0].PERxR*0.80f) : (hhrtim1.Instance->sTimerxRegs[0].CMP1xR + PID_Value);

    //软起动
    if(Data.System_Control.OutputPort_Charger_Power_Control.Ref < 3.8f)
    {
        int ref = Data.System_Control.OutputPort_Charger_Power_Control.Ref;
        if(Data.System_Sample.OutputPort_Charging_Power < ref + 0.05f &&
             Data.System_Sample.OutputPort_Charging_Power > ref - 0.05f)
        {
            Data.System_Control.OutputPort_Charger_Power_Control.Ref += 0.5f;
        }
    }
}

void Output_ResistorVoltage_LoopRun()
{
    hhrtim1.Instance->sTimerxRegs[0].CMP2xR = (int)PID_Controller(&Data.System_Control.OutputPort_Resistor_Voltage_Control
                                                                  ,Data.System_Sample.OutputPort_Resistor_Voltage_Value);

                                                                  
    //软启动
    if(Data.System_Control.OutputPort_Resistor_Voltage_Control.Ref < 11.01f)
    {
        int ref = Data.System_Control.OutputPort_Resistor_Voltage_Control.Ref;
        if(Data.System_Sample.OutputPort_Resistor_Voltage_Value < ref + 0.01f &&
             Data.System_Sample.OutputPort_Resistor_Voltage_Value > ref - 0.01f)
        {
            Data.System_Control.OutputPort_Resistor_Voltage_Control.Ref += 1.0f;
        }
    }
}

void Output_ChargerVoltage_LoopRun()
{
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR += (int)PID_Controller(&Data.System_Control.OutputPort_Charger_Voltage_Control
                                                                ,Data.System_Sample.OutputPort_Charger_Voltage_Value);
    
    //输出限幅
    if(hhrtim1.Instance->sTimerxRegs[0].CMP1xR >= 0.80*hhrtim1.Instance->sTimerxRegs[0].PERxR)
        hhrtim1.Instance->sTimerxRegs[0].CMP1xR =  0.8*hhrtim1.Instance->sTimerxRegs[0].PERxR;
    else if(hhrtim1.Instance->sTimerxRegs[0].CMP1xR <= 0.3*hhrtim1.Instance->sTimerxRegs[0].PERxR)
        hhrtim1.Instance->sTimerxRegs[0].CMP1xR =  0.3*hhrtim1.Instance->sTimerxRegs[0].PERxR;

    // hhrtim1.Instance->sTimerxRegs[0].CMP1xR = (int)PID_Controller(&Data.System_Control.OutputPort_Charger_Voltage_Control
    //                                                              ,Data.System_Sample.OutputPort_Charger_Voltage_Value);
}

void Output_ChargerOutPow_LoopRun()
{
    int PID_Value = (int)PID_Controller(&Data.System_Control.OutputPort_Charger_OutPower_Control
                                        ,Data.System_Sample.OutputPort_Charging_Power);

    
    //输出限幅
    hhrtim1.Instance->sTimerxRegs[0].CMP1xR 
    = (hhrtim1.Instance->sTimerxRegs[0].CMP1xR > hhrtim1.Instance->sTimerxRegs[0].PERxR*0.81f)
    ? (hhrtim1.Instance->sTimerxRegs[0].PERxR*0.80f) : (hhrtim1.Instance->sTimerxRegs[0].CMP1xR + PID_Value);
    
    //软起动
    if(Data.System_Control.OutputPort_Charger_OutPower_Control.Ref > -3.8f)
    {
        int ref = Data.System_Control.OutputPort_Charger_OutPower_Control.Ref;
        if(Data.System_Sample.OutputPort_Charging_Power < ref + 0.05f &&
             Data.System_Sample.OutputPort_Charging_Power > ref - 0.05f)
        {
            Data.System_Control.OutputPort_Charger_OutPower_Control.Ref -= 0.5f;
        }
    }
}

void System_Charging_Program_Start()
{
    Output_Charger_Buck_Switch(ON);
    Output_Resistor_Buck_Switch(ON);
    Output_Bridge_Switch(OFF);
}

void System_Fault_Program_Start()
{
    Output_Charger_Buck_Switch(OFF);
    Output_Resistor_Buck_Switch(OFF);
}

void System_Outputing_Program_Start()
{
    Output_Charger_Buck_Switch(ON);
    Output_Resistor_Buck_Switch(ON);
    Output_Bridge_Switch(ON);

}