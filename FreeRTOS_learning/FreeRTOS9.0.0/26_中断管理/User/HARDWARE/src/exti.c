#include "stm32f10x.h"

/*****************************************************
*��������  ���ⲿ�ж�3/4��ʼ��
*������    ��EXTI34_Init
*��������  ��void
*��������ֵ��void
*����      ��
*           �͵�ƽ���£��ߵ�ƽ̧��
*						KEY0     PE4
*           KEY1     PE3
********************************************************/
void EXTI34_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  //GPIOx���ýṹ��
	NVIC_InitTypeDef  NVIC_InitStruct;  //�ж����ýṹ��
	EXTI_InitTypeDef  EXTI_InitStruct;  //�ⲿ�ж������ýṹ��
	
	//ʱ��ʹ��     AFIO  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);   	//ʹ�ܸ��ù���ʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);   //GPIOE
	
	//GPIOx����
  //PE
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	      //PE3/4�˿�����
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU ; 	          //��������
  GPIO_Init(GPIOE, &GPIO_InitStruct);					            //�����趨������ʼ��GPIE PE4 
	
	/*�ж�����*/
	//�ⲿ�ж�3
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;			        //�ⲿ�ж�3
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x05;	//��ռ���ȼ�5
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;				//�����ȼ�0 
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;							//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStruct);  	                          //����
	//�ⲿ�ж�4
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;			        //�ⲿ�ж�4
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x05;	//��ռ���ȼ�5
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;				//�����ȼ�0 
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;							//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStruct);  	                          //����
	
	/*�ⲿ�ж�����*/
	//�ⲿ�ж����üĴ�������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
	//��ʼ�����ⲿ�ж���
	//�ⲿ�ж�3
	EXTI_InitStruct.EXTI_Line=EXTI_Line3;                     //ѡ���ⲿ�ж���3
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	        //�ж��¼�
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;      //�½��ش���    (EXTI_RTSR�Ĵ����ĵ�0λ)
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                    //ʹ��3�ߵ��ж� (EXTI->IMR�Ĵ����ĵ�0λ)
  EXTI_Init(&EXTI_InitStruct);	  	                        //��ʼ���ⲿ�ж�3
	//�ⲿ�ж�4
  EXTI_InitStruct.EXTI_Line=EXTI_Line4;                     //ѡ���ⲿ�ж���4
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	        //�ж��¼�
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;      //�½��ش���    (EXTI_RTSR�Ĵ����ĵ�0λ)
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;                    //ʹ��4�ߵ��ж� (EXTI->IMR�Ĵ����ĵ�0λ)
  EXTI_Init(&EXTI_InitStruct);	  	                        //��ʼ���ⲿ�ж�4
}


