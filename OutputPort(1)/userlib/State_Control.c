#include "State_Control.h"

int flag = 0;
float sum_c = 0.0f;
float sum_v = 0.0f;
float sum_rv = 0.0f;
uint16_t switch_flag[2] = {0,0};

void State_Program()
{
    //过压保护 + 充满电后防止电流过大保护
    // if(Data.System_Sample.OutputPort_Charger_Voltage_Value > 8.1f)
    // {
    //     if(Data.System_Sample.OutputPort_Charger_Current_Value > 0.06f )
    //     {
    //         Data.System_Flag.Current_State = System_Fault;
    //         Fault_Monitor = AfterCharging_OverCur;
    //     }
    // }
    // if(Data.System_Sample.OutputPort_Charger_Voltage_Value > 8.5f)
    // {
    //     Data.System_Flag.Current_State = System_Fault;
    //     Fault_Monitor = OverVol_Fault;
    // }

    switch (Data.System_Flag.Current_State)
    {
    case System_Charging:
        System_Charging_Program_Start();
        sum_c += Data.System_Sample.OutputPort_Charger_Current_Value;
        sum_v += Data.System_Sample.OutputPort_Charger_Voltage_Value;
        sum_rv += Data.System_Sample.OutputPort_Resistor_Voltage_Value;
        flag++;
        if(flag == 4)
        {
            TogglePin_C13;
            Data.System_Sample.OutputPort_Charger_Current_Value = sum_c/4.0f;
            Data.System_Sample.OutputPort_Charger_Voltage_Value = sum_v/4.0f;
            Data.System_Sample.OutputPort_Resistor_Voltage_Value = sum_rv/4.0f;
            sum_c = 0.0f;
            sum_v = 0.0f;
            sum_rv = 0.0f;

            Output_ResistorVoltage_LoopRun();
            if(switch_flag[0] < 50)
            {
                if((Data.System_Sample.OutputPort_Resistor_Voltage_Value < 12.0f+0.05f)&&
                   (Data.System_Sample.OutputPort_Resistor_Voltage_Value > 12.0f-0.05f))
                {
                    switch_flag[0]++;
                }
                else 
                {
                    switch_flag[0] = 0;
                }
                
            }

            if(switch_flag[0] >= 50)
            {
                if(switch_flag[1] <10)
                {
                    if(Data.System_Sample.OutputPort_Charger_Voltage_Value > 8.0f)
                        switch_flag[1]++;
                    else if(Data.System_Sample.OutputPort_Charger_Voltage_Value < 8.0f)
                    {
                        if(switch_flag[1])
                            switch_flag[1] --;
                        else
                            switch_flag[1] = 0;
                    }
                    Output_ChargerPower_LoopRun();
                }
                else if (switch_flag[1] >= 10)
                    Output_ChargerVoltage_LoopRun();
            }
            

            hhrtim1.Instance->sTimerxRegs[0].CMP4xR = hhrtim1.Instance->sTimerxRegs[0].CMP1xR > 
                                                      hhrtim1.Instance->sTimerxRegs[0].CMP2xR ?
                                                    0.5f * hhrtim1.Instance->sTimerxRegs[0].CMP2xR:
                                                    0.5f * hhrtim1.Instance->sTimerxRegs[0].CMP1xR;
            flag = 0;
            TogglePin_C13;
        }
        break;

    case System_Outputing:


        break;

    case System_Fault:
        System_Fault_Program_Start();
        break;
    default:
        break;
    }
}