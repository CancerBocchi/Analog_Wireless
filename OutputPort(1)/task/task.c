#include "inc.h"

/*variable define*/

/*task init*/
void Task_Init()
{
    System_Init();
}

/*task do*/
void Task_Do()
{
    
}

/*IT Function*/
void HAL_HRTIM_RegistersUpdateCallback(HRTIM_HandleTypeDef * hhrtim,uint32_t TimerIdx)//周期回调
{
    State_Program();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)//adc转换完后进入中断
{
    ADC_Conversion_Program();
}