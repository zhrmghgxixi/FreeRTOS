#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "freeRTOS_demo.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"

/***
* ע�⣺��ʵ������FreeRTOSϵͳ���ܹ�������ȼ���Χ��5~15��
*/

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 4�ŷ��鷽�� 4 λ��ռ���ȼ��� 0 λ��Ӧ���ȼ� */
    SysTick_Init(72);       /* �δ�ʱ����ʼ�������ж� */
    USART1_Init(115200);    /* ����1��ʼ�� 115200 */
    LED_Init();             /* LED��ʼ�� */
    TIM6_Init(9999, 7200);  /* ��ʱ��6��ʼ����1s�ж�һ�� */
    TIM7_Init(9999, 7200);  /* ��ʱ��7��ʼ����1s�ж�һ�� */

    freeRTOS_demo();        /* freeRTOSʵ�� */
}


