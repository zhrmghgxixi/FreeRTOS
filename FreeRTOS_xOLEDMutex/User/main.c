#include "sys.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "freeRTOS_demo.h"

/* 任务函数 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 4号分组方案 4 位抢占优先级， 0 位响应优先级 */
    SysTick_Init(72);       /* 滴答定时器初始化，带中断 */
	OLED_Init();
	OLED_Clear();
    freeRTOS_demo();        /* freeRTOS实验 */
}
