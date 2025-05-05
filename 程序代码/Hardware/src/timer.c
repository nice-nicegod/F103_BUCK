#include "timer.h"
#include "adc.h"
#include "main.h"
#include "sys.h"
/*buck同步整流电路配置*/
#define BUCK_TIMx TIM1
#define BUCK_Plus 0
#define BUCK_ARR 3599 // 重装载值3600
#define BUCK_PSC 0	  // 分频系数1
/*boost电路同步整流PWM函数PA8*/
void BUCK_PWM_Init(void) // buck电流（PA8)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能PORTA,B时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// 初始化GPIO,PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 端口复用
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // PA8

	// 初始化时具单元
	TIM_DeInit(BUCK_TIMx);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = BUCK_ARR;
	TIM_TimeBaseInitStruct.TIM_Prescaler = BUCK_PSC;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BUCK_TIMx, &TIM_TimeBaseInitStruct);
	// 将输出通道2初始化为PWM模式1
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_Pulse = BUCK_Plus;
	TIM_OC1Init(BUCK_TIMx, &TIM_OCInitStruct);
	// 使能预装载寄存器
	TIM_OC1PreloadConfig(BUCK_TIMx, TIM_OCPreload_Enable);
	// 死区和刹车功能配置
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_1;
	TIM_BDTRInitStruct.TIM_DeadTime = 3; // 40.92ns
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(BUCK_TIMx, &TIM_BDTRInitStruct);
	// 使能定时器中断
	TIM_ITConfig(BUCK_TIMx, TIM_IT_Update, ENABLE); // 允许更新中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// 使能自动重装载
	TIM_ARRPreloadConfig(BUCK_TIMx, ENABLE);
	// 开启定时器
	TIM_Cmd(BUCK_TIMx, ENABLE);
	// 主输出使能
	TIM_CtrlPWMOutputs(BUCK_TIMx, ENABLE);
}
extern __IO uint16_t ADC_ConvertedValue[2];
extern float ADC_ConvertedValueLocal;
extern float ADC_ConvertedValueLoca2;

void TIM1_UP_IRQHandler(void)
{
	static u16 pid_count = 0;

	// 检查更新中断发生与否
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		// 增加计数器并每20次执行一次PID控制
		if (++pid_count >= 20)
		{
			pid_count = 0;

			// 根据系统状态执行相应逻辑
			if (protect_status == 1)
			{
				// 保护状态：关闭输出
				buck_pwm = 0;
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
			else if (pid_mode == 1)
			{
				// 考虑漂移电压的PID控制
				// 将目标电压也考虑漂移偏移
				float adjusted_target_v = Target_V + V_OFFSET; // 加上偏移以补偿
				float target_adc = adjusted_target_v / 3.3 * 4096 / V_xishu;

				// 获取当前ADC值 (考虑使用滤波后的值会更稳定)
				float current_adc = ADC_ConvertedValue[0];

				// 简化后的PID控制逻辑
				if (current_adc < target_adc)
				{
					// 输出电压低于目标：增加PWM
					buck_pwm += 1;
				}
				else if (current_adc > target_adc)
				{
					// 输出电压高于目标：减少PWM
					if (buck_pwm <= 1)
						buck_pwm = 0;
					else
						buck_pwm -= 1;
				}

				// 限制最大值并设置PWM
				if (buck_pwm >= 3599)
					buck_pwm = 3599;
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
			else
			{
				// 非PID模式：保持当前PWM
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
		}
		// 清除中断标志
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

/*选择通道函数*/
void set_pwm(TIM_TypeDef *TIMx, u8 chx, u16 prec, u16 up)
{
	// 判断输入参数是否正确
	if (chx < 1 || chx > 4)
		return;
	if (prec > up)
		prec = up;
	// 根据输入的通道设置PWM占空比
	switch (chx)
	{
	case 1:
		TIM_SetCompare1(TIMx, prec);
		break;
	case 2:
		TIM_SetCompare2(TIMx, prec);
		break;
	case 3:
		TIM_SetCompare3(TIMx, prec);
		break;
	case 4:
		TIM_SetCompare4(TIMx, prec);
		break;
	}
}
