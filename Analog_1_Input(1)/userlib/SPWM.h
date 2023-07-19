#ifndef __SPWM_H__
#define __SPWM_H__
#include "inc.h"


typedef struct SPWM
{
    /* data */
#ifdef __HRTIM_H__
    HRTIM_Timerx_TypeDef    *TIM_Handle; //高分辨率定时器接口
#endif
#ifdef __TIM_H__
    TIM_HandleTypeDef       *htim;
#endif

    float                   SinWave_Fre; //调制得到的正弦波的频率
    float                   PWM_Fre;     //产生的PWM的频率
    double                  Step_Raw;    //单位步进 0-2*pi 每次进入中断后theta的步进
    float                   Modulation_Depth;//调制深度

    float                   theta;       //当前spwm输出的角度
}spwm;


#ifdef __HRTIM_H__
void SPWM_Init_Hrtim(spwm* spwm_mode,float Sin_Fre,float PWM_Fre,uint8_t timer);
#endif // DEBUG



void Pessitive_SPWM(spwm* spwm_mode,float theta);

#endif


















#define __SPWM_H__