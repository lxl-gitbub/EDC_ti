#include "Laser_USART.h"

// 串口接收数据函数(缓冲区)
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};

// 单个数据处理
uint8_t new_package[USART_LASER_RX_BUF_LEN];
//接收到新的一包数据标志(0表示还在更新 , 1表示更新了可以使用)
uint8_t g_new_package_flag = 0;			

// 位置数组(传输数据的关键数组)
uint16_t Laser_Loc[10] ;		// 激光位置数组
uint16_t  Rect_Loc[10]  ;		// 矩形位置数组

// 数据检错机制
int Laser_error = -1 ;	// -1:初始化	// 0 : 正常	// 1 : 初始时没找到	// 2 : 丢失	//  3 : 掉帧
int Rect_error	= -1 ;

// 判断初始化时有无找到相应物体,初始找到之后就一直是0,用于判断error中的1号标志位
int Laser_Find_None_Init = 1 ;
int  Rect_Find_None_Init = 1 ; 

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
// 数据处理
// 得到数据高位
uint8_t Get_High_Val_Of_Hex( uint16_t value )
{
	return (value >> 8) & 0xFF;
}
// 得到数据低位
uint8_t Get_Low_Val_Of_Hex( uint16_t value )
{
	return value & 0xFF;
}
// 高位+低位转换为完整数据
uint16_t Get_Val_Of_Hex( uint8_t high , uint8_t low )
{
	return ((uint16_t)high << 8) | low;
}
void Deal_Rx(uint8_t rxtemp)
{
	// 开始处理数据标志位
	static uint8_t g_start = 0;
	// 对摄像头数据计数
  static uint8_t Sensor_Rx_Count = 0;
	// 起始位判断
  if(rxtemp == 0x12)
  {  
    g_start = 1;//开始接收标志
    USART_LASER_RX_BUF[Sensor_Rx_Count ++] = rxtemp;	// 起始位接收
  }
  else
  {
		// 表明在等待起始位时接收到了错误帧头
    if(g_start == 0)
    {
			// *识别为掉帧*
			Laser_error = 3 ;
			Rect_error  = 3 ;
      return;
    }
    else	// 数据处理
    {
      USART_LASER_RX_BUF[Sensor_Rx_Count ++] = rxtemp;
      if(rxtemp == 0x5B)//结束
      {
				// 重置判断
        g_start = 0;
        Sensor_Rx_Count = 0;
				// 临时缓冲区正式赋值到接受区,更新接收区域
        memcpy(new_package,USART_LASER_RX_BUF,USART_LASER_RX_BUF_LEN);
				// 数据更新标志位
        g_new_package_flag = 1;
				//清空数据
        memset(USART_LASER_RX_BUF,0,USART_LASER_RX_BUF_LEN);
				// *状态指示正常*
				Laser_error = 0 ;
				Rect_error  = 0 ;
      }
      if(Sensor_Rx_Count >= USART_LASER_RX_BUF_LEN)//数据异常
      {
				// *识别为掉帧*
				Laser_error = 3 ;
				Rect_error  = 3 ;
        g_start = 0;
        Sensor_Rx_Count = 0;
        memset(USART_LASER_RX_BUF,0,USART_LASER_RX_BUF_LEN);//清空数据
      }
    }
  }
}
// 得到CANMV的相关位置的数据
void Get_CanMV_Loc(int Begin_Index_Of_Buf , int Data_Num , uint16_t Loc[] )
{
	for (int i = 0 ; i < Data_Num ; i += 2)
	{
		Loc[i / 2] = Get_Val_Of_Hex( new_package[i + Begin_Index_Of_Buf], new_package[i + Begin_Index_Of_Buf + 1] );
	}
}

void Get_Laser_Loc( uint16_t Loc[] )
{
	Get_CanMV_Loc(Laser_Begin , Laser_RX_Num , Loc ) ;
}

void Get_Rect_Loc( uint16_t Loc[] )
{
	Get_CanMV_Loc(Rect_Begin , Rect_RX_Num , Loc ) ;
}

// 检测数组是否为空
bool Array_Empty(uint16_t arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if ( arr[i] != 0 )
            return false;
    }
    return true;
}

// 报错机制
void Update_Error(uint16_t arr[] ,int len , int *error , int *Loc_Find_None_Init)
{
	// 如果没掉帧,
	if (*error != 3)
	{
		// 如果数组是空的,开始判断为什么空
		if ( Array_Empty(arr , len) == true )
		{
			// 如果还在初始阶段Laser_Find_None_Init = 1,那么表明是初始时没找到 , 否则就是过了初始化,即丢失
			if ( *Loc_Find_None_Init == 0 )
				*error = 2 ; // 丢失
			else
				*error = 1 ; // 初始时没找到
		}
		else
		{
			// 状态正常
			*error = 0 ;
			*Loc_Find_None_Init = 0 ;
		}
	}
}

// 放在主函数,简洁
void CanMV_Mode(void)
{
	// 处理接收的数据
	char uart_data = DL_UART_Main_receiveData(LASER_UART) ;
	Deal_Rx(uart_data) ;
	// 新数据处理完后进行分配
	if (g_new_package_flag == 1)
	{
		g_new_package_flag = 0 ;
		Get_Laser_Loc(Laser_Loc) ;
		Get_Rect_Loc(Rect_Loc)  ; 
	}
	// 更新报错机制
	Update_Error(Laser_Loc , Laser_RX_Num / 2 , &Laser_error , &Laser_Find_None_Init) ;
	Update_Error(Rect_Loc  ,  Rect_RX_Num / 2 ,  &Rect_error , &Rect_Find_None_Init ) ;
}
