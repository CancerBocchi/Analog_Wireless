#include "inc.h"
#include "project_2.h"
/*variable define*/

/*task init*/
void Task_Init()
{
    System_Init();
}

/*task do*/
void Task_Do()
{
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_2))
    {
        Data.System_Flag.Current_State = System_Charging;
        Charging_Falg = ResVolLoopRun;
    }
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_3))
    {
        Data.System_Flag.Current_State = System_Outputing;
        Charging_Falg = NotInCharging;
    }
    if(Cancer_JudgeKey_LTH(GPIOC,GPIO_PIN_1))
    {
        Data.System_Flag.Current_State = System_Stopping;
        Charging_Falg = NotInCharging;
    }
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