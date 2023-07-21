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
    static uint8_t Switch_Flag;

    sum_Iin  += Data.bus_current;
    sum_Vout += Data.resistor_voltage;

    LoopFre_Flag++;

    if(LoopFre_Flag >= 4)
    {
        Data.bus_current      = sum_Iin/4.0f;
        Data.resistor_voltage = sum_Vout/4.0f;
        sum_Iin = 0.0f;
        sum_Vout = 0.0f;
        LoopFre_Flag = 0;

        if(Data.bus_current < -0.03f && Switch_Flag < 10)
            Switch_Flag++ ; 
        else
        {
            if(Switch_Flag)
                Switch_Flag--;
            else
                Switch_Flag = 0;
        }

        if(Switch_Flag >= 10)
        {
            Flag_Data.Current_State = System_Outputing;
        }

        switch (Flag_Data.Current_State)
        {
        case System_Charging:
            Input_Charging_Program();
            
            break;
        case System_Outputing:
            Input_Outputing_Program();
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
            break;
        default:
            break;
        }

    }
    
}