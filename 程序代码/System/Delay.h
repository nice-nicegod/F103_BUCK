/*-------------------------------------------------*/
/*                                                 */
/*          使用SysTick实现延时功能的头文件        */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __DELAY_H
#define __DELAY_H 

void Delay_Init(void);         //延时初始化函数
void Delay_Us(unsigned int);   //延时毫秒函数
void Delay_Ms(unsigned int);   //延时微秒函数
void delay_us(unsigned int);   //延时毫秒函数
void delay_ms(unsigned int);   //延时微秒函数
#endif





























