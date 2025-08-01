/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "AllHeader.h"

//编码器未测试(定时器已测试
//灰度传感器未移植
//OLED以及状态机
//普通电机左右未测试
// 激光位置串口通信测试
extern uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] ;
extern int Laser_Loc[USART_LASER_RX_BUF_LEN] ;

//灰度传感器数据变量
uint8_t Digtal[8];

// 测试区
int a;
uint32_t tick;
char message[50];

int main(void)
{
	SYSCFG_DL_init();
	
	//编码器初始化
	encoder_init();

	//定时器初始化
	timer_init();
	DL_TimerG_startCounter(TIMER_0_INST);
	
	// OLED Init
	OLED_Init() ;
	//Car1 Init
	MECInit();
//	LSet(0);
//	RSet(0);
	//Laser Init
//	Laser_USART_Init();
//	Laser_Ask_for_Loc();
//	menu_init();
//	menu_begin();
	proB_1();
	
	while (1) 
	{
		LSet(100);
//		getTrackingSensorData(Digtal);
//		snprintf(message, sizeof(message), "%.5f",getSpeed());
//		OLED_ShowString(0,4, message, 8);
//		Delay_ms(40);
	}
}

// 激光串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(LASER_UART) )
    {
			  //如果是接收中断
        case DL_UART_IIDX_RX:
					  // 处理数据(直接不要变量接收了,直接函数return为参数)
						Laser_Loc_Update( DL_UART_Main_receiveData(LASER_UART) ) ;
				    //重新要求发送激光数据
            Laser_Ask_for_Loc();
						#ifdef LASER_DEBUG
						Laser_send_char(DL_UART_Main_receiveData(LASER_UART)) ;	// 看看回显,我发现打开这个debug之后函数发送回显会丢失第三个数据
						#endif
            break;
        default://其他的串口中断
            break;
    }
}

void SysTick_Handler(void)
{
		// 自分频
		static int k = 0 ;
		k ++ ;
	tick ++;
		if (k == 1000)
		{
			k = 0 ;
		}
}


