#include "freeRTOS_demo.h"
/* FreeRTOS相关文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "oled.h"
#include "semphr.h"     // 用互斥量必须加这个头文件

/**************************************************************************/
/* freeRTOS任务配置 */

#define START_PRIO      1
#define START_STK_SIZE  64
static TaskHandle_t StartTask_Handler=NULL;
static void Start_Task(void * pvParameters);

#define TASK1_PRIO      1
#define TASK1_STK_SIZE  64
static TaskHandle_t Task1_Handle=NULL;
static void Task1(void* parameter);

#define TASK2_PRIO      1
#define TASK2_STK_SIZE  64
static TaskHandle_t Task2_Handle=NULL;
static void Task2(void* parameter);

#define TASK3_PRIO      1
#define TASK3_STK_SIZE  64
static TaskHandle_t Task3_Handle=NULL;
static void Task3(void* parameter);

static SemaphoreHandle_t xOLEDMutex = NULL;

/**************************************************************************/

void freeRTOS_demo(void)
{
	
	xOLEDMutex = xSemaphoreCreateMutex();
	
    if (xOLEDMutex == NULL) {
        while(1); // 创建失败，卡住程序
    }
	
	xTaskCreate((TaskFunction_t) Start_Task,
                (char *) "Start_Task", 
                (uint32_t) START_STK_SIZE,
                (void *) NULL,
                (UBaseType_t) START_PRIO,
                (TaskHandle_t *) &StartTask_Handler );
	
    vTaskStartScheduler();  /* 启动任务调度器 */
}

static void Start_Task(void * pvParameters)
{
	
	while(1)
	{
		taskENTER_CRITICAL();

		xTaskCreate((TaskFunction_t) Task1,
                    (const char*) "Task1",
                    (uint32_t) TASK1_STK_SIZE,
                    (void*) NULL,
                    (UBaseType_t) TASK1_PRIO,
                    (TaskHandle_t*) &Task1_Handle);

		xTaskCreate((TaskFunction_t) Task2,
                    (const char*) "Task2",
                    (uint32_t) TASK2_STK_SIZE,
                    (void*) NULL,
                    (UBaseType_t) TASK2_PRIO,
                    (TaskHandle_t*) &Task2_Handle);

		xTaskCreate((TaskFunction_t) Task3,
                    (const char*) "Task3",
                    (uint32_t) TASK3_STK_SIZE,
                    (void*) NULL,
                    (UBaseType_t) TASK3_PRIO,
                    (TaskHandle_t*) &Task3_Handle);
		 		taskEXIT_CRITICAL();
		vTaskDelete(StartTask_Handler);

	}
}

static void Task1(void* parameter)
{
    while(1)
    {
		
		 if (xSemaphoreTake(xOLEDMutex, portMAX_DELAY) == pdTRUE)
			{
			OLED_Clear();
			OLED_ShowString(1, 1, "Task1"); // 第1行显示
			xSemaphoreGive(xOLEDMutex);     // 释放锁
			}
			
			vTaskDelay(6000);
			vTaskDelete(Task2_Handle);

    }
}

static void Task2(void* parameter)
{
    while(1)
    {
		
		 if (xSemaphoreTake(xOLEDMutex, portMAX_DELAY) == pdTRUE)
			{
				OLED_Clear();
				OLED_ShowString(2, 1, "Task2"); // 第2行显示
				xSemaphoreGive(xOLEDMutex);     // 释放锁
			}
			
				vTaskDelay(800);

    }
}

static void Task3(void* parameter)
{
    while(1)
    {
		
		 if (xSemaphoreTake(xOLEDMutex, portMAX_DELAY) == pdTRUE)
			{
			OLED_Clear();
			OLED_ShowString(4, 1, "Task3"); // 第3行显示
			xSemaphoreGive(xOLEDMutex);     // 释放锁
			}
			vTaskDelay(800);
	//		vTaskDelete(Task1_Handle);
			vTaskSuspend(Task1_Handle);
			vTaskDelay(5000);
			vTaskResume(Task1_Handle);
			vTaskDelay(5000);

    }
}
