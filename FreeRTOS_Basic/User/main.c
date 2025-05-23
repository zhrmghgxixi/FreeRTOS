#include "sys.h"
#include "delay.h"
#include "led.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* ������ */
void LED_Flash_Task(void *pvParameters);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // �ж����ȼ�����
    SysTick_Init(72);       /* �δ�ʱ����ʼ�������ж� */
    LED_Init();       // ��ʼ�� LED ����

    // �������񣨶�ջ128�֣����ȼ�2��
    xTaskCreate(LED_Flash_Task, "LED_Flash", 128, NULL, 2, NULL);

    // ����������
    vTaskStartScheduler();

    // ����ִ�е�����
    while (1);
}

/**
 * @brief ѭ��ȫ�� �� ��ʱ �� ȫ�� �� ��ʱ �� �ظ�
 */
void LED_Flash_Task(void *pvParameters)
{
    while (1)
    {
        // ����LED�������͵�ƽ��Ч��
        LED0 = 0;
        LED1 = 0;
        LED2 = 0;
        vTaskDelay(pdMS_TO_TICKS(500)); // ��ʱ500ms

        // ����LEDϨ�𣨸ߵ�ƽ��
        LED0 = 1;
        LED1 = 1;
        LED2 = 1;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
