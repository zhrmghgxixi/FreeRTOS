#include "sys.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "freeRTOS_demo.h"

/* ������ */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 4�ŷ��鷽�� 4 λ��ռ���ȼ��� 0 λ��Ӧ���ȼ� */
    SysTick_Init(72);       /* �δ�ʱ����ʼ�������ж� */
	OLED_Init();
	OLED_Clear();
    freeRTOS_demo();        /* freeRTOSʵ�� */
}
