/*-----------------------------------------------------*/
/*              超纬电子STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数头文件              */
/*                                                     */
/*-----------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H
#include "sys.h"
extern u8 pid_mode;          //=0不工作，=1工作
extern u16 buck_pwm;         // buck电路的PWM
extern float DC_V;           // 电压数据
extern float DC_I;           // 电流数据
extern float Target_V;       // 追踪的电压数据
extern const float V_OFFSET; // 电压偏移量，单位V

extern __IO uint16_t ADC_ConvertedValue[2];
extern u8 protect_status; // 保护状态
extern u8 sw_status;      // 开关状态
#define V_xishu 10.1215   // 测得10.112-10.115之间
#define I_xishu 2.04

#endif
