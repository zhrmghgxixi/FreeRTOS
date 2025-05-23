#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "usart.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h" 

/* ����ͳ������ʱ�� */
volatile uint32_t CPU_RunTime = 0UL;

/******************************************��ʱ���ж�******************************************/
/******************************************
*��������  ����ʱ��6���жϷ�����
*������    ��TIM6_DAC_IRQHandler ��
*��������  ����
*��������ֵ����
*��������  ��
*********************************************/
void TIM6_IRQHandler(void)
{
	//���TIM6�����жϷ������
  if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  { 
    CPU_RunTime++;
  }
	//���TIMx�����жϱ�־ 
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}


