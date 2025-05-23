//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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


//�����ȼ�������
static TaskHandle_t LowPriority_Task_Handle=NULL;	
//�����ȼ���������
static void LowPriority_Task(void* parameter);

//�����ȼ�������
static TaskHandle_t MidPriority_Task_Handle=NULL;	
//�����ȼ���������
static void MidPriority_Task(void* parameter);

//�����ȼ�������
static TaskHandle_t HighPriority_Task_Handle=NULL;	
//�����ȼ���������
static void HighPriority_Task(void* parameter);

//��ֵ�ź������
SemaphoreHandle_t BinarySem_Handle =NULL;

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

	//������ֵ�ź���
  BinarySem_Handle = xSemaphoreCreateBinary();
  if(NULL != BinarySem_Handle) printf("����BinarySem_Handle��ֵ�ź����ɹ�!\r\n");
	xSemaphoreGive( BinarySem_Handle );//�ͷ�һ�ζ�ֵ�ź���
	
	//����LowPriority_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)LowPriority_Task,	       //������
								(const char* 	  )"LowPriority_Task",       //��������
								(uint32_t 		  )128,					             //�����ջ��С
								(void* 		  	  )NULL,				             //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				               //�������ȼ�
								(TaskHandle_t*  )&LowPriority_Task_Handle);//������
								
	if(xReturn == pdPASS) printf("LowPriority_Task���񴴽��ɹ�!\r\n");
								
	//����MidPriority_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)MidPriority_Task,		     //������
								(const char* 	  )"MidPriority_Task",		   //��������
								(uint32_t 		  )128,					             //�����ջ��С
								(void* 		  	  )NULL,				             //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				               //�������ȼ�
								(TaskHandle_t*  )&MidPriority_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("MidPriority_Task���񴴽��ɹ�!\r\n");
	
	//����HighPriority_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)HighPriority_Task,		      //������
								(const char* 	  )"HighPriority_Task",		    //��������
								(uint32_t 		  )128,					              //�����ջ��С
								(void* 		  	  )NULL,				              //���ݸ��������Ĳ���
								(UBaseType_t 	  )3, 				                //�������ȼ�
								(TaskHandle_t*  )&HighPriority_Task_Handle);//������ 
																			
	if(xReturn == pdPASS) printf("HighPriority_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  �������ȼ�������صĲ���
*������    ��LowPriority_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void LowPriority_Task(void* parameter)
{
	static uint32_t i;
	BaseType_t xReturn = pdTRUE;//����һ����ֵ��Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while (1)
	{
		printf("LowPriority_Task ��ȡ�ź���\r\n");
		
    //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,//��ֵ�ź������
                             portMAX_DELAY);  //�ȴ�ʱ��
															
    if( xReturn == pdTRUE ) printf("LowPriority_Task Running\r\n");
    
    for(i=0;i<2000000;i++)//ģ������ȼ�����ռ���ź���
		{
			taskYIELD();//�����������
		}
		
    printf("LowPriority_Task �ͷ��ź���!\r\n");
    xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
    
    vTaskDelay(500);
	}
}


/*********************************************************************
*��������  �������ȼ�������صĲ���
*������    ��MidPriority_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void MidPriority_Task(void* parameter)
{	
	while (1)
	{
		printf("MidPriority_Task Running\r\n");
		vTaskDelay(500);
	}
}


/*********************************************************************
*��������  �������ȼ�������صĲ���
*������    ��HighPriority_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void HighPriority_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE; //����һ����ֵ��Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while (1)
	{
		printf("HighPriority_Task ��ȡ�ź���\r\n");
		
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,  //��ֵ�ź������
														 portMAX_DELAY);    //�ȴ�ʱ��
														 
		if(pdTRUE == xReturn)printf("HighPriority_Task Running\r\n");

		printf("HighPriority_Task �ͷ��ź���\r\n");
		xReturn = xSemaphoreGive( BinarySem_Handle );//������ֵ�ź���
		if(pdTRUE == xReturn)printf("HighPriority_Task Running\r\n");

		vTaskDelay(500);
	}
}




