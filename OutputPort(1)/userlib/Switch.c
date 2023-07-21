#include "Switch.h"

void Output_Charger_Buck_Switch(bool OnOff)
{
    if(OnOff == ON)
    {
        if(!Data.System_Flag.OutputPort_Charger_Buck_State)
        {
            Data.System_Flag.OutputPort_Charger_Buck_State = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TA1);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Data.System_Flag.OutputPort_Charger_Buck_State)
        {
            Data.System_Flag.OutputPort_Charger_Buck_State = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA1);
        }
        return;
    }
}

void Output_Resistor_Buck_Switch(bool OnOff)
{
    if(OnOff == ON)
    {
        if(!Data.System_Flag.OutputPort_Resistor_Buck_State)
        {
            Data.System_Flag.OutputPort_Resistor_Buck_State = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TA2);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Data.System_Flag.OutputPort_Resistor_Buck_State)
        {
            Data.System_Flag.OutputPort_Resistor_Buck_State = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TA2);
        }
        return;
    }
}

void Output_Bridge_Switch(bool OnOff)
{
        if(OnOff == ON)
    {
        if(!Data.System_Flag.OutputPort_Bridge_State)
        {
            Data.System_Flag.OutputPort_Bridge_State = ON;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
            HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TE1|HRTIM_OUTPUT_TE2);
        }
        return;
    }
    else if(OnOff == OFF)
    {
        if(Data.System_Flag.OutputPort_Bridge_State)
        {
            Data.System_Flag.OutputPort_Bridge_State = OFF;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2);
            HAL_HRTIM_WaveformOutputStop(&hhrtim1,HRTIM_OUTPUT_TE1|HRTIM_OUTPUT_TE2);
        }
        return;
    }
}