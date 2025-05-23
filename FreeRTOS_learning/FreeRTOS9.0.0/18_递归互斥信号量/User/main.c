//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//���ǵ�Ӳ������ļ�
#include "stm32f10x.h"
#include "key.h"
#include "usart.h"
#include "led.h"

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


//LED������
static TaskHandle_t LED_Task_Handle=NULL;	
//LED�������������
static void LED_Task(void* parameter);

//KEY������
static TaskHandle_t KEY_Task_Handle=NULL;	
//KEY�����������
static void Key_Task(void* parameter);

//�ݹ黥���ź������
SemaphoreHandle_t MuxSemRecursive_Handle=NULL;
SemaphoreHandle_t xMutex;

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
	KEY_Init();
	LED_Init();
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

	//���������ź���
  MuxSemRecursive_Handle = xSemaphoreCreateRecursiveMutex();
  if(NULL != MuxSemRecursive_Handle) printf("����MuxSem_Handle�����ź����ɹ�!\r\n");
	xSemaphoreGive( MuxSemRecursive_Handle );//�ͷ�һ�λ����ź���
	
	//����LED_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED_Task,		     //������
								(const char* 	  )"LED_Task",		   //��������
								(uint32_t 		  )128,					     //�����ջ��С
								(void* 		  	  )NULL,				     //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				       //�������ȼ�
								(TaskHandle_t*  )&LED_Task_Handle);//������ 
								
	if(xReturn == pdPASS) printf("LED_Task���񴴽��ɹ�!\r\n");
	
	//����Key_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Key_Task,		     //������
								(const char* 	  )"Key_Task",		   //��������
								(uint32_t 		  )128,					     //�����ջ��С
								(void* 		  	  )NULL,				     //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				       //�������ȼ�
								(TaskHandle_t*  )&KEY_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("Key_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ��LED����صĲ���
*������    ��LED_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED_Task(void* parameter)
{
	while (1)
	{
		xSemaphoreTakeRecursive(MuxSemRecursive_Handle, //�ݹ黥���ź������
														portMAX_DELAY);         //�ȴ�ʱ��(һֱ��)
		LED0_ON;
		LED1_ON;
		vTaskDelay(500);
		LED0_OFF;
		LED1_OFF;
		vTaskDelay(500);
		
		xSemaphoreGiveRecursive(MuxSemRecursive_Handle);
	}
}


/*********************************************************************
*��������  ��������صĲ���
*������    ��Key_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Key_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE; //����һ����ȡ���ⷵ��ֵ��Ĭ��ΪpdTRUE
	static int num =0;
	
	while (1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				printf("��ȡ�����ź���\r\n");
				xReturn = xSemaphoreTakeRecursive(MuxSemRecursive_Handle, //�ݹ黥���ź������
																					portMAX_DELAY);         //�ȴ�ʱ��(һֱ��)
				if(xReturn == pdTRUE) 
				{
					num++;
					printf("�ɹ���ȡ�����ź�����num��%d\r\n",num);
				}
				else
				{
					printf("��ȡ�����ź���ʧ��\r\n");
				}
				break;
			case 2: 
				printf("�ͷŻ����ź���\r\n");
				xReturn = xSemaphoreGiveRecursive(MuxSemRecursive_Handle);
				if(xReturn == pdTRUE) 
				{
					num--;
					printf("�ɹ��ͷŻ����ź�����num��%d\r\n",num);
				}
				else
				{
					printf("�ͷŻ����ź���ʧ��\r\n");
				}
				break;
			default: break;
		}
		
		vTaskDelay(100);
	}
}




