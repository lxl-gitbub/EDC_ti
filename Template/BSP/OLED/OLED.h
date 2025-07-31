/**
 * @file      OLED.h
 * @brief     SSD1306 OLED��ʾ���������ͷ�ļ� (���I2C)
 * @details   ������ʹ�����ģ��I2CЭ����OLEDͨ�š����ṩ��OLED��ʼ����
 * ��ʾ�ַ������֡��ַ�����λͼ�ȹ��ܡ�
 * @author    Jianing Wang
 * @version   1.1
 * @date      2025-07-31
 * @note      - ������������һ���ṩ΢�뼶��ʱ���� `Delay_us()` ����ʱ�⡣
 * - ʹ��ǰ����ȷ������SysConfig��������SCL��SDA���ţ����ڴ��ļ���
 * ��ȷ������`OLED_GPIO_PORT`��`OLED_SCL_PIN`��`OLED_SDA_PIN`��
 */

#ifndef __OLED_H__
#define __OLED_H__

#include "ti_msp_dl_config.h" // ����TI MSPM0�ĵײ��
#include <stdint.h>          // �����׼��������

//==================================================================================
// �û������� (User Configuration)
//==================================================================================

// ����OLED���ӵ�GPIO�˿�
// ���磺�������SCL��SDA�����ӵ�GPIOA�������ʹ��GPIOA
#define OLED_GPIO_PORT      (GPIOA)

// ����OLED��SCL��SDA���� (����SysConfig�е����������ñ���һ��)
#define OLED_SCL_PIN        (GPIO_OLED_SCL_PIN)
#define OLED_SDA_PIN        (GPIO_OLED_SDA_PIN)


//==================================================================================
// �ڲ��궨�� (Internal Macros)
//==================================================================================

#define OLED_CMD            (0) // д����ģʽ
#define OLED_DATA           (1) // д����ģʽ

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C ʱ�� SCL ���Ų���
#define OLED_SCL_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SCL_PIN))
#define OLED_SCL_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SCL_PIN))

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C ���� SDA ���Ų���
#define OLED_SDA_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Read()     (DL_GPIO_readPins(OLED_GPIO_PORT, OLED_SDA_PIN))


//==================================================================================
// ����ԭ������ (Function Prototypes)
//==================================================================================

/**
 * @brief  OLED������ʼ��
 * @details ��ʼ��GPIO���Ų�����һϵ��ָ��������OLEDģ�顣
 * @param  None
 * @retval None
 */
void OLED_Init(void);

/**
 * @brief  �������OLED��Ļ
 * @details ���Դ������λ������Ϊ0����Ļ����ڡ�
 * @param  None
 * @retval None
 */
void OLED_Clear(void);

/**
 * @brief  ������ʾ��ɫ��ת
 * @param  i 0: ������ʾ, 1: ��ɫ��ת (�ڵװ��ֱ�׵׺���)
 * @retval None
 */
void OLED_ColorTurn(uint8_t i);

/**
 * @brief  ������Ļ������ת180��
 * @param  i 0: ������ʾ, 1: ��ת180��
 * @retval None
 */
void OLED_DisplayTurn(uint8_t i);

/**
 * @brief  ��OLED��ʾ
 * @param  None
 * @retval None
 */
void OLED_Display_On(void);

/**
 * @brief  �ر�OLED��ʾ (��������)
 * @param  None
 * @retval None
 */
void OLED_Display_Off(void);

/**
 * @brief  ��ָ��λ����ʾһ��ASCII�ַ�
 * @param  x      ��ʼ������ (0-127)
 * @param  y      ��ʼҳ���� (0-7 for 16-size font, 0-63 for direct addressing)
 * @param  chr    Ҫ��ʾ��ASCII�ַ�
 * @param  sizey  �����С (֧�� 8 �� 16)
 * @retval None
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t sizey);

/**
 * @brief  ��ָ��λ����ʾ�ַ���
 * @param  x      ��ʼ������ (0-127)
 * @param  y      ��ʼҳ���� (0-7)
 * @param  chr    ָ���ַ�����ָ��
 * @param  sizey  �����С (֧�� 8 �� 16)
 * @retval None
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t sizey);

/**
 * @brief  ��ʾһ���޷���ʮ��������
 * @param  x      ��ʼ������ (0-127)
 * @param  y      ��ʼҳ���� (0-7)
 * @param  num    Ҫ��ʾ������ (0-4294967295)
 * @param  len    Ҫ��ʾ�����ֳ���
 * @param  sizey  �����С
 * @retval None
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t sizey);

/**
 * @brief  ��ָ��λ����ʾһ��16x16�ĺ���
 * @param  x      ��ʼ������ (0-127)
 * @param  y      ��ʼҳ���� (0-7)
 * @param  no     Ҫ��ʾ�ĺ������ֿ� `Hzk` �е�������
 * @retval None
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no);

/**
 * @brief  ��ʾһ��λͼ
 * @param  x      ͼƬ���Ͻ���ʼ������ (0-127)
 * @param  y      ͼƬ���Ͻ���ʼҳ���� (0-7)
 * @param  sizex  ͼƬ�Ŀ�ȣ����أ�
 * @param  sizey  ͼƬ�ĸ߶ȣ����أ�
 * @param  BMP    ָ��λͼ���������ָ��
 * @retval None
 */
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[]);

#endif /* __OLED_H__ */