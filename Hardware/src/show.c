#include "show.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "main.h"

void canshu_view() // OLED显示的参数界面，用来显示测量的输出电压参数
{
	OLED_Clear();
	// 输出电压
	OLED_ShowCHinese(0, 0, 0);
	OLED_ShowCHinese(16, 0, 1);
	OLED_ShowCHinese(32, 0, 2);
	OLED_ShowCHinese(48, 0, 3);
	OLED_ShowString(64, 0, ":", 16);
	OLED_ShowString(88, 0, ".", 16);
	OLED_ShowString(112, 0, "V", 16);
	// 输出电流
	OLED_ShowCHinese(0, 2, 0);
	OLED_ShowCHinese(16, 2, 1);
	OLED_ShowCHinese(32, 2, 2);
	OLED_ShowCHinese(48, 2, 4);
	OLED_ShowString(64, 2, ":", 16);
	OLED_ShowString(88, 2, ".", 16);
	OLED_ShowString(112, 2, "A", 16);
	// 目标电压
	OLED_ShowCHinese(0, 4, 5);
	OLED_ShowCHinese(16, 4, 6);
	OLED_ShowCHinese(32, 4, 2);
	OLED_ShowCHinese(48, 4, 3);
	OLED_ShowString(64, 4, ":", 16);
	OLED_ShowString(88, 4, ".", 16);
	OLED_ShowString(112, 4, "V", 16);
	// 工作状态
	OLED_ShowCHinese(0, 6, 7);
	OLED_ShowCHinese(16, 6, 8);
	OLED_ShowCHinese(32, 6, 9);
	OLED_ShowCHinese(48, 6, 10);
	OLED_ShowString(64, 6, ":", 16);
}
