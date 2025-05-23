//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
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


//KEY������
static TaskHandle_t KEY_Task_Handle=NULL;	
//KEY�����������
static void Key_Task(void* parameter);

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
*��������  ��������صĲ���
*������    ��Key_Task
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Key_Task(void* parameter)
{
	uint32_t g_memsize;

	while (1)
	{
		switch(KEY_Scan())
		{
			case 1: 
				if(NULL == Test_Ptr)
				{
					//��ȡ��ǰδ������ڴ�Ѵ�С
					g_memsize = xPortGetFreeHeapSize();
					printf("ϵͳ��ǰδ������ڴ��СΪ %d �ֽڣ���ʼ�����ڴ�\r\n",g_memsize);
					Test_Ptr = pvPortMalloc(1024);
					if(NULL != Test_Ptr)
					{
						printf("�ڴ�����ɹ�\r\n");
						printf("���뵽���ڴ��ַΪ%#x\r\n",(int)Test_Ptr);

						//��ȡ��ǰδ������ڴ�Ѵ�С
						g_memsize = xPortGetFreeHeapSize();
						printf("ϵͳ��ǰ�ڴ�ʣ����СΪ %d �ֽ�\r\n",g_memsize);
										
						//��Test_Ptr��д�뵱����:��ǰϵͳʱ��
						sprintf((char*)Test_Ptr,"��ǰϵͳTickCount = %d",xTaskGetTickCount());
						printf("д��������� %s \r\n",(char*)Test_Ptr);
					}
				}
				else
				{
					printf("���Ȱ���KEY1�ͷ��ڴ�������\r\n");
				}
				break;
				
			case 2: 
				if(NULL != Test_Ptr)
				{
					printf("�ͷ��ڴ�\r\n");
					vPortFree(Test_Ptr);	//�ͷ��ڴ�
					Test_Ptr=NULL;
					/* ��ȡ��ǰ��ʣ����С */
					g_memsize = xPortGetFreeHeapSize();
					printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ��ڴ��ͷ����\r\n",g_memsize);
				}
				else
				{
					printf("���Ȱ���KEY0�����ڴ����ͷ�\n");
				}
				break;
				
			default: break;
		}
		
		vTaskDelay(30);
	}
}




