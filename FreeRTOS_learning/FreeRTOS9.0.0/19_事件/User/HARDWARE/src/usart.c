#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"

/*****************************************************
*��������  ����USART1��Ӧ��GPIO�ڽ��г�ʼ������
*������    ��USART1_Init
*��������  ��u32 baud
*��������ֵ��void
*����      ��
*           PA9     TX     ---------���
*           PA10    RX     ---------����
********************************************************/
void USART1_Init(u32 baud)
{
  GPIO_InitTypeDef  GPIO_InitStruct;     //GPIOx���ýṹ��
  USART_InitTypeDef USART_InitStruct;    //USARTx���ýṹ��
  
  //ʱ��ʹ��   GPIOA   USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); 
  
  /*GPIOx��ʼ������*/
  //GPIOx�˿�����
  //PA9   USART1_TX ----- ���ô������
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;				     //PA9 �˿�����
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP; 	  	 //����������
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		   //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStruct);                  //�����趨������ʼ��PA9
  //PA10  USART1_RX ----- ���ô�������
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;				     //PA10 �˿�����
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;  //��������
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		   //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStruct);                  //�����趨������ʼ��PA10
  
  /*USART ��ʼ������*/
  //USART1����
  USART_InitStruct.USART_BaudRate      = baud;                 //����������(USART->BRR�Ĵ���)
  USART_InitStruct.USART_WordLength    = USART_WordLength_8b;  //�ֳ�Ϊ8λ���ݸ�ʽ:һ����ʼλ�� 8������λ�� n��ֹͣλ��(USART->CR1�Ĵ����ĵ�12λ)
  USART_InitStruct.USART_StopBits      = USART_StopBits_1;     //һ��ֹͣλ(USART->CR2�Ĵ����ĵ�12��13λ)
  USART_InitStruct.USART_Parity        = USART_Parity_No;      //����żУ��(USART->CR1�Ĵ����ĵ�10λ)
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������(USART->CR3�Ĵ����ĵ�9λ)
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ(USART->CR1�Ĵ����ĵ�2��3λ)
  USART_Init(USART1,&USART_InitStruct);                        //��ʼ��USART1
  
  //ʹ�ܴ���1
  USART_Cmd(USART1, ENABLE);
}

/*****************************************************
*��������  ������1����һ���ֽڵ����ݣ�u8��
*������    ��USART1_Send_Byte
*��������  ��u8 data
*��������ֵ��void
*����      ��
*           PA9     TX     ---------���
*           PA10    RX     ---------����
********************************************************/
void USART1_Send_Byte(u8 data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);//�ȴ����ͽ���(USART1->SR�ĵ�6λ)
	USART_SendData(USART1, data);                           //����һ���ֽڵ�����(USART1->DR)
}

/*****************************************************
*��������  ������1����һ���ֽڵ����ݣ�u8��
*������    ��USART1_Receive_Byte
*��������  ��void
*��������ֵ��u8
*����      ��
********************************************************/
u8 USART1_Receive_Byte(void)
{
  u8 data;
	
  while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=SET){};//�ȴ����ͽ���(USART1->SR�ĵ�5λ)
  data = USART_ReceiveData(USART1);                         //��ȡһ���ֽڵ�����(USART1->DR)
  
  return data;
}

/******************************************************************
*��������  ������1����һ���ַ���
*������    ��USART1_Send_Str
*��������  ��u8 *str
*��������ֵ��void
*����      ��
            PA9   TX    ���
            PA10  RX    ����
*******************************************************************/
void USART1_Send_Str(u8 *str)
{
  while(*str != '\0')
  {
    USART1_Send_Byte(*str);
    str++;
  }
}

/******************************************************************
*��������  ������1����һ���ַ���
*������    ��USART1_Receive_Str
*��������  ��u8 *str
*��������ֵ��void 
*����      ��
            PA9   TX    ���
            PA10  RX    ����
*******************************************************************/
void USART1_Receive_Str(u8 *str)
{
  u8 data;
  while(1)
  {
    data = USART1_Receive_Byte();
    if(data == '#')   //�� # Ϊ������־
    {
      break;
    }
    *str = data;
    str++;
  }
  *str = '\0'; 
}

























///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}








