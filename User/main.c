#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "SHOW.h"
#include "main.h"
#include "adc.h"
#include "timer.h"
#include "key.h"
#include "sw.h"
#include "timer3.h"

/************************************************************
电压电流检测:电压PA0，电流PA1
PWM波驱动引脚:PA8
芯片使能引脚:PA9
OLED屏幕:SCL:PB1,SDA:PB0
继电器控制位:PA10
按键:ADD(+):PB12 REDUCE(-):PB13 SET:PB14 MODE:PB15
************************************************************/

u8 protect_status = 1; // 过压或过流保护状态标志，1为断开输入，0为连接输入，若触发保护，则需复位程序
u8 sw_status = 0;	   // 继电器开关状态
u8 pid_mode = 0;	   //=0不工作，=1工作
u16 buck_pwm = 2160;   // pid不工作时电路的PWM

float DC_V = 0.0f;		// 电压数据
float Target_V = 9.00f; // 追踪的电压数据
float DC_I = 0.0f;		// 电流数据

float V_yuzhi = 20.00f; // 保护的电压阈值
float I_yuzhi = 2.50f;	// 保护的电流阈值

int main(void)
{
	u8 key = 0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断向量分组：
													// 第2组 抢先优先级：0 1 2 3
													// 子优先级：0 1 2 3

	Delay_Init(); // 初始化delay

	LED_Init(); // 初始化LED

	Init_adc(); // 初始化ADC

	BUCK_PWM_Init(); // PWM初始化

	// OLED初始化
	OLED_Init();
	canshu_view();

	KEY_Init(); // 按键初始化

	SW_Init(); // 控制引脚初始化

	TIM3_ENABLE_5ms(); // 开启5ms定时

loop:
	SW_OFF;		   // 关闭继电器开关
	SD_OFF;		   // 关闭芯片引脚使能
	sw_status = 0; // 标志继电器开关状态，0为关闭，1为开启
	pid_mode = 0;  // 失能PID
	OLED_Clear();  // OLED清零
	canshu_view(); // 启动界面
	while (1)
	{
		key = KEY_Scan(0); // 扫描按键
		if (key == SW_PRES)
		{
			SD_ON; // 打开芯片引脚使能
			SW_ON; // 打开继电器电路准备工作
			Delay_Ms(500);
			break;
		}
		OLED_ShowNum(72, 0, DC_V, 2, 16);
		OLED_ShowNum(96, 0, (u16)(DC_V * 100) % 100, 2, 16);
		OLED_ShowNum(72, 2, DC_I, 2, 16);
		OLED_ShowNum(96, 2, (u16)(DC_I * 100) % 100, 2, 16);
		OLED_ShowNum(72, 4, Target_V, 2, 16);
		OLED_ShowNum(96, 4, (u16)(Target_V * 100) % 100, 2, 16);
		Delay_Ms(50);
	}
	while (1)
	{
		key = KEY_Scan(0);
		OLED_ShowNum(72, 0, DC_V, 2, 16);
		OLED_ShowNum(96, 0, (u16)(DC_V * 100) % 100, 2, 16);
		OLED_ShowNum(72, 2, DC_I, 2, 16);
		OLED_ShowNum(96, 2, (u16)(DC_I * 100) % 100, 2, 16);
		OLED_ShowNum(72, 4, Target_V, 2, 16);
		OLED_ShowNum(96, 4, (u16)(Target_V * 100) % 100, 2, 16);
		if (sw_status == 0)
		{
			OLED_ShowCHinese(84, 6, 13);
			OLED_ShowCHinese(100, 6, 14);
		}
		else
		{
			OLED_ShowCHinese(84, 6, 11);
			OLED_ShowCHinese(100, 6, 12);
		}

		if (key == SW_PRES)
		{
			pid_mode = 1;
			sw_status = 1;
		}
		else if (key == ADD_PRES)
		{
			Target_V = Target_V + 0.100001f;
		}
		else if (key == REDUCE_PRES)
		{
			Target_V = Target_V - 0.099999f;
		}
		else if (key == CHOICE_PRES)
		{
			SW_OFF;
			SD_OFF;
			sw_status = 0;
			pid_mode = 0; // 失能PID
			goto loop;
		}

		if ((DC_V > V_yuzhi) || (DC_I > I_yuzhi))
		{
			SW_OFF;
			SD_OFF;
			protect_status = 1;
			sw_status = 0;
			pid_mode = 0; // 失能PID
			goto loop;
		}
	}
}

void TIM3_IRQHandler(void)
{
	static u16 data_count = 0;
	static float v_out = 0, i_out = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		data_count++;
		// 电压电流检测
		v_out += (float)ADC_ConvertedValue[0] / 4096 * 3.3; // 读取转换的电压值
		i_out += (float)ADC_ConvertedValue[1] / 4096 * 3.3; // 读取转换F的电压值
		if (data_count >= 300)
		{
			data_count = 0;
			DC_V = v_out / 300 * V_xishu; // 读取转换的电压值
			DC_I = i_out / 300 * I_xishu; // 读取转换的电压值
			v_out = 0;
			i_out = 0;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIM3溢出中断标志
	}
}
