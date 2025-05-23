#include "FreeRTOS.h"
#include "task.h"

// TCB ����������ƿ��Ӣ����д


/********************************ȫ�ֱ���********************************/

/* ��ǰ�������е������������ƿ�ָ�룬Ĭ�ϳ�ʼ��ΪNULL */
TCB_t * volatile pxCurrentTCB = NULL;

//����������
static TaskHandle_t xIdleTaskHandle	= NULL; //(����ֻ���帳ֵ��û����ʹ�ã����Ա���������һ�����棬���������)
//����������ʱ
static volatile TickType_t xTickCount = ( TickType_t ) 0U;

/* ��������б� */
//��������������ڵ������
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;          //��ǰ������ȼ�

static volatile UBaseType_t uxCurrentNumberOfTasks 	= ( UBaseType_t ) 0U;    //��ӵ�������
static UBaseType_t uxTaskNumber = ( UBaseType_t ) 0U;


/********************************�궨��********************************/

/* ��������ӵ������б� */                                    
#define prvAddTaskToReadyList( pxTCB )												      \
	taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );								\
	vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ),  \
									&( ( pxTCB )->xStateListItem ) );                 \
/*����������ȼ��ľ�������*/
//ͨ�÷���
#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )
	/* uxTopReadyPriority ����Ǿ��������������ȼ� */
	#define taskRECORD_READY_PRIORITY( uxPriority )		\
	{																									\
		if( ( uxPriority ) > uxTopReadyPriority )				\
		{																								\
			uxTopReadyPriority = ( uxPriority );					\
		}																								\
	} /* taskRECORD_READY_PRIORITY */
/*-------------------------------------------------------------------*/
	#define taskSELECT_HIGHEST_PRIORITY_TASK()															               \
	{																									                                     \
		UBaseType_t uxTopPriority = uxTopReadyPriority;														           \
		/* Ѱ�Ұ������������������ȼ��Ķ��� */                                               \
		/* listLIST_IS_EMPTY �ж��б��Ƿ�Ϊ�� */                                              \
		while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) ) )							   \
		{																								                                     \
			--uxTopPriority;																			                             \
		}																								                                     \
		/* ��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB */							               \
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );\
		/* ����uxTopReadyPriority */                                                         \
		uxTopReadyPriority = uxTopPriority;																                   \
	} /* taskSELECT_HIGHEST_PRIORITY_TASK */
	
	/* �������궨��ֻ����ѡ���Ż�����ʱ���ã����ﶨ��Ϊ�� */
	#define taskRESET_READY_PRIORITY( uxPriority )
	#define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )
	
/* ����������ȼ��ľ������񣺸��ݴ������ܹ��Ż���ķ��� */
#else
	//������ȼ�
	#define taskRECORD_READY_PRIORITY( uxPriority )	portRECORD_READY_PRIORITY(uxPriority, uxTopReadyPriority)
	
	/*------------------------------------------------------------------------------*/
	//��ȡ������ȼ�����
	#define taskSELECT_HIGHEST_PRIORITY_TASK()												\
	{																								                  \
		UBaseType_t uxTopPriority;																		  \
		/* Ѱ��������ȼ� */								                              \
		portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );	\
		/* ��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB */       \
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );\
	} /* taskSELECT_HIGHEST_PRIORITY_TASK() */
	
	/*------------------------------------------------------------------------------*/
	
	#if 0
	#define taskRESET_READY_PRIORITY( uxPriority )														                        \
	{																									                                                \
		if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == ( UBaseType_t ) 0 )	\
		{																								                                                \
			portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );							              \
		}																								                                                \
	}
	#else
	#define taskRESET_READY_PRIORITY( uxPriority )                          \
	{																							                          \
		portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );	  \
	}
	#endif
#endif 



/************************************************************************/

//���񴴽������ַ�ʽ
//һ���Ƕ�̬������ϵͳ��̬����������ƿ������ջ���ڴ�ռ䣬����ɾ�����ڴ�ռ�ᱻ����
//һ���Ǿ�̬��������Ҫ�Լ����ȶ����������ƿ������ջ���ڴ�ռ��С������ɾ�����ڴ�ռ���Ȼ���ڣ������ֶ��ͷ�
/*��������ʾѡ�����񴴽���ʽ��0-��̬��1-��̬*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

/* ��̬���񴴽����� */
/* �����ջ�� ����ĺ���ʵ�壬 ����Ŀ��ƿ���ϵ���� */
TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
																const char * const pcName,           /* �������ƣ��ַ�����ʽ */
																const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
																void * const pvParameters,           /* �����β� */
																UBaseType_t uxPriority,              /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
																TCB_t * const pxTaskBuffer )         /* ������ƿ�ָ�� */
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;   //����һ��������xReturn������������ָ�������TCB
	
	//�жϴ����������ƿ������ջ��ʼ��ַ�ı�����ַ�Ƿ�Ϊ��
	if((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
	{
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;
		
		/* �����µ����� */
		prvInitialiseNewTask( pxTaskCode,        /* ������� */
													pcName,            /* �������ƣ��ַ�����ʽ */
													ulStackDepth,      /* ����ջ��С����λΪ�� */ 
													pvParameters,      /* �����β� */
													uxPriority,        /* �������ȼ� */
													&xReturn,          /* ������ */ 
													pxNewTCB);         /* ������ƿ� */ 
													
		/* ��������ӵ������б� */
		prvAddNewTaskToReadyList( pxNewTCB );
	}
	else
	{
		xReturn = NULL;
	}

	/* ������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ� */
	return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */


/* ���������� */
static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
																		const char * const pcName,              /* �������ƣ��ַ�����ʽ */
																		const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
																		void * const pvParameters,              /* �����β� */
																		UBaseType_t uxPriority,                 /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																		TaskHandle_t * const pxCreatedTask,     /* ������ */
																		TCB_t *pxNewTCB )                       /* ������ƿ�ָ�� */
{
	StackType_t *pxTopOfStack;    //���ջ����ַ
	UBaseType_t x;	
	
	/* ��ȡջ����ַ */
	//ջ����ַ = ������ʼ��ַ + (����ջ��С - 1)
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	
	/* ������8�ֽڶ��� */
	//���ǽ����������ܱ�8��������
	//����(���¶���ʮ������)��64 -> 64     65 -> 64    68 -> 64     72 -> 72
	//�������� 8 �ֽڶ����ʱ�򣬾ͻ�ճ������ֽڣ�����ʹ��
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	
	
	/* ����������ִ洢��TCB�� */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	
	/* �������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';
	
	/* ��ʼ��TCB�е�xStateListItem����ڵ� */
	//�������ݽڵ��ʼ��
  vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
	
	/* ����xStateListItem�ڵ��ӵ���� */
	//�������ݽڵ��������ƿ�ӵ����
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
	
	/* ��ʼ�����ȼ� */
	if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
	{
		uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
	}
	pxNewTCB->uxPriority = uxPriority;
	
	/* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );
	
	/* ��������ָ��������ƿ� */
  if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}



/* �����б��ʼ�� */
//��ʵ���Ƕ������Ա���и��ڵ��ʼ��
void prvInitialiseTaskLists( void )
{
	UBaseType_t uxPriority;
     
	for(uxPriority = ( UBaseType_t ) 0U;
			uxPriority < ( UBaseType_t ) configMAX_PRIORITIES;
			uxPriority++ )
	{
		//���ڵ��ʼ��
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
}

//��main.c�ﶨ���������ƿ�
extern TCB_t Task1TCB;
extern TCB_t Task2TCB;

//��������
extern TCB_t IdleTaskTCB;
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize );

/*�������������*/
void vTaskStartScheduler( void )
{
	/*======================================������������start==============================================*/     
	TCB_t *pxIdleTaskTCBBuffer = NULL;               /* ����ָ�����������ƿ� */
	StackType_t *pxIdleTaskStackBuffer = NULL;       /* ���ڿ�������ջ��ʼ��ַ */
	uint32_t ulIdleTaskStackSize;                    /* �������ÿ�������ջ��С */

	/* ��ȡ����������ڴ棺����ջ������TCB */
	vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
																 &pxIdleTaskStackBuffer, 
																 &ulIdleTaskStackSize );    

	xIdleTaskHandle = xTaskCreateStatic( (TaskFunction_t)prvIdleTask,              /* ������� */
																			 (char *)"IDLE",                           /* �������ƣ��ַ�����ʽ */
																			 (uint32_t)ulIdleTaskStackSize ,           /* ����ջ��С����λΪ�� */
																			 (void *) NULL,                            /* �����β� */
																			 (UBaseType_t) tskIDLE_PRIORITY,           /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
																			 (StackType_t *)pxIdleTaskStackBuffer,     /* ����ջ��ʼ��ַ */
																			 (TCB_t *)pxIdleTaskTCBBuffer );           /* ������ƿ� */
	/*======================================������������end================================================*/
																
	/* ���������� */
	if( xPortStartScheduler() != pdFALSE )
	{
			/* �����������ɹ����򲻻᷵�أ��������������� */
	}
}

//��������
static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
	/* ��ֹ�������ľ��� */
	( void ) pvParameters;
    
    for(;;)
    {
        /* ����������ʱʲô������ */
    }
}

//���������л�
void vTaskDelay( const TickType_t xTicksToDelay )
{
	TCB_t *pxTCB = NULL;

	/* ��ȡ��ǰ�����TCB */
	pxTCB = pxCurrentTCB;

	/* ������ʱʱ�� */
	pxTCB->xTicksToDelay = xTicksToDelay;
	
	/* ������Ӿ����б��Ƴ� */
	//uxListRemove( &( pxTCB->xStateListItem ) );      //������Ҫ�õ��������ⲿ������ʱ��������һ�»���в���
	taskRESET_READY_PRIORITY( pxTCB->uxPriority );

	/* �����л� */
	taskYIELD();
}



//�δ�ʱ���ж����������������ʱ����
void xTaskIncrementTick( void )
{
	TCB_t *pxTCB = NULL;
	BaseType_t i = 0;

	/* ����ϵͳʱ��������xTickCount��xTickCount��һ����port.c�ж����ȫ�ֱ��� */
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;


	/* ɨ������б��������̵߳�xTicksToDelay�������Ϊ0�����1 */
	for(i=0; i<configMAX_PRIORITIES; i++)
	{
		pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[i] ) );
		if(pxTCB->xTicksToDelay > 0)
		{
			pxTCB->xTicksToDelay --;
			
			/* ��ʱʱ�䵽����������� */
			if( pxTCB->xTicksToDelay ==0 )
			{
				taskRECORD_READY_PRIORITY( pxTCB->uxPriority );
			}
		}
	}

	/* �����л� */
	portYIELD();
}



/************************** ������ȼ���ѯ---�޸�ȫ�ֿ��ƿ飬ʵ�������л� *****************************/
/* �����л�����Ѱ�����ȼ���ߵľ������� */
void vTaskSwitchContext( void )
{
	/* ��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB */
    taskSELECT_HIGHEST_PRIORITY_TASK();
}



//��������ӵ������б�
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB )
{
	/* �����ٽ�� */
	taskENTER_CRITICAL();
	{
		/* ȫ�������ʱ����һ���� */
		uxCurrentNumberOfTasks++;
        
		/* ���pxCurrentTCBΪ�գ���pxCurrentTCBָ���´��������� */
		if( pxCurrentTCB == NULL )
		{
			pxCurrentTCB = pxNewTCB;

			/* ����ǵ�һ�δ�����������Ҫ��ʼ��������ص��б� */
			if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
			{
				/* ��ʼ��������ص��б� */
				prvInitialiseTaskLists();
			}
		}
		else /* ���pxCurrentTCB��Ϊ�գ��������������ȼ���pxCurrentTCBָ��������ȼ������TCB */
		{
			if( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority )
			{
				pxCurrentTCB = pxNewTCB;
			}
		}
		uxTaskNumber++;
        
		/* ��������ӵ������б� */
    prvAddTaskToReadyList( pxNewTCB );

	}
	/* �˳��ٽ�� */
	taskEXIT_CRITICAL();
}






