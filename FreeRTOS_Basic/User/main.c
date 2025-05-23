#include "sys.h"
#include "delay.h"
#include "led.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* 任务函数 */
void LED_Flash_Task(void *pvParameters);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 中断优先级分组
    SysTick_Init(72);       /* 滴答定时器初始化，带中断 */
    LED_Init();       // 初始化 LED 引脚

    // 创建任务（堆栈128字，优先级2）
    xTaskCreate(LED_Flash_Task, "LED_Flash", 128, NULL, 2, NULL);

    // 启动调度器
    vTaskStartScheduler();

    // 不会执行到这里
    while (1);
}

/**
 * @brief 循环全亮 → 延时 → 全灭 → 延时 → 重复
 */
void LED_Flash_Task(void *pvParameters)
{
    while (1)
    {
        // 所有LED点亮（低电平有效）
        LED0 = 0;
        LED1 = 0;
        LED2 = 0;
        vTaskDelay(pdMS_TO_TICKS(500)); // 延时500ms

        // 所有LED熄灭（高电平）
        LED0 = 1;
        LED1 = 1;
        LED2 = 1;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
