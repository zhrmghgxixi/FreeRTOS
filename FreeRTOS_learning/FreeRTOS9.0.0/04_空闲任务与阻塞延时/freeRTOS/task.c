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
													&xReturn,          /* ������ */ 
													pxNewTCB);         /* ������ƿ� */  
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
																			 (StackType_t *)pxIdleTaskStackBuffer,     /* ����ջ��ʼ��ַ */
																			 (TCB_t *)pxIdleTaskTCBBuffer );           /* ������ƿ� */
	/* ��������ӵ������б� */                                 
	vListInsertEnd( &( pxReadyTasksLists[0] ), &( ((TCB_t *)pxIdleTaskTCBBuffer)->xStateListItem ) );
	/*======================================������������end================================================*/
	
	/* �ֶ�ָ����һ�����е����� */
	pxCurrentTCB = &Task1TCB;
																			 
	/* ��ʼ��ϵͳʱ�������� */
  xTickCount = ( TickType_t ) 0U;
																			 

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


//�޸Ŀ��ƿ飬ʵ�������л�
void vTaskSwitchContext( void )
{    
	
	/* �����ǰ�߳��ǿ����̣߳���ô��ȥ����ִ���߳�1�����߳�2��
		 �������ǵ���ʱʱ���Ƿ����������̵߳���ʱʱ���û�е��ڣ�
		 �Ǿͷ��ؼ���ִ�п����߳� */
	
	//�����ǰ�����ǿ�������
	if( pxCurrentTCB == &IdleTaskTCB )
	{
		//�ж��ĸ������ڿ���״̬��û�еĻ��ͼ���ִ�п�������
		if(Task1TCB.xTicksToDelay == 0)
		{            
			pxCurrentTCB =&Task1TCB;
		}
		else if(Task2TCB.xTicksToDelay == 0)
		{
			pxCurrentTCB =&Task2TCB;
		}
		else
		{
			return;		/* �߳���ʱ��û�е����򷵻أ�����ִ�п����߳� */
		} 
	}
	//��ǰ�����ǿ�������
	else
	{
		/*�����ǰ�߳����߳�1�����߳�2�Ļ������������һ���߳�,���������̲߳�����ʱ�У����л������߳�
      �����ж��µ�ǰ�߳��Ƿ�Ӧ�ý�����ʱ״̬������ǵĻ������л��������̡߳�����Ͳ������κ��л� */
		
		if(pxCurrentTCB == &Task1TCB)
		{
			if(Task2TCB.xTicksToDelay == 0)
			{
				pxCurrentTCB =&Task2TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
		else if(pxCurrentTCB == &Task2TCB)
		{
			if(Task1TCB.xTicksToDelay == 0)
			{
				pxCurrentTCB =&Task1TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
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
		}
	}

	/* �����л� */
	portYIELD();
}

