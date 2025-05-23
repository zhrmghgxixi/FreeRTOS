#ifndef __DELAY_H
#define __DELAY_H

#include "stdint.h"  // ʹ�ñ�׼��������

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ��ʼ�� SysTick ��ʱ����֧���� FreeRTOS ���棩
 * @param sysclk  ϵͳʱ��Ƶ�ʣ���λ��MHz�������� 72 ��ʾ 72MHz
 */
void SysTick_Init(uint16_t sysclk);

/**
 * @brief ΢����ʱ����
 * @param us  Ҫ��ʱ��΢����
 */
void delay_us(uint32_t us);

/**
 * @brief ������ʱ����
 * @param ms  Ҫ��ʱ�ĺ�����
 */
void delay_ms(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif // __DELAY_H
