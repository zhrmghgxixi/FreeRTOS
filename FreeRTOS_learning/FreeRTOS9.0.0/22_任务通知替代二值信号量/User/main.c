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

//��������֪ͨ1��������
static TaskHandle_t Receive1_Task_Handle = NULL;
//��������֪ͨ1��������
static void Receive1_Task(void* pvParameters);

//��������֪ͨ2��������
static TaskHandle_t Receive2_Task_Handle = NULL;
//��������֪ͨ2��������
static void Receive2_Task(void* pvParameters);

//��������֪ͨ��������
static TaskHandle_t Send_Task_Handle = NULL;
//��������֪ͨ��������
static void Send_Task(void* pvParameters);/* Send_Task����ʵ�� */


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
	
	//����Receive1_Task����
	xReturn = xTaskCreate(
												(TaskFunction_t	)Receive1_Task,		      //������
												(const char* 	  )"Receive1_Task",		    //��������
												(uint16_t 		  )128,	                  //�����ջ��С
												(void* 		  	  )NULL,				          //���ݸ��������Ĳ���
												(UBaseType_t 	  )1, 	                  //�������ȼ�
												(TaskHandle_t*  )&Receive1_Task_Handle);//������  
	if(pdPASS == xReturn) printf("����Receive1_Task����ɹ�!\r\n");
	
	//����Receive2_Task����
	xReturn = xTaskCreate(
												(TaskFunction_t	)Receive2_Task,		      //������
												(const char* 	  )"Receive2_Task",		    //��������
												(uint16_t 		  )128,	                  //�����ջ��С
												(void* 		  	  )NULL,				          //���ݸ��������Ĳ���
												(UBaseType_t 	  )2, 	                  //�������ȼ�
												(TaskHandle_t*  )&Receive2_Task_Handle);//������  
	if(pdPASS == xReturn) printf("����Receive2_Task����ɹ�!\r\n");
	
	//����Send_Task����
	xReturn = xTaskCreate(
												(TaskFunction_t	)Send_Task,		      //������
												(const char* 	  )"Send_Task",		    //��������
												(uint16_t 		  )128,	              //�����ջ��С
												(void* 		  	  )NULL,				      //���ݸ��������Ĳ���
												(UBaseType_t 	  )3, 	              //�������ȼ�
												(TaskHandle_t*  )&Send_Task_Handle);//������  
	if(pdPASS == xReturn) printf("����Send_Task����ɹ�!\r\n");
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  ����������֪ͨ1��صĲ���
*������    ��Receive1_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Receive1_Task(void* parameter)
{
  while(1)
	{
		//�ȴ�����֪ͨ��û��Ϣһֱ��
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
    printf("Receive1_Task ����֪ͨ��ȡ�ɹ�!\r\n");                      
		LED0_TOGGLE;   //LED��ת
	}
}


/*********************************************************************
*��������  ����������֪ͨ2��صĲ���
*������    ��Receive2_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Receive2_Task(void* parameter)
{
  while(1)
	{
		//�ȴ�����֪ͨ��û��Ϣһֱ��
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
    printf("Receive2_Task ����֪ͨ��ȡ�ɹ�!\r\n"); 
		
		LED1_TOGGLE;   //LED��ת
	}
}


/*********************************************************************
*��������  ����������֪ͨ��صĲ���
*������    ��Send_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Send_Task(void* parameter)
{
	BaseType_t xReturn = pdTRUE;    //����һ��������Ϣ����ֵ��pdTRUE���ɹ�

  while(1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				xReturn = xTaskNotifyGive(Receive1_Task_Handle);
				if( xReturn == pdPASS ) printf("Receive1_Task_Handle ����֪ͨ��Ϣ���ͳɹ�!\r\n");
				break;
				
			case 2: 
				xReturn = xTaskNotifyGive(Receive2_Task_Handle);
				if( xReturn == pdPASS ) printf("Receive2_Task_Handle ����֪ͨ��Ϣ���ͳɹ�!\r\n");
				break;
			default: break;
		}
		
		vTaskDelay(30);
	}
}

