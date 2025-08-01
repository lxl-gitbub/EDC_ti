#ifndef __LASER_USART_H
#define __LASER_USART_H
#include "ti_msp_dl_config.h"
#include <stdbool.h>

// �������鳤��
#define USART_LASER_RX_BUF_LEN 50
// ���ں궨��
#define LASER_UART UART0
#define LASER_INI_IRQn	UART0_INT_IRQn
// ���ݸ�������
#define Laser_Msg_Num 19 	// ֡��ʽ�ĳ��� = K230_MSG_DATA_NUM * 2 + 3 (�ߵ�λ+ͷβ֡+���ݸ����궨)

// ���ݴ洢λ��
#define Laser_Type 1
#define Rect_Type  2

#define Laser_Num 4
#define Rect_Num  4

// ���ڲ�������
void Laser_send_char(char ch);
void Laser_send_string(char* str);
void Laser_USART_Init(void) ;
// ���ݴ���
void Get_Laser_Msg(char uart_data) ;	// �õ���������
void Laser_Loc_Update( void ) ;				// ���¼���λ��
void Fill_Data(uint16_t data[] , int begin_index , int data_num) ;	// �ڲ�����:�����Ӧ����
int Get_K230_data(int DataType , uint16_t receive_data[]) ;					// �õ�ָ����������
// ������
void clear_array(uint8_t arr[], int len) ;
bool Array_Empty(uint16_t arr[], int len) ;
// ����ص� + ������
int Get_Laser_Loc(uint16_t data[]) ;
int Get_Rect_Loc(uint16_t data[]) ;
// ����ģʽ
void K230_UASRT_Mode(void);
#endif
