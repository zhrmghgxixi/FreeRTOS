#ifndef __TIMER_H
#define __TIMER_H

    /****************   �ⲿͷ�ļ�����   ****************/
    #include "sys.h"
    #include "usart.h"



    /****************    �����ⲿ����   *****************/
    extern uint32_t FreeRTOSRunTimeTicks;        /* FreeRTOS��������ʱ��ͳ�Ʊ��� */



    /****************    �����ⲿ����   *****************/
    void TIM6_Init(uint16_t arr, uint16_t psc);      /* ��ʱ��6��ʼ�� */
    void TIM7_Init(uint16_t arr, uint16_t psc);      /* ��ʱ��7��ʼ�� */
    void ConfigureTimeForRunTimeStats(void);         /* ͳ��FreeRTOS��������ʱ���ʱ����ʱ����ʼ�� */

#endif
