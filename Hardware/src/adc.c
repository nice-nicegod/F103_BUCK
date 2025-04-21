#include "adc.h"
#define ADC1_DR_Address ((u32)0x40012400 + 0x4c)
__IO uint16_t ADC_ConvertedValue[2]; // 全局变量//AD采样存放空间
void Init_adc(void)					 // 初始化ADC
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//---------------------AD初始化--------------------
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  // 启动DMA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 启动ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 启动GPIOA时钟

	// 采样脚设置		PA0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 采样脚设置		PA1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_DeInit(DMA1_Channel1);													// DMA1通道1配置
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;					// 外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;		// 传输到的内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							// dma传输方向单向
	DMA_InitStructure.DMA_BufferSize = 2;										// 设置DMA在传输时缓冲区的长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// 设置DMA的外设递增模式，一个外设
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// 设置DMA的内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据字长
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			// 内存数据字长
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								// 设置DMA的传输模式：连续不断的循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							// 设置DMA的优先级别
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// 设置DMA的2个memory中的变量互相访问
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);										// 使能通道1
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					// 独立工作模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						// 扫描方式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					// 连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发禁止
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				// 数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 2;								// 用于转换的通道数
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5); // 规则模式通道配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);	// 使能ADC1的DMA
	ADC_Cmd(ADC1, ENABLE);		// 使能ADC1
	ADC_ResetCalibration(ADC1); // 使能ADC1复位校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1))
		;						// 检查校准寄存器是否复位完毕
	ADC_StartCalibration(ADC1); // 开始校准
	while (ADC_GetCalibrationStatus(ADC1))
		;									// 检测是否校准完毕
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 开启ADC1的软件转换
}
