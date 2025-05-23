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

//�����ʱ��1���
static TimerHandle_t Swtmr1_Handle =NULL;
//�����ʱ����������
static void Swtmr1_Callback(void* parameter);

//�����ʱ��2���
static TimerHandle_t Swtmr2_Handle =NULL;
//�����ʱ��2�������� 
static void Swtmr2_Callback(void* parameter);

static void BSP_Init(void);


static uint32_t TmrCb_Count1 = 0; /* ��¼�����ʱ��1�ص�����ִ�д��� */
static uint32_t TmrCb_Count2 = 0; /* ��¼�����ʱ��2�ص�����ִ�д��� */

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
	//�����ٽ�������������������Ǳ��뱣֤���ٽ���
	taskENTER_CRITICAL();   
	
	//������ʱ��1����
	Swtmr1_Handle=xTimerCreate((const char*		)"AutoReloadTimer",       //��ʱ��������
                            (TickType_t			)1000,                    //��ʱ������----1000(tick)
                            (UBaseType_t		)pdTRUE,                  //����ģʽ
                            (void*				  )1,                       //Ϊÿ����ʱ������һ��������ΨһID
                            (TimerCallbackFunction_t)Swtmr1_Callback);//��ʱ��������
	if(Swtmr1_Handle != NULL)  
	{
		xTimerStart(Swtmr1_Handle,0);	//�������ڶ�ʱ��
		printf("���������ʱ��1\r\n");
	}
	
	//������ʱ��2����
	Swtmr2_Handle=xTimerCreate((const char*			)"OneShotTimer",         //��ʱ��������
                             (TickType_t			)5000,                   //��ʱ������---5000(tick)
                             (UBaseType_t			)pdFALSE,                //����ģʽ
                             (void*					  )2,                      //ÿ����ʱ������һ��������ΨһID
                             (TimerCallbackFunction_t)Swtmr2_Callback);//��ʱ�������� 
	if(Swtmr2_Handle != NULL)
	{   
		xTimerStart(Swtmr2_Handle,0);	//�������ڶ�ʱ��
		printf("���������ʱ��2\r\n");
	} 
	
	//ɾ����������AppTaskCreate������(���ǵ�ǰ��������Լ�)
	vTaskDelete(AppTaskCreate_Handle);
	//�˳��ٽ���
	taskEXIT_CRITICAL();            
}

/*********************************************************************
*��������  �������ʱ��1��صĲ���
*������    ��Swtmr1_Callback
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Swtmr1_Callback(void* parameter)
{		
  TickType_t tick_num1;

  TmrCb_Count1++;	//ÿ�ص�һ�μ�һ

  tick_num1 = xTaskGetTickCount();	//��ȡ�δ�ʱ���ļ���ֵ
  
  LED1_TOGGLE;
  
  printf("Swtmr1_Callback����ִ�� %d ��\r\n", TmrCb_Count1);
  printf("�δ�ʱ����ֵ=%d\r\n", tick_num1);
}


/*********************************************************************
*��������  �������ʱ��2��صĲ���
*������    ��Swtmr2_Callback
*��������  ��void* parameter
*��������ֵ��static void
*����      ��
**********************************************************************/
static void Swtmr2_Callback(void* parameter)
{	
  TickType_t tick_num2;

  TmrCb_Count2++;			//ÿ�ص�һ�μ�һ

  tick_num2 = xTaskGetTickCount();	//��ȡ�δ�ʱ���ļ���ֵ
	
	LED0_TOGGLE;

  printf("Swtmr2_Callback����ִ�� %d ��\r\n", TmrCb_Count2);
  printf("�δ�ʱ����ֵ=%d\r\n", tick_num2);
}




