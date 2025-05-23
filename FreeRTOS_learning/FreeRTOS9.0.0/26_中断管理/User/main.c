//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//���ǵ�Ӳ������ļ�
#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "exti.h"

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

//LED����������
static TaskHandle_t LED_Task_Handle = NULL;
//LED�����������
static void LED_Task(void* parameter);

//Exti�жϴ���������
static TaskHandle_t Exti_Task_Handle = NULL;
//Exti�жϴ����������
static void Exti_Task(void* parameter);

//��Ϣ���о��
QueueHandle_t Test_Queue =NULL;
#define  QUEUE_LEN    4   //���еĳ��ȣ����ɰ������ٸ���Ϣ
#define  QUEUE_SIZE   4   //������ÿ����Ϣ��С���ֽڣ�


static void BSP_Init(void);

uint8_t *Test_Ptr = NULL;     //����ָ����������Ŀռ��ָ��

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
	EXTI34_Init();
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

	//����Test_Queue
  Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,  //��Ϣ���еĳ���
                            (UBaseType_t ) QUEUE_SIZE);//��Ϣ�Ĵ�С
	if(NULL != Test_Queue) printf("Test_Queue��Ϣ���д����ɹ�!\r\n");
	
	//����LED_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED_Task,		      //������
								(const char* 	  )"LED_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				        //�������ȼ�
								(TaskHandle_t*  )&LED_Task_Handle); //������ 
																			
	if(xReturn == pdPASS) printf("LED_Task���񴴽��ɹ�!\r\n");
	
	//����Exti_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Exti_Task,		      //������
								(const char* 	  )"Exti_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				        //�������ȼ�
								(TaskHandle_t*  )&Exti_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("Exti_Task���񴴽��ɹ�!\r\n");
	
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
	while(1)
	{
		LED0_TOGGLE;
		vTaskDelay(500);
	}
}

/*********************************************************************
*��������  ��Exti�жϴ�����صĲ���
*������    ��Exti_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Exti_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;//����һ����Ϣ������Ϣ����ֵ
  uint32_t r_queue;	          //����һ��������Ϣ�ı��� 
	while (1)
	{
		//���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ�
    xReturn = xQueueReceive( Test_Queue,    //��Ϣ���еľ��
                             &r_queue,      //���͵���Ϣ����
                             portMAX_DELAY);//�ȴ�ʱ�� һֱ��
														 
		if(pdPASS == xReturn) printf("�����жϵ��� KEY%d !\r\n",r_queue);
		else                  printf("���ݽ��ճ���\r\n");

		
		LED1_TOGGLE;
	}
}




