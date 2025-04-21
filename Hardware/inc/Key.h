#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define SW GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)
#define SW_PRES 1
#define CHOICE GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)
#define CHOICE_PRES 2
#define ADD GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define ADD_PRES 3
#define REDUCE GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)
#define REDUCE_PRES 4
void KEY_Init(void); // IO初始化
u8 KEY_Scan(u8);     // 按键扫描函数
#endif
