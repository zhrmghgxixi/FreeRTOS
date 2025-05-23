#include "freeRTOS_demo.h"
#include "led.h"
#include "malloc.h"
/* FreeRTOS����ļ� */
#include "FreeRTOS.h"
#include "task.h"



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



char task_buff[500];   /* �嵥��Ϣ������ */
/**********************************************************
* @funcName ��Task1
* @brief    ������1
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ��ʹ�ò�ѯFreeRTOS����״̬����
************************************************************/
static void Task1(void* parameter)
{
    UBaseType_t priority_num = 0;          /* �������ȼ�ֵ */
    UBaseType_t task_num = 0;              /* �������� */
    UBaseType_t task_num2 = 0;             /* �������� */
    TaskStatus_t * status_array = 0;       /* ��������״̬��Ϣ */
    TaskStatus_t * status_array2 = 0;      /* ָ������������״̬��Ϣ */
    TaskHandle_t task_handle = 0;          /* ĳ������������� */
    UBaseType_t task_stack_min = 0;        /* ʣ���ջ��С */
    int state = 0;                  /* ָ�������״̬��Ϣ */

    uint8_t i = 0;

    /* �޸�ָ����������ȼ� */
    vTaskPrioritySet(Task2_Handle, 4);

    /* ��ȡָ���������ȼ�ֵ��NULL��ʾ��ȡ����� */
    priority_num = uxTaskPriorityGet( NULL );
    printf("task1�������ȼ�Ϊ%ld\r\n", priority_num);

    /* ��ȡ��ǰϵͳ���������� */
    task_num = uxTaskGetNumberOfTasks();
    printf("����������%ld\r\n",task_num);

    /* ��ȡ���������״̬��Ϣ */
    status_array = mymalloc(0,(sizeof(TaskStatus_t) * task_num));   /* ��SRAM���ڴ����������ռ� */
    task_num2 = uxTaskGetSystemState( status_array,task_num,NULL);
    printf("������\t\t�������ȼ�\t������\r\n");
    for(i = 0; i < task_num2; i++)
    {
        printf("%s\t\t%ld\t%ld\r\n",
                status_array[i].pcTaskName,
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }

    /* ��ȡָ�������������Ϣv*/
    status_array2 = mymalloc(SRAMIN,sizeof(TaskStatus_t));
    vTaskGetInfo(Task2_Handle, status_array2, pdTRUE,eInvalid);
    printf("��������%s\r\n",status_array2->pcTaskName);
    printf("�������ȼ���%ld\r\n",status_array2->uxCurrentPriority);
    printf("�����ţ�%ld\r\n",status_array2->xTaskNumber);
    printf("����״̬��%d\r\n",status_array2->eCurrentState);

    /* ͨ����������ȡ������ */
    task_handle = xTaskGetHandle( "Task1" );
    printf("��������%#x\r\n",(int)task_handle);
    printf("task1����������%#x\r\n",(int)Task1_Handle);

    /* ��ȡָ�������״̬ */
    eTaskGetState(Task2_Handle);
    state = eTaskGetState(Task2_Handle);
    printf("��ǰtask2������״̬Ϊ��%d\r\n",state);

    /* ���嵥��ʽ��ȡ��������״̬��Ϣ */
    vTaskList( task_buff );
    printf("%s\r\n",task_buff);

    while(1)
    {
        /* ��ȡʣ���ջ��С */
        task_stack_min = uxTaskGetStackHighWaterMark(Task2_Handle);
        printf("task2��ʷʣ����С��ջΪ%ld\r\n",task_stack_min);
        vTaskDelay(800);   /* ��ʱ800��tick */
    }
}



/**********************************************************
* @funcName ��Task2
* @brief    ������2
* @param    ��void* parameter(�������,δ�õ�)
* @retval   ��void
* @details  ��
* @fn       ����ֻ��LED��ת��֤��ϵͳ��������
************************************************************/
static void Task2(void* parameter)
{
    while(1)
    {
        GPIOB->ODR ^= (1<<5);     /* LED0��ת */
        GPIOE->ODR ^= (1<<5);     /* LED1��ת */
        vTaskDelay(800);          /* ��ʱ800��tick */
    }
}




