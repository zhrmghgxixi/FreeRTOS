#ifndef __DELAY_H
#define __DELAY_H

    /****************   �ⲿͷ�ļ�����   ****************/
    #include "sys.h"



    /****************    �����ⲿ����   *****************/
    void SysTick_Init(uint16_t sysclk);/* �δ�ʱ����ʼ�� */
    void delay_us(uint32_t us);        /* ΢����ʱ */
    void delay_ms(uint16_t ms);        /* ������ʱ */

#endif





























