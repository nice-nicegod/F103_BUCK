#include "stm32f10x.h" // Device header

/**
 * @brief 初始化LED灯
 *
 * 该函数用于初始化LED灯，包括开启相关的GPIO端口时钟和配置GPIO引脚为输出模式
 */
void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 开启时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/// @brief 开启LED1，C13pin口
/// @param
void LED1_ON(void)
{
    
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/// @brief 关闭LED，C131pin口
/// @param
void LED1_OFF(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

/// @brief 翻转LED，C13pin口,实现单独按键开关
/// @param
void LED1_turn(void)
{
    if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
}
