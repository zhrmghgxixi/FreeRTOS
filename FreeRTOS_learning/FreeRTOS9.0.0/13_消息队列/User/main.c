//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//���ǵ�Ӳ������ļ�
#include "stm32f10x.h"
#include "key.h"
#include "usart.h"

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


//������Ϣ������
static TaskHandle_t Receive_Task_Handle=NULL;	
//������Ϣ��������
static void Receive_Task(void* parameter);

//������Ϣ������
static TaskHandle_t Send_Task_Handle=NULL;	
//������Ϣ��������
static void Send_Task(void* parameter);

//������Ϣ���о��
QueueHandle_t Test_Queue =NULL;
//��Ϣ���еĳ���
#define  QUEUE_LEN    4  
//��Ϣ������ÿ����Ϣ�Ĵ�С(��λ���ֽ�)
#define  QUEUE_SIZE   4  

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

	//������Ϣ����
  Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,  //��Ϣ���еĳ���
                            (UBaseType_t ) QUEUE_SIZE);//��Ϣ�Ĵ�С
  if(NULL != Test_Queue) printf("����Test_Queue��Ϣ���гɹ�!\r\n");
	
	//����Receive_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Receive_Task,		     //������
								(const char* 	  )"Receive_Task",		   //��������
								(uint32_t 		  )128,					         //�����ջ��С
								(void* 		  	  )NULL,				         //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				           //�������ȼ�
								(TaskHandle_t*  )&Receive_Task_Handle);//������ 
								
	if(xReturn == pdPASS) printf("Receive_Task���񴴽��ɹ�!\r\n");
								
	//����Send_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Send_Task,		      //������
								(const char* 	  )"Send_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				        //�������ȼ�
								(TaskHandle_t*  )&Send_Task_Handle);//������
																			
	if(xReturn == pdPASS) printf("Send_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ��������Ϣ������صĲ���
*������    ��Receive_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Receive_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;//����һ����Ϣ������Ϣ����ֵ��Ĭ��ΪpdTRUE
  uint32_t r_queue;	          //����һ��������Ϣ�ı���
	
	while (1)
	{
		xReturn = xQueueReceive( Test_Queue,    //��Ϣ���еľ��
                             &r_queue,      //���͵���Ϣ����
                             portMAX_DELAY);//�ȴ�ʱ�� һֱ��
														 
    if(pdTRUE == xReturn) printf("���ν��յ���������%d\r\n\r\n",r_queue);
    else                  printf("���ݽ��ճ���,�������0x%lx\n",xReturn);
	}
}


/*********************************************************************
*��������  ��������Ϣ����Ϣ������صĲ���
*������    ��Send_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Send_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE; //����һ����Ϣ������Ϣ����ֵ��Ĭ��ΪpdTRUE
  uint32_t send_data1 = 1;     //����1
  uint32_t send_data2 = 2;     //����2
	
	while (1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				printf("������Ϣsend_data1��\r\n");
				xReturn = xQueueSend( Test_Queue, //��Ϣ���еľ��
															&send_data1,//���͵���Ϣ����
															0 );        //�ȴ�ʱ�� 0
        if(pdPASS == xReturn) printf("��Ϣsend_data1���ͳɹ�!\r\n\r\n");
				break;
					
			case 2: 
				printf("������Ϣsend_data2��\r\n");
				xReturn = xQueueSend( Test_Queue, //��Ϣ���еľ��
															&send_data2,//���͵���Ϣ����
															0 );        //�ȴ�ʱ�� 0
        if(pdPASS == xReturn) printf("��Ϣsend_data2���ͳɹ�!\r\n\r\n");			
				break;
				
			default: break;
		}
		
		vTaskDelay(20);
	}
}

