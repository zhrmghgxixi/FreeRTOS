//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
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


//LED0������
static TaskHandle_t LED0_Task_Handle=NULL;	
//LED0�������������
static void LED0_Task(void* parameter);

//LED1������
static TaskHandle_t LED1_Task_Handle=NULL;	
//LED1�������������
static void LED1_Task(void* parameter);

//KEY������
static TaskHandle_t KEY_Task_Handle=NULL;	
//KEY�����������
static void Key_Task(void* parameter);

//�¼����
static EventGroupHandle_t Event_Handle =NULL;
//�¼�λ���������һ��������ĳ��λ����ĳ��λ��1����ʾ����ĳ���¼�
//����¼����õ���32λ�ģ�ֻ��24λ���¼�λ
//����¼����õ���16λ�ģ�ֻ��16λ���¼�λ
//ע����ݺ��޸ĵģ������¼���32λ�Ļ���16λ��
//����Ȥ��λ
#define KEY1_EVENT  (0x01 << 0)//�����¼�1�����λ0
#define KEY2_EVENT  (0x01 << 1)//�����¼�2�����λ1
#define KEY3_EVENT  (0x01 << 2)//�����¼�3�����λ2
#define KEY4_EVENT  (0x01 << 3)//�����¼�4�����λ3

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

	//�����¼�
	Event_Handle = xEventGroupCreate();	 
  if(NULL != Event_Handle) printf("Event_Handle �¼������ɹ�!\r\n");
	
	//����LED0_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED0_Task,		      //������
								(const char* 	  )"LED0_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				        //�������ȼ�
								(TaskHandle_t*  )&LED0_Task_Handle);//������ 
								
	if(xReturn == pdPASS) printf("LED0_Task���񴴽��ɹ�!\r\n");
	
	//����LED1_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LED1_Task,		      //������
								(const char* 	  )"LED1_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				        //�������ȼ�
								(TaskHandle_t*  )&LED1_Task_Handle);//������ 
								
	if(xReturn == pdPASS) printf("LED1_Task���񴴽��ɹ�!\r\n");
	
	//����Key_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Key_Task,		     //������
								(const char* 	  )"Key_Task",		   //��������
								(uint32_t 		  )128,					     //�����ջ��С
								(void* 		  	  )NULL,				     //���ݸ��������Ĳ���
								(UBaseType_t 	  )3, 				       //�������ȼ�
								(TaskHandle_t*  )&KEY_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("Key_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ��LED0����صĲ���
*������    ��LED0_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED0_Task(void* parameter)
{
	EventBits_t r_event;  //����һ���¼����ձ���
	while (1)
	{
		//�ȴ��¼�����
		r_event = xEventGroupWaitBits(Event_Handle,         //�¼�������
                                  KEY1_EVENT|KEY2_EVENT,//�����̸߳���Ȥ���¼�
                                  pdTRUE,               //�˳�ʱ����¼�λ
                                  pdTRUE,               //�������Ȥ�������¼�(�߼���)
                                  portMAX_DELAY);       //ָ����ʱ�¼�,һֱ��
																	
		if((r_event & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT)) 
    {
      printf ( "KEY1��KEY2������\r\n");		
      LED0_TOGGLE;       //LED0	��ת
    }
    else
      printf ( "�¼�����\r\n");	
		
		
	}
}


/*********************************************************************
*��������  ��LED1����صĲ���
*������    ��LED1_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LED1_Task(void* parameter)
{
	EventBits_t r_event;  //����һ���¼����ձ���
	while (1)
	{
		//�ȴ��¼�����
		r_event = xEventGroupWaitBits(Event_Handle,         //�¼�������
                                  KEY3_EVENT|KEY4_EVENT,//�����̸߳���Ȥ���¼�
                                  pdTRUE,               //�˳�ʱ����¼�λ
                                  pdFALSE,              //�������Ȥ������һ���¼�(�߼���)
                                  portMAX_DELAY);       //ָ����ʱ�¼�,һֱ��
																	
		if(r_event & (KEY3_EVENT|KEY4_EVENT))
    {
      printf ( "KEY1��KEY2������\r\n");		
      LED1_TOGGLE;       //LED1	��ת
    }
    else
      printf ( "�¼�����\r\n");	
			
		vTaskDelay(30);
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
	while (1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				printf ( "KEY1������,�����¼�1���¼�3\r\n" );
				//�����¼�1
				xEventGroupSetBits(Event_Handle,KEY1_EVENT);
				xEventGroupSetBits(Event_Handle,KEY3_EVENT);
				break;
			case 2: 
				printf ( "KEY2�����£������¼�2���¼�4\r\n" );	
				//�����¼�2
				xEventGroupSetBits(Event_Handle,KEY2_EVENT); 	
				xEventGroupSetBits(Event_Handle,KEY4_EVENT);
				break;
			default: break;
		}
		
		vTaskDelay(30);
	}
}




