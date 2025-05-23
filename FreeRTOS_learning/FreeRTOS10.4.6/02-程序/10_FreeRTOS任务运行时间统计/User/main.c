#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "freeRTOS_demo.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 4�ŷ��鷽�� 4 λ��ռ���ȼ��� 0 λ��Ӧ���ȼ� */
    SysTick_Init(72);       /* �δ�ʱ����ʼ�������ж� */
    USART1_Init(115200);    /* ����1��ʼ�� 115200 */
    LED_Init();             /* LED��ʼ�� */
    KEY_Init();             /* KEY��ʼ�� */

    freeRTOS_demo();        /* freeRTOSʵ�� */
}


