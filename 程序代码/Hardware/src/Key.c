#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "delay.h"

// 按键初始化函数
void KEY_Init(void) // IO初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);								 // 使能PORTB时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // SW
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;										 // 设置成上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief 按键扫描函数
 * @param mode 按键工作模式（0：不支持连按，1：支持连按）
 * @retval 按键值（0：无按键，SW_PRES/CHOICE_PRES/ADD_PRES/REDUCE_PRES：对应按键按下）
 */
u8 KEY_Scan(u8 mode)
{
	static u8 key_up = 1; // 按键按松开标志
	u8 keyValue = 0;	  // 返回值

	if (mode)
	{
		key_up = 1; // 支持连按
	}
	// 检测是否有按键按下
	if (key_up && (SW == 0 || CHOICE == 0 || ADD == 0 || REDUCE == 0))
	{
		Delay_Ms(10); // 去抖动

		// 再次检测确认按键状态，防止抖动误判
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
	return keyValue; // 返回按键值
}
