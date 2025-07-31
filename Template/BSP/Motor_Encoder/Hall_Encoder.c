#include "Hall_Encoder.h"

EncoderSpeed encoders[MAXENCODERNUM]; // Array to store encoder data
int numEncoders = 0; // Number of encoders initialized
int LRFlag = 0; // Flag to indicate if left and right encoders are initialized

void EncoderInit(GPTIMER_Regs *Timer, GPTIMER_Regs *realTimer,
	int wheelLength, int ppr)
	//set global variables for encoders
{
	if (numEncoders >= MAXENCODERNUM)
	{
		return; // Maximum number of encoders reached
	}

	encoders[numEncoders].timer = Timer;
	encoders[numEncoders].realTimer = realTimer;
	
	// 启动QEI定时器计数器
	DL_TimerG_startCounter(Timer);
	
	// 清除并启动实时定时器中断
	DL_TimerG_clearInterruptStatus(realTimer, DL_TIMER_INTERRUPT_ZERO_EVENT);
	DL_TimerG_enableInterrupt(realTimer, DL_TIMER_INTERRUPT_ZERO_EVENT);
	DL_TimerG_startCounter(realTimer);

	encoders[numEncoders].reloadFre = 100; // Set reload frequency to 100Hz
	encoders[numEncoders].speed = 0; // Initialize speed to 0
	encoders[numEncoders].dis = 0.0; // Initialize distance to 0
	// 重置QEI计数器（如果有这个功能的话，否则注释掉）
	// DL_TimerG_setCounterValue(Timer, 0);
	encoders[numEncoders].prev_cnt = DL_Timer_getTimerCount(Timer); // 初始化 prev_cnt
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
   NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);


	EncoderParamInit(&encoders[numEncoders].param, wheelLength, ppr);
	numEncoders++;
	return;
}

void EncoderParamInit(EncoderParam *param, int wheelLength, int ppr)
	//set the parameter of the encoder
{
	if (param == NULL)
	{
		return; // Parameter pointer is NULL
	}
	param->wheelLength = wheelLength; // Set wheel length in mm
	param->ppr = ppr; // Set Pulses Per Revolution
}

void LRInit(GPTIMER_Regs *LTimer, GPTIMER_Regs *RTimer,
			int wheelLength, int ppr, // The last two parameters are for encoder parameters
			GPTIMER_Regs *realTimer)
	//set global variables for left and right encoders
{
	EncoderInit(LTimer, realTimer, wheelLength, ppr);
	EncoderInit(RTimer, realTimer, wheelLength, ppr);
	
	LRFlag = 1; // Set flag to indicate left and right encoders are initialized
}

void UpdateSpeed(int i)
	//get the speed of the motor
	//E is the encoder speed struct, reload_tim is the timer used to measure real time
{	
	if (encoders[i].timer == NULL || encoders[i].realTimer == NULL)
	{
		return; // Timer not initialized
	}

	int16_t current_cnt = DL_Timer_getTimerCount(encoders[i].timer); // 获取当前计数值
        // 计算速度差值（考虑16位定时器溢出/下溢）
        // 假设使用16位定时器且计数值在int16_t范围内
	 encoders[i].speed = (int16_t)(current_cnt - encoders[i].prev_cnt);

	 encoders[i].prev_cnt = current_cnt; // 保存当前计数值，供下次调用使用
	 encoders[i].dis += StoDis(encoders[i]);
	 // 更新距离 traveled by the wheel
}

void UpdateAllSpeed(void)
		//update all encoders' speed
{
	for (int i = 0; i < numEncoders; i++)
	{
		UpdateSpeed(i);
	}
}

double StoDis(EncoderSpeed e)
{
	return e.speed * e.param.wheelLength * 1e-3 * PI 
	/ (e.param.ppr * 2);
	// Compute the distance traveled by the wheel
}

//Get wheels' speed with the unit of m/s
//'+' means forwards and '-' means backwards.
//When you are using the code, please connect PE9 to E1A, PE11 to E1B, 
double getSpeed(int index)
{
	return encoders[index].speed * 0.79748 ;
		//4.394215799e-3;
		// Left number is computed by (length of the circle) * (frequence of the clock / (leftSpeed add number for every single circle
		// which is (6.6e-2 * pi * (240e6 / ((65535 + 1) * (239 + 1)) / (360 * 2))
}

double getDis(int index)
{
	return encoders[index].dis; // Return the distance traveled by the wheel
	// The distance is already updated in UpdateSpeed function
}

//Functions below are speciafic for left and right encoders
double cSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left and right encoders initialized
	}
	return (getSpeed(0) + getSpeed(1)) / 2.0; // Average speed of left and right encoders
}
double lSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left encoder initialized
	}
	return getSpeed(0); // Speed of left encoder
}
double rSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No right encoder initialized
	}
	return getSpeed(1); // Speed of right encoder
}

double lDis()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left encoder initialized
	}
	return getDis(0); // Distance traveled by left wheel
}
double rDis()
{
	if(LRFlag == 0)
	{
		return 0.0; // No right encoder initialized
	}
	return getDis(1); // Distance traveled by right wheel
}
