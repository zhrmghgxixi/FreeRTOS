//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "limits.h"
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


//��������֪ͨ��������
static TaskHandle_t Take_Task_Handle = NULL;
//��������֪ͨ��������
static void Take_Task(void* pvParameters);

//��������֪ͨ��������
static TaskHandle_t Give_Task_Handle = NULL;
//��������֪ͨ��������
static void Give_Task(void* pvParameters);


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
	
	//����Take_Task����
	xReturn = xTaskCreate(
												(TaskFunction_t	)Take_Task,		      //������
												(const char* 	  )"Take_Task",		    //��������
												(uint16_t 		  )128,	                  //�����ջ��С
												(void* 		  	  )NULL,				          //���ݸ��������Ĳ���
												(UBaseType_t 	  )2, 	                  //�������ȼ�
												(TaskHandle_t*  )&Take_Task_Handle);//������  
	if(pdPASS == xReturn) printf("����Take_Task����ɹ�!\r\n");
	
	//����Give_Task����
	xReturn = xTaskCreate(
												(TaskFunction_t	)Give_Task,		      //������
												(const char* 	  )"Give_Task",		    //��������
												(uint16_t 		  )128,	              //�����ջ��С
												(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
												(UBaseType_t 	  )3, 	              //�������ȼ�
												(TaskHandle_t*  )&Give_Task_Handle);//������  
	if(pdPASS == xReturn) printf("����Give_Task����ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}


/*********************************************************************
*��������  ����������֪ͨ��صĲ���
*������    ��Take_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Take_Task(void* parameter)
{
	uint32_t take_num = 0;  //��������֪ͨ����ֵ
	
  while(1)
	{
		if(KEY0_Scan() == 1)
		{
			//�ȴ�����֪ͨ��û��Ϣ�Ͳ��ȴ�
			take_num = ulTaskNotifyTake(pdFALSE, 0);
			
			if(take_num > 0) printf("KEY0�����£��ɹ����뵽ͣ��λ����ǰ��λΪ %d\r\n", take_num);                      
			else             printf("KEY0�����£���λ�Ѿ�û���ˣ��밴KEY1�ͷų�λ\r\n");
		}
		
		vTaskDelay(30);
	}
}


/*********************************************************************
*��������  ����������֪ͨ��صĲ���
*������    ��Give_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Give_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;    //����һ��������Ϣ����ֵ��pdTRUE���ɹ�

  while(1)
	{
				
		if(KEY1_Scan() == 2)
		{
			xReturn = xTaskNotifyGive(Take_Task_Handle);
			if( xReturn == pdPASS ) printf("KEY1�����£��ͷ�1��ͣ��λ��\r\n");
		}

		vTaskDelay(30);
	}
}

