#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#include "math.h"

typedef struct{
	float Kp;
	float Ki;
	float Kd;
	float Value_D;
	float Value_I;
	float Error;
	float Past_Error;
	float Output;
	float Ref;

	float Output_Max;
	float Output_Min;
	//积分限幅
	float Value_I_Max;
	//积分隔离
	float Value_I_Iso;
	
}PID_t;

void PID_Init(PID_t* plant,float Kp,float Ki,float Kd);
float PI_Controller(PID_t* plant,float ValueCurrent);
float PID_Controller(PID_t* plant,float ValueCurrent);

#endif