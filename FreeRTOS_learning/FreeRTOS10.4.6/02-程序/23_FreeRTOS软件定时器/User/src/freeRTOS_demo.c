#include "freeRTOS_demo.h"
#include "key.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"



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
/* FreeRTOS�����ʱ������ */

/* �����ʱ��1 */
void timer1_callback( TimerHandle_t pxTimer );
TimerHandle_t timer1_handle = 0;    /* ���ζ�ʱ������ */

/* �����ʱ��2 */
void timer2_callback( TimerHandle_t pxTimer );
TimerHandle_t timer2_handle = 0;    /* ���ڶ�ʱ������ */



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

    /* �����ʱ��1 --- ���ζ�ʱ�� */
    timer1_handle = xTimerCreate( "timer1", 
                                    500,
                                    pdFALSE,
                                    (void *)1,
                                    timer1_callback );

    /* �����ʱ��2 --- ���ڶ�ʱ�� */
    timer2_handle = xTimerCreate( "timer2", 
                                    2000,
                                    pdTRUE,
                                    (void *)2,
                                    timer2_callback );

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
* @fn       ��ͨ���������������ʱ���Ŀ�����ر�
************************************************************/
static void Task1(void* parameter)
{
    uint8_t key = 0;

    while(1)
    {
        key = KEY_Scan();

        switch(key)
        {
            /* KEY0���� */
            case 1:
                    /* ���������ʱ�� */
                    xTimerStart(timer1_handle,portMAX_DELAY);
                    xTimerStart(timer2_handle,portMAX_DELAY);
                    break;

            /* KEY1���� */
            case 2:
                    /* �ر������ʱ�� */
                    xTimerStop(timer1_handle,portMAX_DELAY);
                    xTimerStop(timer2_handle,portMAX_DELAY);
                    break;
        }

        vTaskDelay(10);   /* ��ʱ10��tick */
    }
}



/**********************************************************
* @funcName ��timer1_callback
* @brief    �������ʱ��1�ĳ�ʱ�ص�����
* @param    ��TimerHandle_t pxTimer(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void timer1_callback(TimerHandle_t pxTimer)
{
    static uint32_t timer = 0;
    printf("timer1�����д�����%d\r\n",++timer);
}



/**********************************************************
* @funcName ��timer2_callback
* @brief    �������ʱ��2�ĳ�ʱ�ص�����
* @param    ��TimerHandle_t pxTimer(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void timer2_callback(TimerHandle_t pxTimer)
{
    static uint32_t timer = 0;
    printf("timer2�����д�����%d\r\n",++timer);
}





