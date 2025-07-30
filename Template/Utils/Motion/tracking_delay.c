#include "tracking_delay.h"

// HAL���Ѿ�������SysTick�ĳ�ʼ��������delay_initͨ������Ҫ�ˡ�
// ������뱣���������Խ���������ջ�����������ʼ����
void delay_init(void)
{
    // HAL���ʼ����SysTick�Ѿ����ú��ˣ�ͨ������Ҫ����ĳ�ʼ��
}

/**********************************************************
** ������: delay_ms
** ��������: ��ʱnms
** �������: nms
** �������: ��
** ˵����ֱ��ʹ��HAL_Delay
***********************************************************/
void delay_ms(uint16_t nms)
{
    HAL_Delay(nms);
}

/**********************************************************
** ������: delay_us
** ��������: ��ʱnus��nusΪҪ��ʱ��us��.
** �������: nus
** �������: ��
** ˵����HAL��û��ֱ�ӵ�΢����ʱ��������Ҫ�Լ�ʵ�֡�
        �����ṩһ������__NOP()�ļ�ʵ�֣������Ȳ��������Ż�Ӱ�졣
        ����ȷ��ʵ����Ҫʹ�ö�ʱ��������TIM��DWT��
***********************************************************/
void delay_us(uint32_t nus)
{
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_per_us = SystemCoreClock / 1000000;
    uint32_t ticks_to_wait = nus * ticks_per_us;
    uint32_t current_tick;
    uint32_t elapsed_ticks = 0;

    if (ticks_to_wait == 0) return;

    do {
        current_tick = SysTick->VAL;
        if (current_tick > start_tick) { // ��������LOAD��0�����������ǵ�����
            elapsed_ticks += (start_tick + SysTick->LOAD + 1 - current_tick);
        } else {
            elapsed_ticks += (start_tick - current_tick);
        }
        start_tick = current_tick;
    } while (elapsed_ticks < ticks_to_wait);
}