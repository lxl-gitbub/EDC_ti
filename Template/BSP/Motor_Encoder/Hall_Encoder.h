#ifndef HALL_ENCODER_H
#define HALL_ENCODER_H

#include "ti_msp_dl_config.h"

#define PI 3.1415926535 // Pi constant
#define MAXENCODERNUM 10 // The maximum number of encoders that can be used in this code

typedef struct{
	int wheelLength; // The length of wheel in mm
	int ppr; // The Pulses Per Revolution of encoder
}EncoderParam;

typedef struct {
	int speed; // The speed of wheel in m/s
	float dis; // The distance traveled by the wheel in m
	GPTIMER_Regs *timer; // The timer instance used for QEI
	GPTIMER_Regs *realTimer; // The timer used to measure real time(reload time)
	uint32_t reloadFre; // The frequency of the timer used to measure real time
	EncoderParam param; // The parameter of the encoder, like wheel length and ppr
	int16_t prev_cnt;
} EncoderSpeed;


#ifdef __cplusplus
extern "C" {
#endif

// ...existing code...

void EncoderInit(GPTIMER_Regs *Timer, GPTIMER_Regs *realTimer, 
	int wheelLength, int ppr);
// Initialize the encoder with the given parameters
void EncoderParamInit(EncoderParam *param, int wheelLength, int ppr);
void LRInit(GPTIMER_Regs *LTimer, GPTIMER_Regs *RTimer,
			int wheelLength, int ppr, // The last two parameters are for encoder parameters	
			GPTIMER_Regs *realTimer);

int getTimerQEICnt(GPTIMER_Regs *timer);
void UpdateSpeed(int i);
void UpdateAllSpeed(void);

double getSpeed(int index);
double getDis(int index);
double cSpeed();
double lSpeed();
double rSpeed();
double lDis();
double rDis();

double StoDis(EncoderSpeed e);

uint32_t ReloadTime(GPTIMER_Regs* tim);

#endif // HALL_ENCODER_H
