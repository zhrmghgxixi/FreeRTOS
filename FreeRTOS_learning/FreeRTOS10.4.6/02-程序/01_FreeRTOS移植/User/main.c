#include "sys.h"
#include "delay.h"
#include "led.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 4�ŷ��鷽�� 4 λ��ռ���ȼ��� 0 λ��Ӧ���ȼ� */
    SysTick_Init(72);       /* �δ�ʱ����ʼ�������ж� */
    LED_Init();             /* LED��ʼ�� */

    while(1)
    {
        LED0_ON;
        LED1_ON;
        delay_ms(300);
        LED0_OFF;
        LED1_OFF;
        delay_ms(300);
    }
}


