#ifndef __LASER_USART_H
#define __LASER_USART_H
#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include "string.h"

// 接收数组长度
#define USART_LASER_RX_BUF_LEN 100

/******
	库讲解:
	1.串口编号: 若需要改变串口,将串口宏定义的号码改掉即可,同时改变中断函数名称
	2.通信协议: 使用16进制: 1.0x12(起始帧)	2.数据个数(高八位+低八位) 3.数据(每个数据都由高八位和低八位组成) 4.0x5B(结束帧)
	3.使用接口: Laser_Loc[4] 和 Rect_Loc[4] 各自储存四个数据,在主函数中声明 , Laser_error 和 Rect_error为判错机制,见下文
	4.错误机制: 使用Laser_error 和 Rect_error 得到错误信息: error = -1:初始化	// 0 : 正常	// 1 : 初始时没找到	// 2 : 丢失	//  3 : 掉帧
	5.工程配置: 1.全局配置:导入库 , 错误状态标志位 , 位置数组 2.main:Laser_USART_Init初始化	3. 中断函数声明
******/

// 串口宏定义
#define LASER_UART UART0
#define LASER_INI_IRQn	UART0_INT_IRQn

// 数据存储位置和数量(高位+低位)
#define Laser_Begin 2
#define Laser_RX_Num 8

#define Rect_Begin  10
#define Rect_RX_Num  8

/* 串口相关函数 */

// 激光串口初始化
void Laser_USART_Init(void) ;
//串口发送单个字符
void Laser_send_char(char ch) ;
//串口发送字符串
void Laser_send_string(char* str) ;
/* 数据处理相关函数 */

// 得到想要的数组
void Get_Laser_Loc( uint16_t Loc[] ) ;
void Get_Rect_Loc( uint16_t Loc[] ) ;
// 综合函数,作用是放在主函数,可以保持简洁
void CanMV_Mode(void) ;
#endif
