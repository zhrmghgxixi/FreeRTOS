#include "FreeRTOS.h"
#include "task.h"

/******************************************************
*
*	��ע�⡿
*				��FreeRTOS�У����ȼ���ԽС���߼����ȼ�Խ��
*   ��RT-Thread��uC/OS�����෴�����ȼ���ԽС���߼�
*   ���ȼ�Խ��
*
*******************************************************/

ListItem_t xStateListItem;

//��������ȫ�ֱ���
portCHAR flag1;
portCHAR flag2;

//��������ջ
//��������128�ִ�С������ջ
//1���ֵ���4���ֽ�
//FreeRTOS�Ƽ�����С����ջΪ512���ֽ�
/*����1��ջ*/
#define TASK1_STACK_SIZE  128
StackType_t Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;  //����1���ƿ�
TaskHandle_t Task1_Handle; //����1���
void Task1_Entry( void *p_arg );    //����1��������

/*����2��ջ*/
#define TASK2_STACK_SIZE  128
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;  //����2���ƿ�
TaskHandle_t Task2_Handle;    //����2���
void Task2_Entry( void *p_arg );    //����2��������



int main(void)
{
	/* Ӳ����ʼ�� */
	/* ��Ӳ����صĳ�ʼ�����������������������û����س�ʼ������ */

	/* �������� */
	Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task1_Entry,   /* ������� */
																		(char *)"Task1",               /* �������ƣ��ַ�����ʽ */
																		(uint32_t)TASK1_STACK_SIZE ,   /* ����ջ��С����λΪ�� */
																		(void *) NULL,                 /* �����β� */
																		(UBaseType_t) 1,               /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																		(StackType_t *)Task1Stack,     /* ����ջ��ʼ��ַ */
																		(TCB_t *)&Task1TCB );          /* ������ƿ� */
																		
	Task2_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,   /* ������� */
																		(char *)"Task2",               /* �������ƣ��ַ�����ʽ */
																		(uint32_t)TASK2_STACK_SIZE ,   /* ����ջ��С����λΪ�� */
																		(void *) NULL,                 /* �����β� */
																		(UBaseType_t) 2,               /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																		(StackType_t *)Task2Stack,     /* ����ջ��ʼ��ַ */
																		(TCB_t *)&Task2TCB );          /* ������ƿ� */																
	
	/* ������������ǰ���ر��ж� */                                  
	portDISABLE_INTERRUPTS();
	/* ��������������ʼ��������ȣ������ɹ��򲻷��� */
  vTaskStartScheduler();  
																		
	while(1);
}

/***************************��������***************************/
void delay (uint32_t count)
{
	for(; count!=0; count--);
}

/*����1*/
void Task1_Entry( void *p_arg )
{
	while(1)
	{
		flag1 = 1;
		vTaskDelay(10);		
		flag1 = 0;
		vTaskDelay(10);
	}
}

/* ����2 */
void Task2_Entry( void *p_arg )
{
	while(1)
	{
		flag2 = 1;
		vTaskDelay(10);		
		flag2 = 0;
		vTaskDelay(10);
	}
}


/* ��ȡ����������ڴ� */
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];   //��������ջ
TCB_t IdleTaskTCB;                                     //����������ƿ�

void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize )
{
		*ppxIdleTaskTCBBuffer=&IdleTaskTCB;             //����������ƿ�
		*ppxIdleTaskStackBuffer=IdleTaskStack;          //��������ջ
		*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; //��������ջ��С
}
