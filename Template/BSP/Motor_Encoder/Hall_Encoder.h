#ifndef _HALL_ENCODER_H_
#define _HALL_ENCODER_H_

#include "ti_msp_dl_config.h"

typedef enum {
    D,  // ����
    R  // ����
} ENCODER_DIR;

typedef struct {
    volatile long long temp_count; //����ʵʱ����ֵ
    int count;         						 //���ݶ�ʱ��ʱ����µļ���ֵ
    ENCODER_DIR dir;            	 //��ת����
} ENCODER_RES;


void encoder_init(void);
int get_encoder_count(void);
ENCODER_DIR get_encoder_dir(void);
void encoder_update(void);
void timer_init(void);
double getSpeed(void);
#endif