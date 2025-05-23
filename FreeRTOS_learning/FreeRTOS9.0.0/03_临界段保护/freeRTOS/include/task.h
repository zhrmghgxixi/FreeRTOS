#ifndef TASK_H
#define TASK_H

	#include "list.h"
	

	/* ������ */
	typedef void * TaskHandle_t;
	#define taskYIELD()			portYIELD()      //�����л�
	
	
	/***************************************************************************
	*                               ��������                                   *
	****************************************************************************/
	extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
	
	/***************************************************************************
	*                               ��������                                   *
	****************************************************************************/
	static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
																			const char * const pcName,              /* �������ƣ��ַ�����ʽ */
																			const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
																			void * const pvParameters,              /* �����β� */
																			TaskHandle_t * const pxCreatedTask,     /* ������ */
																			TCB_t *pxNewTCB );                      /* ������ƿ�ָ�� */
	
	#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
																	const char * const pcName,           /* �������ƣ��ַ�����ʽ */
																	const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
																	void * const pvParameters,           /* �����β� */
																	StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																	TCB_t * const pxTaskBuffer );         /* ������ƿ�ָ�� */	
	#endif /* configSUPPORT_STATIC_ALLOCATION */
	
	void prvInitialiseTaskLists( void );
	void vTaskStartScheduler( void );
	void vTaskSwitchContext( void );

#endif
