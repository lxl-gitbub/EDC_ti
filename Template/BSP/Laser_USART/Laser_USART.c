#include "ti_msp_dl_config.h"
#include "Laser_USART.h"

// ���⴮�ڽ�������(��ɸ):����λ�÷���֡: 0x12 target_x target_y real_x real_y 0x5B
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};
// ���⴮��λ������(��ֵ):target_x 	target_y 	real_x 	real_y
int Laser_Loc[USART_LASER_RX_BUF_LEN] = {0};

// ****************�������*******************
// ���⴮�ڳ�ʼ��
void Laser_USART_Init(void)
{
		//��������жϱ�־
    NVIC_ClearPendingIRQ(LASER_INI_IRQn);
    //ʹ�ܴ����ж�
    NVIC_EnableIRQ(LASER_INI_IRQn);
}

//���ڷ��͵����ַ�
void Laser_send_char(char ch)
{
    //������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
    while( DL_UART_isBusy(LASER_UART) == true );
    //���͵����ַ�
    DL_UART_Main_transmitData(LASER_UART, ch);
}

//���ڷ����ַ���
void Laser_send_string(char* str)
{
    //��ǰ�ַ�����ַ���ڽ�β ���� �ַ����׵�ַ��Ϊ��
    while(*str!=0&&str!=0)
    {
        //�����ַ����׵�ַ�е��ַ��������ڷ������֮���׵�ַ����
        Laser_send_char(*str++);
    }
}
// ****************�������*******************

// ����Ҫ����ռ�������
void Laser_Ask_for_Loc(void)
{
	// ���ڲ�æ
	if (DL_UART_isBusy(LASER_UART) == false)
	{
		//���͵����ַ�:L(LASER)
    DL_UART_Main_transmitData(LASER_UART, 'L');
	}
}

// ���ڴ�������
void Laser_Loc_Update(char uart_data)
{
		static int Laser_Rx_Count = 0 ;
		// �洢�յ������ݲ����
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
				// ������
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

