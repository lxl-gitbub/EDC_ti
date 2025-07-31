#ifndef __LASER_USART_H
#define __LASER_USART_H
#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include "string.h"

// �������鳤��
#define USART_LASER_RX_BUF_LEN 100

/******
	�⽲��:
	1.���ڱ��: ����Ҫ�ı䴮��,�����ں궨��ĺ���ĵ�����,ͬʱ�ı��жϺ�������
	2.ͨ��Э��: ʹ��16����: 1.0x12(��ʼ֡)	2.���ݸ���(�߰�λ+�Ͱ�λ) 3.����(ÿ�����ݶ��ɸ߰�λ�͵Ͱ�λ���) 4.0x5B(����֡)
	3.ʹ�ýӿ�: Laser_Loc[4] �� Rect_Loc[4] ���Դ����ĸ�����,�������������� , Laser_error �� Rect_errorΪ�д����,������
	4.�������: ʹ��Laser_error �� Rect_error �õ�������Ϣ: error = -1:��ʼ��	// 0 : ����	// 1 : ��ʼʱû�ҵ�	// 2 : ��ʧ	//  3 : ��֡
	5.��������: 1.ȫ������:����� , ����״̬��־λ , λ������ 2.main:Laser_USART_Init��ʼ��	3. �жϺ�������
******/

// ���ں궨��
#define LASER_UART UART0
#define LASER_INI_IRQn	UART0_INT_IRQn

// ���ݴ洢λ�ú�����(��λ+��λ)
#define Laser_Begin 2
#define Laser_RX_Num 8

#define Rect_Begin  10
#define Rect_RX_Num  8

/* ������غ��� */

// ���⴮�ڳ�ʼ��
void Laser_USART_Init(void) ;
//���ڷ��͵����ַ�
void Laser_send_char(char ch) ;
//���ڷ����ַ���
void Laser_send_string(char* str) ;
/* ���ݴ�����غ��� */

// �õ���Ҫ������
void Get_Laser_Loc( uint16_t Loc[] ) ;
void Get_Rect_Loc( uint16_t Loc[] ) ;
// �ۺϺ���,�����Ƿ���������,���Ա��ּ��
void CanMV_Mode(void) ;
#endif
