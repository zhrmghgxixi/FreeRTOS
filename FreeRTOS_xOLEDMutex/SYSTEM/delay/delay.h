#ifndef __DELAY_H
#define __DELAY_H

#include "stdint.h"  // 使用标准整数类型

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 SysTick 定时器（支持与 FreeRTOS 共存）
 * @param sysclk  系统时钟频率（单位：MHz），例如 72 表示 72MHz
 */
void SysTick_Init(uint16_t sysclk);

/**
 * @brief 微秒延时函数
 * @param us  要延时的微秒数
 */
void delay_us(uint32_t us);

/**
 * @brief 毫秒延时函数
 * @param ms  要延时的毫秒数
 */
void delay_ms(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif // __DELAY_H
