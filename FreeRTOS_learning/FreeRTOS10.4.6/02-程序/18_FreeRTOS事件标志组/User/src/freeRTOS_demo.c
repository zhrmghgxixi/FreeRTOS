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
* @date      ��2024-02-18
* @brief     ��freeRTOSʵ�����
******************************************************************************
* @attention
*   �ҵ�GitHub   ��https://github.com/XR-bin
*   �ҵ�gitee    ��https://gitee.com/xrbin
*   �ҵ�leetcode ��https://leetcode.cn/u/xrbin/
******************************************************************************
*/



/**************************************************************************/
/* FreeRTOS�¼���־������ */

EventGroupHandle_t  eventgroup_handle;        /* eventgroup_handle�¼���־���� */
#define EVENTBIT_0  (1 << 0)                  /* �¼�1��־ */
#define EVENTBIT_1  (1 << 1)                  /* �¼�2��־ */



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

    /* �����¼���־�� */
    eventgroup_handle = xEventGroupCreate();
    if(eventgroup_handle != NULL) printf("�¼���־�鴴���ɹ�����\r\n");
    else                          printf("�¼���־�鴴��ʧ�ܣ���\r\n");

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

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}



/**********************************************************
* @funcName ��Task1
* @brief    ������1
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ����ȡ�¼���־����Ϣ
************************************************************/
static void Task1(void* parameter)
{
    EventBits_t event_bit = 0;

    while(1)
    {
        event_bit = xEventGroupWaitBits( eventgroup_handle,         /* �¼���־���� */
                                         EVENTBIT_0 | EVENTBIT_1,   /* �ȴ��¼���־���bit0��bit1λ */
                                         pdTRUE,                    /* �ɹ��ȴ����¼���־λ������¼���־���е�bit0��bit1λ */
                                         pdTRUE,                    /* �ȴ��¼���־���bit0��bit1λ����1,�ͳ��� */
                                         portMAX_DELAY );           /* ���� */
        printf("�ȴ������¼���־λֵΪ��%#x\r\n",event_bit);
    }
}



/**********************************************************
* @funcName ��Task2
* @brief    ������2
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��ͨ�����������¼���־���λ
************************************************************/
static void Task2(void* parameter)
{
    uint8_t key = 0;

    while(1)
    {
        key = KEY_Scan();

        switch(key)
        {
            /* KEY0���� */
            case 1: 
                    xEventGroupSetBits( eventgroup_handle, EVENTBIT_0); /* ���¼���־���bit0λ��1 */
                    break;

            /* KEY1���� */
            case 2:
                    xEventGroupSetBits( eventgroup_handle, EVENTBIT_1); /* ���¼���־���bit1λ��1 */
                    break;
        }

        vTaskDelay(10);   /* ��ʱ10��tick */
    }
}




