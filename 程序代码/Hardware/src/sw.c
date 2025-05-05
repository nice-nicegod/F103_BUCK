#include "sw.h"

void SW_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // ���ó��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	SW_OFF;
	SD_OFF;
}
