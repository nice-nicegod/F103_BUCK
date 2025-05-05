#include "sw.h"

void SW_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置成上拉输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	SW_OFF;
	SD_OFF;
}
