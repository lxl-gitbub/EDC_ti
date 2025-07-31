#include "ti_msp_dl_config.h"
#include "Laser_USART.h"

// 激光串口接收数组(初筛):激光位置返回帧: 0x12 target_x target_y real_x real_y 0x5B
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};
// 激光串口位置数组(含值):target_x 	target_y 	real_x 	real_y
int Laser_Loc[USART_LASER_RX_BUF_LEN] = {0};

// ****************串口相关*******************
// 激光串口初始化
void Laser_USART_Init(void)
{
		//清除串口中断标志
    NVIC_ClearPendingIRQ(LASER_INI_IRQn);
    //使能串口中断
    NVIC_EnableIRQ(LASER_INI_IRQn);
}

//串口发送单个字符
void Laser_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(LASER_UART) == true );
    //发送单个字符
    DL_UART_Main_transmitData(LASER_UART, ch);
}

//串口发送字符串
void Laser_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        Laser_send_char(*str++);
    }
}
// ****************激光相关*******************

// 串口要求接收激光数据
void Laser_Ask_for_Loc(void)
{
	// 串口不忙
	if (DL_UART_isBusy(LASER_UART) == false)
	{
		//发送单个字符:L(LASER)
    DL_UART_Main_transmitData(LASER_UART, 'L');
	}
}

// 串口处理数据
void Laser_Loc_Update(char uart_data)
{
		static int Laser_Rx_Count = 0 ;
		// 存储收到的数据并检查
		if (Laser_Rx_Count == 0)
		{
			if (uart_data != 0x12)
			{
				Laser_Rx_Count = 0;
			}
			else
			{
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
			}
		}
		else if (Laser_Rx_Count == 5)
		{
			if (uart_data != 0x5B)
			{
				Laser_Rx_Count = 0;
			}
			else			
			{
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
				// 处理结果
				for (int i = 0 ; i <= Laser_Msg_Num - 1 - 2 ; i ++ )
				{
					Laser_Loc[i] = USART_LASER_RX_BUF[i + 1] ;
				}
			}
		}
		else if (Laser_Rx_Count > 0 && Laser_Rx_Count < Laser_Msg_Num - 1 )
		{
			if ( uart_data < 0 || uart_data > 160)
			{
				Laser_Rx_Count = 0;
			}
			else 
			{
				USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
			}
		}
		else
	  {
			Laser_Rx_Count = 0 ;
		}
}

