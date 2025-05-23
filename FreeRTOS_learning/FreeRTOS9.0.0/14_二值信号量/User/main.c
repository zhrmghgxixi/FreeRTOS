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


//��ȡ��ֵ�ź���������
static TaskHandle_t Take_Task_Handle=NULL;	
//��ȡ��ֵ�ź�����������
static void Take_Task(void* parameter);

//�ͷŶ�ֵ�ź���������
static TaskHandle_t Give_Task_Handle=NULL;	
//�ͷŶ�ֵ�ź�����������
static void Give_Task(void* parameter);

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
	
	//����Take_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Take_Task,		      //������
								(const char* 	  )"Take_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )1, 				        //�������ȼ�
								(TaskHandle_t*  )&Take_Task_Handle);//������ 
								
	if(xReturn == pdPASS) printf("Take_Task���񴴽��ɹ�!\r\n");
								
	//����Give_Task����
	xReturn = xTaskCreate(
								(TaskFunction_t	)Give_Task,		      //������
								(const char* 	  )"Give_Task",		    //��������
								(uint32_t 		  )128,					      //�����ջ��С
								(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
								(UBaseType_t 	  )2, 				        //�������ȼ�
								(TaskHandle_t*  )&Give_Task_Handle);//������
																			
	if(xReturn == pdPASS) printf("Give_Task���񴴽��ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ����ȡ��ֵ�ź�����صĲ���
*������    ��Take_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Take_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;//����һ����ֵ��Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while (1)
	{
		//��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(BinarySem_Handle,//��ֵ�ź������
                             portMAX_DELAY);  //�ȴ�ʱ��
														 
    if(pdTRUE == xReturn) printf("BinarySem_Handle��ֵ�ź�����ȡ�ɹ�!\r\n\r\n");
	}
}


/*********************************************************************
*��������  ���ͷŶ�ֵ�ź�����صĲ���
*������    ��Give_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Give_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE; //����һ����ֵ��Ϣ����ֵ��Ĭ��ΪpdTRUE
	
	while (1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				//�ͷŶ�ֵ�ź���
				xReturn = xSemaphoreGive(BinarySem_Handle);//������ֵ�ź���
				if( xReturn == pdTRUE ) printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\r\n");
				else                    printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\r\n");
				break;
					
			case 2: 
				//�ͷŶ�ֵ�ź���
				xReturn = xSemaphoreGive(BinarySem_Handle);//������ֵ�ź���
				if( xReturn == pdTRUE ) printf("BinarySem_Handle��ֵ�ź����ͷųɹ�!\r\n");
				else                    printf("BinarySem_Handle��ֵ�ź����ͷ�ʧ��!\r\n");	
				break;
				
			default: break;
		}
		
		vTaskDelay(20);
	}
}

