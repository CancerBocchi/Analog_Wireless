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

    // fabsf(Data.bus_current)<0.001f && fabsf(Data.resistor_voltage)>2.0f
    // ?  Judgee_Flag++ : Judgee_Flag = 0;
    // if(Judgee_Flag >= 10)
    // {
    //     Judgee_Flag = 10;
    //     Flag_Data = System_Outputing;
    // }
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