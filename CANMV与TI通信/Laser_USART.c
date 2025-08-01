#include "Laser_USART.h"

/*
1.���ݸ�ʽ ���⴮�ڽ�������(��ɸ):����λ�÷���֡: 0x12 ���ݸ���-> ��������(��λ�͵�λ) 0x5B
2.��������:
3.���÷���:
4.ע���:
*/

// ���ڽ������ݺ���
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};
// ���⴮��λ������(��ֵ)
int Laser_Loc[USART_LASER_RX_BUF_LEN] = {0};

// ********************�ɱ�����********************
// ��־λ����
int Laser_flag = 1 ; // flag = 0 : ����	// flag = 1 : û�ҵ�(��ʼ״̬)	// flag = 2 : ��ʧ		// flag = 3 : ��֡(�϶���һ���֡)
int Rect_flag  = 1 ; // flag = 0 : ����	// flag = 1 : û�ҵ�(��ʼ״̬)	// flag = 2 : ��ʧ		// flag = 3 : ��֡(�϶���һ���֡)

uint16_t Laser_Location[20] = {0};		// �洢��������:target_x , target_y , real_x , real_y
uint16_t  Rect_Location[20] = {0};		// �洢��������:
int error_Laser = -1;
int error_Rect = -1;
// ********************�ɱ�����********************

// �������ݽ��մ���
int Laser_Rx_Count = 0 ;
bool is_RX_Complete = false ;


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

// *************�ص�����******************

// ���ڴ�������ģʽ
void K230_UASRT_Mode(void)
{
	// ������ˢ�´���״̬
	error_Laser = Laser_flag;
	error_Rect  = Rect_flag;
	// ��������(ֱ�Ӳ�Ҫ����������,ֱ�Ӻ���returnΪ����)
	Get_Laser_Msg(DL_UART_Main_receiveData(LASER_UART)) ;
	if (is_RX_Complete == true)
		Laser_Loc_Update() ;
	error_Laser = Get_Laser_Loc(Laser_Location) ;
	error_Rect = Get_Rect_Loc(Rect_Location) ;
}

// ���ڴ�������
void Get_Laser_Msg(char uart_data)
{
		// ����ͷ֡���ݲ����
		if (Laser_Rx_Count == 0)
		{
			if (uart_data != 0x12)
			{
				is_RX_Complete = false ;
				Laser_Rx_Count = 0;
				Laser_flag = 3 ;	// ��֡
				Rect_flag = 3 ;		// ��֡
			}
			else
			{
				// ��ȷ֡ͷ���ˣ����������֡״̬���ؼ���
        Laser_flag = (Laser_flag == 3) ? 0 : Laser_flag;
        Rect_flag  = (Rect_flag == 3) ? 0 : Rect_flag;
				USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
				
			}
		}
		else if (Laser_Rx_Count == Laser_Msg_Num - 1)	// ����β֡���ݲ����
		{
			if (uart_data != 0x5B)
			{
				Laser_Rx_Count = 0;
				Laser_flag = 3 ;  // ��֡
				Rect_flag = 3 ;		// ��֡
				is_RX_Complete = false ;
			}
			else			
			{
				USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
				is_RX_Complete = true ;
			}
		}
		else if (Laser_Rx_Count > 0 && Laser_Rx_Count < Laser_Msg_Num - 1 )
		{
			USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
			Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
		}
		else
	 {
			Laser_Rx_Count = 0 ;
			Laser_flag = 3 ;  // ��֡
			Rect_flag = 3 ;		// ��֡
		}
	 if (Laser_flag == 3 || Rect_flag == 3)
		 clear_array( USART_LASER_RX_BUF , USART_LASER_RX_BUF_LEN ) ;		 // ��֡����ս�������
}

// ���󷵻غ���1
int Get_Laser_Loc(uint16_t data[])
{
	// �����ʼ��û�ҵ�״̬
	static int Find_Laser_None = 1 ;
	// �õ��������
	Get_K230_data(Laser_Type , data) ;
	// �Ժ���Ҳ�����г�ʼ��û�ҵ��������
	if (Laser_flag != 1)
		Find_Laser_None = 0 ;
	// ��֡��������ֵ��Զ����
	if (Laser_flag == 3)
		return Laser_flag ; 
	// ��������Ϊ�յ����
	if ( Array_Empty(data , Laser_Num) ) 
	{
		// ��һ�ζ�ʧ�ж�Ϊ��ʼ��û�ҵ�
		if (Find_Laser_None == 1)
			Laser_flag = 1 ;
		else		// �Ժ��ٶ�ʧ�ж�Ϊ׷�ٶ�ʧ
			Laser_flag = 2 ;
	}
	else
		Laser_flag = 0 ;	// �����ж�Ϊ����(ǰ����û�е�֡)
	return Laser_flag ;
}

// ���󷵻غ���2
int Get_Rect_Loc(uint16_t data[])
{
	// �����ʼ��û�ҵ�״̬
	static int Find_Rect_None = 1 ;
	// �õ��������
	Get_K230_data(Rect_Type , data) ;
	// ��֡��������ֵ��Զ����
	if (Laser_flag == 3)
		return Laser_flag ; 
	// �Ժ���Ҳ�����г�ʼ��û�ҵ��������
	if (Rect_flag != 1)
		Find_Rect_None = 0 ;
	if ( Array_Empty(data , Rect_Num) ) 
	{
		// ��һ�ζ�ʧ�ж�Ϊ��ʼ��û�ҵ�
		if (Find_Rect_None == 1)
		{
			DL_GPIO_togglePins(LED_PORT , LED_LED0_PIN ) ;
			Rect_flag = 1 ;
		}
		else		// �Ժ��ٶ�ʧ�ж�Ϊ��ʧ
			Rect_flag = 2 ;
	}
	else
		Laser_flag = 0 ;	// �����ж�Ϊ����
	return Laser_flag ;
}

// �õ�����ͷָ������
int Get_K230_data(int DataType , uint16_t receive_data[])
{
	int begin_index = 0 ;
	int data_num = 0 ;
	switch (DataType)
	{
		case Laser_Type:
			begin_index = 0 ;
			data_num = Laser_Num ;
			break ;
		case Rect_Type :
			begin_index = 4 ;
			data_num = Rect_Num ;
			break ;
		default:
			return 0 ;
	}
	Fill_Data(receive_data , begin_index , data_num ) ;
	return 1 ;
}

// *********************************************************

// ����˳���ȸ�λ���ٵ�λ
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
// ���¼�����Ϣ��ַ
void Laser_Loc_Update( void )
{
	// 0:��ʼ֡(0x12)	1:���ݸ���(ż��)	 (ż�� + ���� : ����λ)	(���һλ:����֡(0x5B))
	int data_num = USART_LASER_RX_BUF[1] ; // �õ����ݸ���,�����֡��,�����������,��ô���ݸ�������0,��ô��Ȼԭ���鲻��
	for (	int i = 0  ; i < data_num ; i += 2 )
	{
		Laser_Loc[i / 2] = Get_Val_Of_Hex(USART_LASER_RX_BUF[i + 2], USART_LASER_RX_BUF[i + 3]);
	}
}

// ����������
void Fill_Data(uint16_t data[] , int begin_index , int data_num)
{
	for (int i = 0 ; i < data_num ; i ++)
	{
		data[i] = Laser_Loc[begin_index + i] ;
	}
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
void clear_array(uint8_t arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        arr[i] = 0;
    }
}

