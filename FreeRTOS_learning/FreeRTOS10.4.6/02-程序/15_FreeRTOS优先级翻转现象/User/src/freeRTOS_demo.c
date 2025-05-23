#include "freeRTOS_demo.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/**
******************************************************************************
* @file      ��.\User\src\freeRTOS_demo.c
*              .\User\inc\freeRTOS_demo.h
* @author    ��XRbin
* @version   ��V1.0
* @date      ��2024-02-16
* @brief     ��freeRTOSʵ�����
******************************************************************************
* @attention
*   �ҵ�GitHub   ��https://github.com/XR-bin
*   �ҵ�gitee    ��https://gitee.com/xrbin
*   �ҵ�leetcode ��https://leetcode.cn/u/xrbin/
******************************************************************************
*/



/**************************************************************************/
/* FreeRTOS��ֵ�ź������� */

QueueHandle_t semphore_handle;   /* semphore_handle��ֵ�ź������ */



/* freeRTOS�������� */

/***
* ���񴴽���������
* ����: �������ȼ� ��ջ��С ������ ��������
*/
#define START_PRIO      1                     /* �������ȼ� */
#define START_STK_SIZE  64                    /* �����ջ��С����Ϊ��λ��1�ֵ���4�ֽ� */
static TaskHandle_t StartTask_Handler=NULL;   /* ��������������� */
static void StartTaskCreate(void* parameter); /* ����������������� */

/***
* TASK1��������
* ����: �������ȼ� ��ջ��С ������ ��������
*/
#define TASK1_PRIO      1              /* �������ȼ� */
#define TASK1_STK_SIZE  64             /* �����ջ��С����Ϊ��λ��1�ֵ���4�ֽ� */
static TaskHandle_t Task1_Handle=NULL; /* ����1�������� */
static void Task1(void* parameter);    /* ����1���������� */

/***
* TASK2��������
* ����: �������ȼ� ��ջ��С ������ ��������
*/
#define TASK2_PRIO      2              /* �������ȼ� */
#define TASK2_STK_SIZE  64             /* �����ջ��С����Ϊ��λ��1�ֵ���4�ֽ� */
static TaskHandle_t Task2_Handle=NULL; /* ����2�������� */
static void Task2(void* parameter);    /* ����2���������� */

/***
* TASK3��������
* ����: �������ȼ� ��ջ��С ������ ��������
*/
#define TASK3_PRIO      3              /* �������ȼ� */
#define TASK3_STK_SIZE  64             /* �����ջ��С����Ϊ��λ��1�ֵ���4�ֽ� */
static TaskHandle_t Task3_Handle=NULL; /* ����3�������� */
static void Task3(void* parameter);    /* ����3���������� */

/**************************************************************************/





/**********************************************************
* @funcName ��freeRTOS_demo
* @brief    ��freeRTOSʵ�麯��
* @param    ��void
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void freeRTOS_demo(void)
{
    BaseType_t xReturn = pdPASS; /* ����һ��������Ϣ����ֵ pdPASS���ɹ� */

    /* ������ֵ�ź��� */
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL) printf("��ֵ�ź��������ɹ�������\r\n");
    else                        printf("��ֵ�ź�������ʧ�ܣ�����\r\n");

    /* ��̬����������������� */
    xReturn = xTaskCreate(
                         (TaskFunction_t )StartTaskCreate,     /* ������ */
                         (const char*    )"StartTaskCreate",   /* �������� */
                         (uint32_t       )START_STK_SIZE,      /* �����ջ��С */
                         (void*          )NULL,                /* ���ݸ��������Ĳ��� */
                         (UBaseType_t    )START_PRIO,          /* �������ȼ� */
                         (TaskHandle_t*  )&StartTask_Handler); /* ������ */

    if(xReturn == pdPASS) printf("StartTaskCreate���񴴽��ɹ�!\r\n");
    else                  printf("StartTaskCreate���񴴽�ʧ��!\r\n");

    vTaskStartScheduler();  /* ������������� */
}



/**********************************************************
* @funcName ��StartTaskCreate
* @brief    �����ڴ������������
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��
*            ������񴴽�������ר�����������������ģ���
*        �ǻ���������񴴽����������������񴴽���ɺ���
*        �ǻ����ǻ�Ѹ��������١�
************************************************************/
static void StartTaskCreate(void* parameter)
{
    BaseType_t xReturn = pdPASS; /* ����һ��������Ϣ����ֵ pdPASS���ɹ� */

    taskENTER_CRITICAL();        /* �����ٽ�������������������Ǳ��뱣֤���ٽ��� */

    /* ��̬����Task1���� */
    xReturn = xTaskCreate(
                         (TaskFunction_t )Task1,             /* ������ */
                         (const char*    )"Task1",           /* �������� */
                         (uint32_t       )TASK1_STK_SIZE,    /* �����ջ��С */
                         (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                         (UBaseType_t    )TASK1_PRIO,        /* �������ȼ� */
                         (TaskHandle_t*  )&Task1_Handle);    /* ������ */

    if(xReturn == pdPASS) printf("Task1���񴴽��ɹ�!\r\n");
    else                  printf("Task1���񴴽�ʧ��!\r\n");

    /* ��̬����Task2���� */
    xReturn = xTaskCreate(
                         (TaskFunction_t )Task2,             /* ������ */
                         (const char*    )"Task2",           /* �������� */
                         (uint32_t       )TASK2_STK_SIZE,    /* �����ջ��С */
                         (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                         (UBaseType_t    )TASK2_PRIO,        /* �������ȼ� */
                         (TaskHandle_t*  )&Task2_Handle);    /* ������ */

    if(xReturn == pdPASS) printf("Task2���񴴽��ɹ�!\r\n");
    else                  printf("Task2���񴴽�ʧ��!\r\n");

    /* ��̬����Task3���� */
    xReturn = xTaskCreate(
                         (TaskFunction_t )Task3,             /* ������ */
                         (const char*    )"Task3",           /* �������� */
                         (uint32_t       )TASK3_STK_SIZE,    /* �����ջ��С */
                         (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                         (UBaseType_t    )TASK3_PRIO,        /* �������ȼ� */
                         (TaskHandle_t*  )&Task3_Handle);    /* ������ */

    if(xReturn == pdPASS) printf("Task3���񴴽��ɹ�!\r\n");
    else                  printf("Task3���񴴽�ʧ��!\r\n");

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}



/**********************************************************
* @funcName ��Task1
* @brief    ������1
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       �������ȼ�����
************************************************************/
static void Task1(void* parameter)
{
    while(1)
    {
        printf("low_task��ȡ�ź���\r\n");
        xSemaphoreTake(semphore_handle,portMAX_DELAY);
        printf("low_task�������У�����\r\n");
        delay_ms(3000);
        printf("low_task�ͷ��ź���\r\n");
        xSemaphoreGive(semphore_handle); 
        vTaskDelay(1000);
    }
}



/**********************************************************
* @funcName ��Task2
* @brief    ������2
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ���е����ȼ�����
************************************************************/
static void Task2(void* parameter)
{
    while(1)
    {
        printf("middle_task�������У�����\r\n");
        vTaskDelay(1000);
    }
}



/**********************************************************
* @funcName ��Task3
* @brief    ������3
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       �������ȼ�����
************************************************************/
static void Task3(void* parameter)
{
    while(1)
    {
        printf("high_task��ȡ�ź���\r\n");
        xSemaphoreTake(semphore_handle,portMAX_DELAY);
        printf("high_task�������У�����\r\n");
        delay_ms(1000);
        printf("high_task�ͷ��ź���\r\n");
        xSemaphoreGive(semphore_handle); 
        vTaskDelay(1000);
    }
}


