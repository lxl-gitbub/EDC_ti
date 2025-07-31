#ifndef __LASER_USART_H
#define __LASER_USART_H
// 接受数组长度
#define USART_LASER_RX_BUF_LEN 25
// 串口宏定义
#define LASER_UART UART2
#define LASER_INI_IRQn	UART2_INT_IRQn
#define Laser_Msg_Num 6	// 帧格式的长度
//#define LASER_DEBUG

void Laser_send_char(char ch);
void Laser_send_string(char* str);
void Laser_USART_Init(void) ;
void Laser_UASRT_IRQHandler(void) ;
void Laser_Loc_Update(char uart_data) ;
void Laser_Ask_for_Loc(void) ;

#endif
