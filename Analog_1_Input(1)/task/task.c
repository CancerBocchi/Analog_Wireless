#include "inc.h"

/*variable define*/
float current;
/*task init*/
void Task_Init()
{
    Input_Init();
}

/*task do*/
void Task_Do()
{
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_1))
    {
        Flag_Data.Current_State = System_Outputing;
    }
}

/*IT Function*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)//adc转换完后进入中断
{
    //HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    ADC_Conversion();
    //HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
}

void HAL_HRTIM_RegistersUpdateCallback(HRTIM_HandleTypeDef * hhrtim,uint32_t TimerIdx)//周期回调
{
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_1))
    {
        if(Flag_Data.Current_State != System_Outputing)
        {
            Flag_Data.Current_State = System_Outputing;
        }
        else if(Flag_Data.Current_State != System_Charging)
        {
            Flag_Data.Current_State = System_Charging;
        }
    }

    static uint8_t LoopFre_Flag  = 0;
    static float sum_Iin  = 0.0f;
    static float sum_Vout = 0.0f;

    sum_Iin  += Data.bus_current;
    sum_Vout += Data.resistor_voltage;

    LoopFre_Flag++;

    //static uint8_t Judgee_Flag = 0;

    if(LoopFre_Flag >= 4)
    {
        Data.bus_current      = sum_Iin/4.0f;
        Data.resistor_voltage = sum_Vout/4.0f;
        sum_Iin = 0.0f;
        sum_Vout = 0.0f;
        LoopFre_Flag = 0;

        switch (Flag_Data.Current_State)
        {
        case System_Charging:
            Input_Charging_Program();
            break;
        case System_Outputing:
            Input_Outputing_Program();
            break;
        default:
            break;
        }
    }
    
}