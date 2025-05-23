//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
//���ǵ�Ӳ������ļ�
#include "stm32f10x.h"
#include "led.h"
#include "usart.h"

/*****************************************************************************
*																����֪ʶ
*		1����ʶxTaskCreateStatic����
*						xTaskCreateStatic�Ĳ�����
*                        ����1��������
*                        ����2��������
*                        ����3�������ջ��С
*                        ����4�����ݸ��������Ĳ���
*                        ����5���������ȼ�
*                        ����6�������ջ
*                        ����7��������ƿ�
*          xTaskCreateStatic�ķ���ֵ��������
*
*		2����̬����������ص㣺
*            ��Ҫ�Լ����������ջ(����)���������Լ�����������ջ��������
*        ��ɾ��������Ȼ���ڣ�������û�취ȥ�ͷŵ�
*
*   3��ʹ�þ�̬��������ʱ��ע�⣺
*						1��Ҫ����FreeRTOSConfig.h��configSUPPORT_STATIC_ALLOCATION��Ҫ��1
*           2��������Ҫʵ������������vApplicationGetIdleTaskMemory()�� vApplicationGetTimerTaskMemory()��
*              �������������û��趨�Ŀ��У�Idle�������붨ʱ����Timer����
*              ��Ķ�ջ��С���������û��Լ����䣬�������Ƕ�̬���䡣
*
******************************************************************************/



/****************************************************************************** 
*                             ʲô��������
* ֪ʶ���䣺
* 			��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
* 	�Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
* 	����������ΪNULL��
*
*   ˵ֱ�׵㣬����������һ��ָ��������ƿ��ָ�룬������ƿ���������������Ϣ
*******************************************************************************/


/*****************************************
*ע�⣺
*    FreeRTOS�Ƽ�����С����ջΪ512���ֽ�
*    ��������128�ִ�С������ջ
*    1���ֵ���4���ֽ�
*    FreeRTOS�Ƽ�����С����ջΪ512���ֽ�
******************************************/

//��������AppTaskCreate��������
static TaskHandle_t AppTaskCreate_Handle;
//��������AppTaskCreate����������
static void AppTaskCreate(void);
//��������AppTaskCreate�������ջ
static StackType_t AppTaskCreate_Stack[128];
//��������AppTaskCreate��������ƿ�
static StaticTask_t AppTaskCreate_TCB;


//LED������
static TaskHandle_t LED_Task_Handle;	
//LED�������������
static void LED_Task(void* parameter);
//LED����ĵ������ջ
static StackType_t LED_Task_Stack[128];
//LED����ĵ�������ƿ�
static StaticTask_t LED_Task_TCB;


static void BSP_Init(void);

int main(void)
{
	//Ӳ����ʼ��
	BSP_Init();    
	printf("Ӳ����ʼ�����\r\n");
	
	//��̬������ʽ��������
	//��������Ĳ������ǰ�������AppTaskCreate�����������������������
	AppTaskCreate_Handle = xTaskCreateStatic(
															(TaskFunction_t	)AppTaskCreate,		   //������
															(const char* 	  )"AppTaskCreate",		 //��������
															(uint32_t 		  )128,	               //�����ջ��С
															(void* 		  	  )NULL,				       //���ݸ��������Ĳ���
															(UBaseType_t 	  )3, 	               //�������ȼ�
															(StackType_t*   )AppTaskCreate_Stack,//�����ջ
															(StaticTask_t*  )&AppTaskCreate_TCB);//������ƿ�   
	
	//�����ɹ��Ϳ��Կ��������������
	if(NULL != AppTaskCreate_Handle)  vTaskStartScheduler();   /* �������񣬿������� */
		
	while(1);
}


/*********************************************************************
*��������  ��Ӳ����ʼ��
*������    ��BSP_Init
*��������  ��void
*��������ֵ��static void
*����      ��
*            ����Ӳ����ʼ����д�ڸú����
*            ���磺���ڳ�ʼ������ʱ����ʼ����LED��ʼ����������ʼ��.....
**********************************************************************/
static void BSP_Init(void)
{
	/*
	* STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	* ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	* ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	*/
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	USART1_Init(115200);
	LED_Init();
}



/*****************************************************
*��������  �����񴴽�������
*������    ��AppTaskCreate
*��������  ��void
*��������ֵ��static void
*����      ��
*            ������񴴽�������ר�����������������ģ���
*        �ǻ���������񴴽����������������񴴽���ɺ���
*        �ǻ����ǻ�Ѹ��������١�
********************************************************/
static void AppTaskCreate(void)
{
	//�����ٽ�������������������Ǳ��뱣֤���ٽ���
	taskENTER_CRITICAL();        
	
	//����LED_Task����
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t	)LED_Task,		  //������
																			(const char* 	  )"LED_Task",		//��������
																			(uint32_t 		  )128,					  //�����ջ��С
																			(void* 		  	  )NULL,				  //���ݸ��������Ĳ���
																			(UBaseType_t 	  )4, 				    //�������ȼ�
																			(StackType_t*   )LED_Task_Stack,//�����ջ
																			(StaticTask_t*  )&LED_Task_TCB);//������ƿ� 
																			
	if(NULL != LED_Task_Handle) printf("LED_Task���񴴽��ɹ�!\r\n");
	else                        printf("LED_Task���񴴽�ʧ��!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ��LED��صĲ���
*������    ��LED_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED_Task(void* parameter)
{
	while (1)
	{
		LED0_ON;
		LED1_ON
		vTaskDelay(500);   /* ��ʱ500��tick */
		printf("LED��\r\n");
	
		LED0_OFF
		LED1_OFF;     
		vTaskDelay(500);   /* ��ʱ500��tick */		 		
		printf("LED��\r\n");
	}
}

/**************************************************************
*
*
*
*		ע�⣺
*
*				������ʹ���˾�̬���������ʱ����FreeRTOSConfig.h��
*   configSUPPORT_STATIC_ALLOCATION��Ҫ��1������������Ҫʵ��
*   ����������vApplicationGetIdleTaskMemory()�� vApplicationGetTimerTaskMemory()��
*   �������������û��趨�Ŀ��У�Idle�������붨ʱ����Timer����
*   ��Ķ�ջ��С���������û��Լ����䣬�������Ƕ�̬���䡣
*
*
*
*
****************************************************************/
// ����������ƿ�
static StaticTask_t Idle_Task_TCB;	
// �������������ջ
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

// ��ʱ��������ƿ�
static StaticTask_t Timer_Task_TCB;
// ��ʱ�������ջ
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/*********************************************************************
*��������  ����ȡ��������������ջ��������ƿ��ڴ�
*������    ��vApplicationGetIdleTaskMemory
*��������  ��ppxIdleTaskTCBBuffer   	:		������ƿ��ڴ�
*					   ppxIdleTaskStackBuffer	:	  �����ջ�ڴ�
*					   pulIdleTaskStackSize	  :		�����ջ��С
*��������ֵ��void
*����      ��
**********************************************************************/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer   = &Idle_Task_TCB;/* ������ƿ��ڴ� */
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;/* �����ջ�ڴ� */
	*pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;/* �����ջ��С */
}

/*********************************************************************
*��������  ����ȡ��ʱ������������ջ��������ƿ��ڴ�
*������    ��vApplicationGetTimerTaskMemory
*��������  ��ppxTimerTaskTCBBuffer   	:		������ƿ��ڴ�
*					   ppxTimerTaskStackBuffer	:	  �����ջ�ڴ�
*					   pulTimerTaskStackSize	  :		�����ջ��С
*��������ֵ��void
*����      ��
**********************************************************************/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
																		StackType_t **ppxTimerTaskStackBuffer, 
																		uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer   = &Timer_Task_TCB;/* ������ƿ��ڴ� */
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;/* �����ջ�ڴ� */
	*pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
}


