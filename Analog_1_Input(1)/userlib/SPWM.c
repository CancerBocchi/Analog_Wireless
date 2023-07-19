#include "SPWM.h"
#include "inc.h"

#ifdef __HRTIM_H__
void SPWM_Init_Hrtim(spwm* spwm_mode,float Sin_Fre,float PWM_Fre,uint8_t timer)
{
    switch (timer)
    {
    case 'a':
        spwm_mode->TIM_Handle = &(HRTIM1->sTimerxRegs[0]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TA1|HRTIM_OUTPUT_TA2); //通道打开
        HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1, HRTIM_TIMERID_TIMER_A); //开启子定时器A
        break;
    case 'b':
        spwm_mode->TIM_Handle = &(HRTIM1->sTimerxRegs[1]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TB1|HRTIM_OUTPUT_TB2); //通道打开
        HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1, HRTIM_TIMERID_TIMER_B); //开启子定时器
        break;
    case 'e':
        spwm_mode->TIM_Handle = &(HRTIM1->sTimerxRegs[4]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TE1|HRTIM_OUTPUT_TE2); //通道打开
        HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1, HRTIM_TIMERID_TIMER_E); //开启子定时器A
        break;
    case 'f':
        spwm_mode->TIM_Handle = &(HRTIM1->sTimerxRegs[5]);
        HAL_HRTIM_WaveformOutputStart(&hhrtim1,HRTIM_OUTPUT_TF1|HRTIM_OUTPUT_TF2); //通道打开
        HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1, HRTIM_TIMERID_TIMER_F); //开启子定时器A
        break;
    }

    spwm_mode->theta = 0.0f;
    spwm_mode->PWM_Fre = PWM_Fre;
    spwm_mode->SinWave_Fre = Sin_Fre;
    spwm_mode->Step_Raw = 2.0f*PI*spwm_mode->SinWave_Fre/spwm_mode->PWM_Fre;
    spwm_mode->Modulation_Depth = 0.5f;

}
#endif // DEBUG

#ifdef __TIM_H__
void SPWM_Init_TIM(spwm* spwm_mode,float Sin_Fre,float PWM_Fre,uint8_t timer)
{

}
#endif // DEBUG


void AUTO_SPWM(spwm* spwm_mode)
{
#ifdef __HRTIM_H__
    spwm_mode->TIM_Handle->CMP1xR = (uint32_t)((spwm_mode->Modulation_Depth*(arm_sin_f32(spwm_mode->theta)*0.475f+0.5f))*(float)spwm_mode->TIM_Handle->PERxR);
#endif    
    spwm_mode->theta += spwm_mode->Step_Raw;
}

void Pessitive_SPWM(spwm* spwm_mode,float theta)
{
#ifdef __HRTIM_H__
    spwm_mode->TIM_Handle->CMP1xR = (uint32_t)((spwm_mode->Modulation_Depth*(arm_sin_f32(spwm_mode->theta)*0.475f+0.5f))*(float)spwm_mode->TIM_Handle->PERxR);
#endif 

    spwm_mode->theta = theta;
}