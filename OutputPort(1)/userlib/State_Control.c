#include "State_Control.h"

Charging_State Charging_Falg;

void System_Charging_Program()
{  
        if(Charging_Falg == NotInCharging)
            Charging_Falg = ResVolLoopRun;

        static int flag = 0;
        static float sum_c = 0.0f;
        static float sum_v = 0.0f;
        static float sum_rv = 0.0f;
        static uint16_t switch_flag[2] = {0,0};

        System_Charging_Program_Start();

        sum_c += Data.System_Sample.OutputPort_Charger_Current_Value;
        sum_v += Data.System_Sample.OutputPort_Charger_Voltage_Value;
        sum_rv += Data.System_Sample.OutputPort_Resistor_Voltage_Value;
        flag++;
        if(flag == 4)
        {
            Data.System_Sample.OutputPort_Charger_Current_Value = sum_c/4.0f;
            Data.System_Sample.OutputPort_Charger_Voltage_Value = sum_v/4.0f;
            Data.System_Sample.OutputPort_Resistor_Voltage_Value = sum_rv/4.0f;
            sum_c = 0.0f;
            sum_v = 0.0f;
            sum_rv = 0.0f;
            
            if(switch_flag[0] < 50)
            {
                if((Data.System_Sample.OutputPort_Resistor_Voltage_Value < 12.0f+0.05f)&&
                   (Data.System_Sample.OutputPort_Resistor_Voltage_Value > 12.0f-0.05f))
                    switch_flag[0]++;
                else 
                    switch_flag[0] = 0;
            }

            if(switch_flag[0] >= 50)
            {
                Charging_Falg = ChaPowLoopRun;
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
                }
                else if (switch_flag[1] >= 10)
                Charging_Falg = ChaVolLoopRun;
            }
            
            Output_ResistorVoltage_LoopRun();

            if(Charging_Falg == ChaPowLoopRun)
                Output_ChargerPower_LoopRun();
            else if(Charging_Falg == ChaVolLoopRun)
                Output_ChargerVoltage_LoopRun();


            hhrtim1.Instance->sTimerxRegs[0].CMP4xR = hhrtim1.Instance->sTimerxRegs[0].CMP1xR > 
                                                      hhrtim1.Instance->sTimerxRegs[0].CMP2xR ?
                                                    0.5f * hhrtim1.Instance->sTimerxRegs[0].CMP2xR:
                                                    0.5f * hhrtim1.Instance->sTimerxRegs[0].CMP1xR;
            flag = 0;
        }
}

void System_Stopping_Program()
{
    // Output_Charger_Buck_Switch(OFF);
    //Output_Resistor_Buck_Switch(OFF);
}

// void System_Judging_Program()
// {
//     static uint8_t switch_Flag[2];
// //充电充满时限流
//     if(Data.System_Flag.Current_State == System_Charging &&
//        Charging_Falg == ChaVolLoopRun)
//     {
//         Data.System_Sample.OutputPort_Charger_Current_Value < 0.05f ?
//         switch_Flag[0]++ : (switch_Flag[0] = 0);

//         if(switch_Flag[0] >= 40)
//         {
//             Data.System_Flag.Current_State = System_Stopping;
//             Charging_Falg = NotInCharging;
//         }
//     }
// }

void System_Outputing_Program()
{
    static uint8_t Fre_FLag = 0;
    Fre_FLag++;
    //固定原12V占空比
    System_Outputing_Program_Start();
    hhrtim1.Instance->sTimerxRegs[0].CMP2xR = 0.1 * hhrtim1.Instance->sTimerxRegs[0].PERxR;
    //电容稳定功率放电
    if(Fre_FLag >= 4)
    {
        Output_ChargerOutPow_LoopRun();
    }
    
}

void State_Program()
{
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_1))
    {
        if(Data.System_Flag.Current_State != System_Outputing)
        {
            Data.System_Flag.Current_State = System_Outputing;
            Charging_Falg = NotInCharging;
        }
        else if(Data.System_Flag.Current_State != System_Charging)
        {
            Data.System_Flag.Current_State = System_Charging;
            Charging_Falg = ResVolLoopRun;
        }
    }
    //System_Judging_Program();
    switch (Data.System_Flag.Current_State)
    {
    case System_Charging:
        System_Charging_Program();
        break;

    case System_Stopping:
        System_Stopping_Program();
        break;

    case System_Outputing:
        System_Outputing_Program();
        break;

    case System_Fault:
        System_Fault_Program_Start();
        break;
    }
}