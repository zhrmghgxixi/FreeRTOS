#include "stm32f10x.h"
#include "key.h"

static void delay_ms(u8 ms)
{
	u32 i;
	for(i=0; i< ms*100; i++);
}

/*****************************************************
*��������  ����KEY��Ӧ��GPIO�ڽ��г�ʼ������
*������    ��KEY_Init
*��������  ��void
*��������ֵ��void
*����      ��
*            KEY0     PE4
*            KEY1     PE3
*            KEY_UP   PA0
*            KEY0��KEY1 �͵�ƽ���£��ߵ�ƽ̧��
*            KEY_UP     �ߵ�ƽ���£��͵�ƽ̧��
*                                 -------------���ģʽ
********************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;         //GPIOx���ýṹ��
	
	/*GPIOx����*/
  //GPIOxʱ��ʹ��  GPIOA��GPIOE
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  //GPIOx����
  //PE
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	    //PE3/4�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 		        //��������
  GPIO_Init(GPIOE, &GPIO_InitStructure);					          //�����趨������ʼ��GPIE PE3/4  
  //PA
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	                //PA0�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ; 		        //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);					          //�����趨������ʼ��GPIOA PA0
}

/*****************************************************
*��������  ������ɨ�躯��
*������    ��KEY_Scan
*��������  ��void
*��������ֵ��u8
*����      ��
             KEY0     PE4
             KEY1     PE3
             KEY_UP   PA0
             KEY0��KEY1 �͵�ƽ���£��ߵ�ƽ̧��
             KEY_UP     �ߵ�ƽ���£��͵�ƽ̧��
                                  -------------���ģʽ
********************************************************/
u8 KEY_Scan(void)
{
  u8 temp = 0;
  static u8 key_flay = 1;
  if(key_flay && (KEY_UP||!(KEY1)||!(KEY0)))
  {
    delay_ms(10);
    if(KEY_UP||!(KEY1)||!(KEY0))
    {
      key_flay = 0;
      if(!(KEY0))        temp = 1;
      else if(!(KEY1))   temp = 2;
      else if(KEY_UP)    temp = 3;
    }
  }
  if(!(KEY_UP) && (KEY1) && (GPIOE->IDR & (1<<4)))
  {
    key_flay = 1;
  }
  return temp;
}



/*****************************************************
*��������  ������KEY0ɨ�躯��
*������    ��KEY0_Scan
*��������  ��void
*��������ֵ��u8
*����      ��
             KEY0     PE4
             KEY0 �͵�ƽ���£��ߵ�ƽ̧��
                                  -------------���ģʽ
********************************************************/
u8 KEY0_Scan(void)
{
  // ����Ƿ��а�������
	if(KEY0 == 0)  
	{	
		delay_ms(3);
		if(KEY0 == 0)
		{
			//�ȴ������ͷ�
			while(KEY0 == 0);   
			return 	1;	
		} 
	}

	return 0;
}


/*****************************************************
*��������  ������KEY1ɨ�躯��
*������    ��KEY1_Scan
*��������  ��void
*��������ֵ��u8
*����      ��
             KEY1     PE3
             KEY1 �͵�ƽ���£��ߵ�ƽ̧��
                                  -------------���ģʽ
********************************************************/
u8 KEY1_Scan(void)
{
  // ����Ƿ��а�������
	if(KEY1 == 0)  
	{	
		delay_ms(3);
		if(KEY1 == 0)
		{
			//�ȴ������ͷ�
			while(KEY1 == 0);   
			return 	2;
		}	 
	}

	return 0;
}





