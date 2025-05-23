#include "freeRTOS_demo.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"



/**
******************************************************************************
* @file      ��.\User\src\freeRTOS_demo.c
*              .\User\inc\freeRTOS_demo.h
* @author    ��XRbin
* @version   ��V1.0
* @date      ��2024-01-20
* @brief     ��freeRTOSʵ�����
******************************************************************************
* @attention
*   �ҵ�GitHub   ��https://github.com/XR-bin
*   �ҵ�gitee    ��https://gitee.com/xrbin
*   �ҵ�leetcode ��https://leetcode.cn/u/xrbin/
******************************************************************************
*/



/**************************************************************************/
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

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}



/**********************************************************
* @funcName ��Task1
* @brief    ������1
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��FreeRTOS�жϹ�����ƶ�ʱ��7�ж�
************************************************************/
static void Task1(void* parameter)
{
    uint8_t task1_num = 0;

    while(1)
    {
        if(++task1_num == 5)
        {
            task1_num = 0;
            printf("���ж�!!\r\n");
            portDISABLE_INTERRUPTS();
            delay_ms(5000);
            printf("���жϣ�����\r\n");
            portENABLE_INTERRUPTS();
        }
    }
}


