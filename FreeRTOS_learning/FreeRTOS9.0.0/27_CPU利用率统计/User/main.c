//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
//���ǵ�Ӳ������ļ�
#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "time.h"
#include "string.h"

/*****************************************************************************************
*																       ����֪ʶ
*		1����Ҫ�˽�ĺ�����
*		
*                 
******************************************************************************************/

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
static TaskHandle_t AppTaskCreate_Handle=NULL;
//��������AppTaskCreate����������
static void AppTaskCreate(void);

//LED0����������
static TaskHandle_t LED0_Task_Handle = NULL;
//LED0�����������
static void LED0_Task(void* parameter);

//LED1����������
static TaskHandle_t LED1_Task_Handle = NULL;
//LED1�����������
static void LED1_Task(void* parameter);

//CPU�����ʲ�ѯ������
static TaskHandle_t CPU_Task_Handle = NULL;
//CPU�����ʲ�ѯ��������
static void CPU_Task(void* parameter);

static void BSP_Init(void);

int main(void)
{
	BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ pdPASS���ɹ�
	
	//Ӳ����ʼ��
	BSP_Init();    
	printf("Ӳ����ʼ�����\r\n");
	
	//��̬������ʽ��������
	//��������Ĳ������ǰ�������AppTaskCreate�����������������������
	xReturn = xTaskCreate(
									(TaskFunction_t	)AppTaskCreate,		      //������
									(const char* 	  )"AppTaskCreate",		    //��������
									(uint16_t 		  )128,	                  //�����ջ��С
									(void* 		  	  )NULL,				          //���ݸ��������Ĳ���
									(UBaseType_t 	  )1, 	                  //�������ȼ�
									(TaskHandle_t*  )&AppTaskCreate_Handle);//������  
															
															 
	
	//�����ɹ��Ϳ��Կ��������������
	if(xReturn == pdPASS)  vTaskStartScheduler();   /* �������񣬿������� */
	else                   printf("���񴴽�ʧ��\r\n");
		
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
	BASIC_TIM_Init();
}



/*****************************************************
*��������  ��ר�Ŵ������������
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
	BaseType_t xReturn = pdPASS;//����һ��������Ϣ����ֵ pdPASS���ɹ�

	//�����ٽ�������������������Ǳ��뱣֤���ٽ���
	taskENTER_CRITICAL();  
	//����LED0_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED0_Task,		      //������
								(const char* 	  )"LED0_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				        //�������ȼ�
								(TaskHandle_t*  )&LED0_Task_Handle); //������ 
																			
	if(xReturn == pdPASS) printf("LED0_Task���񴴽��ɹ�!\r\n");
	
	//����LED1_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED1_Task,		      //������
								(const char* 	  )"LED1_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				        //�������ȼ�
								(TaskHandle_t*  )&LED1_Task_Handle); //������ 
																			
	if(xReturn == pdPASS) printf("LED1_Task���񴴽��ɹ�!\r\n");
	
	//����CPU_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)CPU_Task,		      //������
								(const char* 	  )"CPU_Task",		    //��������
								(uint32_t 		  )512,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )3, 				        //�������ȼ�
								(TaskHandle_t*  )&CPU_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("CPU_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ��LED0��صĲ���
*������    ��LED0_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED0_Task(void* parameter)
{
	while(1)
	{
		LED0_ON;
		printf("LED0_Task Running,LED0_ON\r\n");
		vTaskDelay(500);
		LED0_OFF;
		printf("LED0_Task Running,LED0_OFF\r\n");
		vTaskDelay(500);
	}
}

/*********************************************************************
*��������  ��LED1��صĲ���
*������    ��LED1_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED1_Task(void* parameter)
{
	while(1)
	{
		LED1_ON;
		printf("LED1_Task Running,LED0_ON\r\n");
		vTaskDelay(500);
		LED1_OFF;
		printf("LED1_Task Running,LED0_OFF\r\n");
		vTaskDelay(500);
	}
}

/*********************************************************************
*��������  ��CPU��������صĲ���
*������    ��CPU_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void CPU_Task(void* parameter)
{	
	uint8_t CPU_RunInfo[400];		//������������ʱ����Ϣ
	
  while (1)
  {
		memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskList((char *)&CPU_RunInfo);  //��ȡ��������ʱ����Ϣ
    
    printf("---------------------------------------------\r\n");
    printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("������       ���м���         ������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(1000);   //��ʱ1000��tick	
	}
	
}

