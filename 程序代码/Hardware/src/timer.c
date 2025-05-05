#include "timer.h"
#include "adc.h"
#include "main.h"
#include "sys.h"
/*buckͬ��������·����*/
#define BUCK_TIMx TIM1
#define BUCK_Plus 0
#define BUCK_ARR 3599 // ��װ��ֵ3600
#define BUCK_PSC 0	  // ��Ƶϵ��1
/*boost��·ͬ������PWM����PA8*/
void BUCK_PWM_Init(void) // buck������PA8)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // ʹ��PORTA,Bʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// ��ʼ��GPIO,PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �˿ڸ���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // PA8

	// ��ʼ��ʱ�ߵ�Ԫ
	TIM_DeInit(BUCK_TIMx);
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = BUCK_ARR;
	TIM_TimeBaseInitStruct.TIM_Prescaler = BUCK_PSC;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(BUCK_TIMx, &TIM_TimeBaseInitStruct);
	// �����ͨ��2��ʼ��ΪPWMģʽ1
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_Pulse = BUCK_Plus;
	TIM_OC1Init(BUCK_TIMx, &TIM_OCInitStruct);
	// ʹ��Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(BUCK_TIMx, TIM_OCPreload_Enable);
	// ������ɲ����������
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_1;
	TIM_BDTRInitStruct.TIM_DeadTime = 3; // 40.92ns
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(BUCK_TIMx, &TIM_BDTRInitStruct);
	// ʹ�ܶ�ʱ���ж�
	TIM_ITConfig(BUCK_TIMx, TIM_IT_Update, ENABLE); // ��������ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// ʹ���Զ���װ��
	TIM_ARRPreloadConfig(BUCK_TIMx, ENABLE);
	// ������ʱ��
	TIM_Cmd(BUCK_TIMx, ENABLE);
	// �����ʹ��
	TIM_CtrlPWMOutputs(BUCK_TIMx, ENABLE);
}
extern __IO uint16_t ADC_ConvertedValue[2];
extern float ADC_ConvertedValueLocal;
extern float ADC_ConvertedValueLoca2;

void TIM1_UP_IRQHandler(void)
{
	static u16 pid_count = 0;

	// �������жϷ������
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		// ���Ӽ�������ÿ20��ִ��һ��PID����
		if (++pid_count >= 20)
		{
			pid_count = 0;

			// ����ϵͳ״ִ̬����Ӧ�߼�
			if (protect_status == 1)
			{
				// ����״̬���ر����
				buck_pwm = 0;
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
			else if (pid_mode == 1)
			{
				// ����Ư�Ƶ�ѹ��PID����
				// ��Ŀ���ѹҲ����Ư��ƫ��
				float adjusted_target_v = Target_V + V_OFFSET; // ����ƫ���Բ���
				float target_adc = adjusted_target_v / 3.3 * 4096 / V_xishu;

				// ��ȡ��ǰADCֵ (����ʹ���˲����ֵ����ȶ�)
				float current_adc = ADC_ConvertedValue[0];

				// �򻯺��PID�����߼�
				if (current_adc < target_adc)
				{
					// �����ѹ����Ŀ�꣺����PWM
					buck_pwm += 1;
				}
				else if (current_adc > target_adc)
				{
					// �����ѹ����Ŀ�꣺����PWM
					if (buck_pwm <= 1)
						buck_pwm = 0;
					else
						buck_pwm -= 1;
				}

				// �������ֵ������PWM
				if (buck_pwm >= 3599)
					buck_pwm = 3599;
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
			else
			{
				// ��PIDģʽ�����ֵ�ǰPWM
				set_pwm(TIM1, 1, buck_pwm, 3599);
			}
		}
		// ����жϱ�־
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

/*ѡ��ͨ������*/
void set_pwm(TIM_TypeDef *TIMx, u8 chx, u16 prec, u16 up)
{
	// �ж���������Ƿ���ȷ
	if (chx < 1 || chx > 4)
		return;
	if (prec > up)
		prec = up;
	// ���������ͨ������PWMռ�ձ�
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
