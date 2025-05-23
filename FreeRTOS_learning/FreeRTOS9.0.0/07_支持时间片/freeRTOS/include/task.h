#ifndef TASK_H
#define TASK_H

	#include "list.h"
	

	/* ������ */
	typedef void * TaskHandle_t;
	#define taskYIELD()			portYIELD()      //�����л�
	#define tskIDLE_PRIORITY			       ( ( UBaseType_t ) 0U )
	
	//�����ٽ���
	#define taskENTER_CRITICAL()		        portENTER_CRITICAL()
	#define taskENTER_CRITICAL_FROM_ISR()   portSET_INTERRUPT_MASK_FROM_ISR()

	#define taskEXIT_CRITICAL()			        portEXIT_CRITICAL()
	#define taskEXIT_CRITICAL_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
	
	
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
																			UBaseType_t uxPriority,                 /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																			TaskHandle_t * const pxCreatedTask,     /* ������ */
																			TCB_t *pxNewTCB );                      /* ������ƿ�ָ�� */
	
	#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
																	const char * const pcName,           /* �������ƣ��ַ�����ʽ */
																	const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
																	void * const pvParameters,           /* �����β� */
																	UBaseType_t uxPriority,              /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																	StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																	TCB_t * const pxTaskBuffer );         /* ������ƿ�ָ�� */	
	#endif /* configSUPPORT_STATIC_ALLOCATION */
	
	void prvInitialiseTaskLists( void );
	void vTaskStartScheduler( void );
	void vTaskSwitchContext( void );
	static portTASK_FUNCTION( prvIdleTask, pvParameters );
	void vTaskDelay( const TickType_t xTicksToDelay );
	BaseType_t xTaskIncrementTick( void );
	static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB );
	static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait );
	static void prvResetNextTaskUnblockTime( void );

#endif
