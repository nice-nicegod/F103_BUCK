#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "delay.h"

// ������ʼ������
void KEY_Init(void) // IO��ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);								 // ʹ��PORTBʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // SW
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;										 // ���ó���������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief ����ɨ�躯��
 * @param mode ��������ģʽ��0����֧��������1��֧��������
 * @retval ����ֵ��0���ް�����SW_PRES/CHOICE_PRES/ADD_PRES/REDUCE_PRES����Ӧ�������£�
 */
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1; // �������ɿ���־
	u8 keyValue = 0;	  // ����ֵ

	if (mode)
	{
		key_up = 1; // ֧������
	}
	// ����Ƿ��а�������
	if (key_up && (SW == 0 || CHOICE == 0 || ADD == 0 || REDUCE == 0))
	{
		Delay_Ms(10); // ȥ����

		// �ٴμ��ȷ�ϰ���״̬����ֹ��������
		if (SW == 0 || CHOICE == 0 || ADD == 0 || REDUCE == 0)
		{
			key_up = 0;
			if (SW == 0)
				keyValue = SW_PRES;
			else if (CHOICE == 0)
				keyValue = CHOICE_PRES;
			else if (ADD == 0)
				keyValue = ADD_PRES;
			else if (REDUCE == 0)
				keyValue = REDUCE_PRES;
		}
	}
	else if (SW == 1 && CHOICE == 1 && ADD == 1 && REDUCE == 1)
	{
		key_up = 1;
	}
	return keyValue; // ���ذ���ֵ
}
