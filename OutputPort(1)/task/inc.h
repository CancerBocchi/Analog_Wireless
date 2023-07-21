#ifndef __INC_H__
#define __INC_H__

/*hal 模块*/
#include "gpio.h"
#include "main.h"
#include "arm_math.h"
#include "hrtim.h"
#include "adc.h"

/*基本模块*/
#include "mylib.h"
#include "SEGGER_RTT.h"

/*标准库模块*/
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/*常用算法模块*/
#include "PID.h"

#include "Switch.h"
#include "project_2.h"

void Task_Do();
void Task_Init();

#endif // !__INC_H__