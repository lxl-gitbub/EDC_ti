#include "Laser_USART.h"

// ���ڽ������ݺ���(������)
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};

// �������ݴ���
uint8_t new_package[USART_LASER_RX_BUF_LEN];
//���յ��µ�һ�����ݱ�־(0��ʾ���ڸ��� , 1��ʾ�����˿���ʹ��)
uint8_t g_new_package_flag = 0;			

// λ������(�������ݵĹؼ�����)
uint16_t Laser_Loc[10] ;		// ����λ������
uint16_t  Rect_Loc[10]  ;		// ����λ������

// ���ݼ�����
int Laser_error = -1 ;	// -1:��ʼ��	// 0 : ����	// 1 : ��ʼʱû�ҵ�	// 2 : ��ʧ	//  3 : ��֡
int Rect_error	= -1 ;

// �жϳ�ʼ��ʱ�����ҵ���Ӧ����,��ʼ�ҵ�֮���һֱ��0,�����ж�error�е�1�ű�־λ
int Laser_Find_None_Init = 1 ;
int  Rect_Find_None_Init = 1 ; 

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
// ���ݴ���
// �õ����ݸ�λ
uint8_t Get_High_Val_Of_Hex( uint16_t value )
{
	return (value >> 8) & 0xFF;
}
// �õ����ݵ�λ
uint8_t Get_Low_Val_Of_Hex( uint16_t value )
{
	return value & 0xFF;
}
// ��λ+��λת��Ϊ��������
uint16_t Get_Val_Of_Hex( uint8_t high , uint8_t low )
{
	return ((uint16_t)high << 8) | low;
}
void Deal_Rx(uint8_t rxtemp)
{
	// ��ʼ�������ݱ�־λ
	static uint8_t g_start = 0;
	// ������ͷ���ݼ���
  static uint8_t Sensor_Rx_Count = 0;
	// ��ʼλ�ж�
  if(rxtemp == 0x12)
  {  
    g_start = 1;//��ʼ���ձ�־
    USART_LASER_RX_BUF[Sensor_Rx_Count ++] = rxtemp;	// ��ʼλ����
  }
  else
  {
		// �����ڵȴ���ʼλʱ���յ��˴���֡ͷ
    if(g_start == 0)
    {
			// *ʶ��Ϊ��֡*
			Laser_error = 3 ;
			Rect_error  = 3 ;
      return;
    }
    else	// ���ݴ���
    {
      USART_LASER_RX_BUF[Sensor_Rx_Count ++] = rxtemp;
      if(rxtemp == 0x5B)//����
      {
				// �����ж�
        g_start = 0;
        Sensor_Rx_Count = 0;
				// ��ʱ��������ʽ��ֵ��������,���½�������
        memcpy(new_package,USART_LASER_RX_BUF,USART_LASER_RX_BUF_LEN);
				// ���ݸ��±�־λ
        g_new_package_flag = 1;
				//�������
        memset(USART_LASER_RX_BUF,0,USART_LASER_RX_BUF_LEN);
				// *״ָ̬ʾ����*
				Laser_error = 0 ;
				Rect_error  = 0 ;
      }
      if(Sensor_Rx_Count >= USART_LASER_RX_BUF_LEN)//�����쳣
      {
				// *ʶ��Ϊ��֡*
				Laser_error = 3 ;
				Rect_error  = 3 ;
        g_start = 0;
        Sensor_Rx_Count = 0;
        memset(USART_LASER_RX_BUF,0,USART_LASER_RX_BUF_LEN);//�������
      }
    }
  }
}
// �õ�CANMV�����λ�õ�����
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

// ��������Ƿ�Ϊ��
bool Array_Empty(uint16_t arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if ( arr[i] != 0 )
            return false;
    }
    return true;
}

// �������
void Update_Error(uint16_t arr[] ,int len , int *error , int *Loc_Find_None_Init)
{
	// ���û��֡,
	if (*error != 3)
	{
		// ��������ǿյ�,��ʼ�ж�Ϊʲô��
		if ( Array_Empty(arr , len) == true )
		{
			// ������ڳ�ʼ�׶�Laser_Find_None_Init = 1,��ô�����ǳ�ʼʱû�ҵ� , ������ǹ��˳�ʼ��,����ʧ
			if ( *Loc_Find_None_Init == 0 )
				*error = 2 ; // ��ʧ
			else
				*error = 1 ; // ��ʼʱû�ҵ�
		}
		else
		{
			// ״̬����
			*error = 0 ;
			*Loc_Find_None_Init = 0 ;
		}
	}
}

// ����������,���
void CanMV_Mode(void)
{
	// ������յ�����
	char uart_data = DL_UART_Main_receiveData(LASER_UART) ;
	Deal_Rx(uart_data) ;
	// �����ݴ��������з���
	if (g_new_package_flag == 1)
	{
		g_new_package_flag = 0 ;
		Get_Laser_Loc(Laser_Loc) ;
		Get_Rect_Loc(Rect_Loc)  ; 
	}
	// ���±������
	Update_Error(Laser_Loc , Laser_RX_Num / 2 , &Laser_error , &Laser_Find_None_Init) ;
	Update_Error(Rect_Loc  ,  Rect_RX_Num / 2 ,  &Rect_error , &Rect_Find_None_Init ) ;
}
