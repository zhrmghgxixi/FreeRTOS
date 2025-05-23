#include "freeRTOS_demo.h"
#include "key.h"
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
/* FreeRTOS��ֵ�ź������� */

QueueHandle_t semphore_handle;    /* semphore_handle��ֵ�ź������ */



/* FreeRTOS�������� */

QueueHandle_t queue_handle;       /* queue_handle���о�� */



/* FreeRTOS���м����� */

QueueSetHandle_t queueset_handle; /* queueset_handle���м���� */



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

    /* ������ֵ�ź��� */
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL) printf("��ֵ�ź��������ɹ�������\r\n");
    else                        printf("��ֵ�ź�������ʧ�ܣ�����\r\n");

    /* ������Ϣ���� */
    queue_handle = xQueueCreate(1, sizeof(uint8_t));              /* ���г���Ϊ1����Ա��СΪsizeof(uint8_t) */
    if(queue_handle != NULL)  printf("��Ϣ���д����ɹ�����\r\n");
    else                      printf("��Ϣ���д���ʧ�ܣ���\r\n");

    /* �������м� */
    queueset_handle = xQueueCreateSet(2);                         /* �������м������Դ��2������ */
    if(queueset_handle != NULL) printf("���м������ɹ�����\r\n");
    else                        printf("���м�����ʧ�ܣ���\r\n");

    /* ����ֵ�ź����Ͷ�����ӵ����м� */
    xQueueAddToSet(queue_handle, queueset_handle);
    xQueueAddToSet(semphore_handle, queueset_handle);

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
* @fn       ����ȡ���м��е���Ϣ
************************************************************/
static void Task1(void* parameter)
{
    QueueSetMemberHandle_t member_handle;
    uint8_t key;

    while(1)
    {
        /* ��ѯ���м���Ϣ */
        member_handle = xQueueSelectFromSet(queueset_handle, portMAX_DELAY);

        /* ʶ����м����Ƕ�ֵ�ź���������Ϣ���������� */
        if(member_handle == queue_handle)
        {
            /* ��ȡ���м�����Ϣ���е����� */
            xQueueReceive(member_handle, &key, portMAX_DELAY);
            printf("��ȡ���Ķ�������Ϊ��%d\r\n",key);
        }
        else if(member_handle == semphore_handle)
        {
            /* ��ȡ���м��ж�ֵ�ź��������� */
            xSemaphoreTake(member_handle, portMAX_DELAY);
            printf("��ȡ�ź����ɹ�����\r\n");
        }
    }
}



/**********************************************************
* @funcName ��Task2
* @brief    ������2
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��ͨ����������Ϣ���з������ݻ��ͷŶ�ֵ�ź���
************************************************************/
static void Task2(void* parameter)
{
    uint8_t key = 0;
    BaseType_t err;

    while(1)
    {
        key = KEY_Scan();

        /* KEY0������ */
        if(key == 1)
        {
            if(semphore_handle != NULL)
            {
                /* ����м��е���Ϣ���з������� */
                err = xQueueSend(queue_handle, &key, portMAX_DELAY);
                if(err == pdPASS) printf("������queue_handleд�����ݳɹ�����\r\n");
                else              printf("������queue_handleд������ʧ�ܣ���\r\n");
            }
        }
        else if(key == 2)
        {
            err = xSemaphoreGive(semphore_handle);
            if(err == pdPASS) printf("�ͷ��ź����ɹ�����\r\n");
            else              printf("�ͷ��ź���ʧ�ܣ���\r\n");
        }

        vTaskDelay(10);   /* ��ʱ10��tick */
    }
}




