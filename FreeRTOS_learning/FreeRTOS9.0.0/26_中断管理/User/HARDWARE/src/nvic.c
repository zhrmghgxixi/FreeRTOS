#include "stm32f10x.h" 
#include "stdio.h"
//FreeRTOS����ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void delay_ms(u8 ms)
{
	u32 i;
	for(i=0; i< ms*100; i++);
}

/******************************************�ⲿ�ж�******************************************/
/* ���������ⲿ���� */
extern QueueHandle_t Test_Queue;
static uint32_t send_data1 = 1;
static uint32_t send_data2 = 2;
/******************************************************************
*��������  ���ⲿ�ж�3������
*������    ��EXTI3_IRQHandler
*��������  ��void
*��������ֵ��void
*����      ��
*******************************************************************/
void EXTI3_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	//ȷ���Ƿ������EXTI Line�ж�
  uint32_t ulReturn;
  //�����ٽ�Σ��ٽ�ο���Ƕ��
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
	//�ж��ⲿ�ж���3�Ƿ���Ĵ���   SET=1   RESET=0
  if(EXTI_GetFlagStatus(EXTI_Line3) == SET)
  {
    delay_ms(60);
		//�˲�
    if(!(GPIOE->IDR & (1<<3)))
    {
			// ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0 
			xQueueSendFromISR(Test_Queue, //��Ϣ���еľ��
												&send_data1,//���͵���Ϣ���� 
												&pxHigherPriorityTaskWoken);
			
			//�����Ҫ�Ļ�����һ�������л�
			portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
  }
	 EXTI->PR |= (1<<3);
	//�˳��ٽ��
  taskEXIT_CRITICAL_FROM_ISR(ulReturn);
}

/******************************************************************
*��������  ���ⲿ�ж�4������
*������    ��EXTI4_IRQHandler
*��������  ��void
*��������ֵ��void
*����      ��
*******************************************************************/
void EXTI4_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
  //ȷ���Ƿ������EXTI Line�ж�
  uint32_t ulReturn;
  //�����ٽ�Σ��ٽ�ο���Ƕ��
	ulReturn = taskENTER_CRITICAL_FROM_ISR();

	//�ж��ⲿ�ж���4�Ƿ���Ĵ���   SET=1   RESET=0
  if(EXTI_GetFlagStatus(EXTI_Line4) == SET)
  {
    delay_ms(60);
		//�˲�
    if(!(GPIOE->IDR & (1<<4)))
    {
			// ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0 
			xQueueSendFromISR(Test_Queue, //��Ϣ���еľ��
												&send_data2,//���͵���Ϣ���� 
												&pxHigherPriorityTaskWoken);
			
			//�����Ҫ�Ļ�����һ�������л�
			portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
  }
	
	EXTI->PR |= (1<<4);
	//�˳��ٽ��
  taskEXIT_CRITICAL_FROM_ISR(ulReturn);
}





