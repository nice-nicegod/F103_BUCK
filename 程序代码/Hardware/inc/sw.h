#ifndef __SW_H
#define __SW_H
#include "main.h"
void SW_Init(void);
#define SW_ON GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define SW_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_10)
#define SD_ON GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define SD_OFF GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#endif
