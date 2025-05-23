#include "FreeRTOS.h"
#include "task.h"

// TCB ����������ƿ��Ӣ����д


/********************************ȫ�ֱ���********************************/

/* ��ǰ�������е������������ƿ�ָ�룬Ĭ�ϳ�ʼ��ΪNULL */
TCB_t * volatile pxCurrentTCB = NULL;

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
/*�������������*/
void vTaskStartScheduler( void )
{
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}

//�޸Ŀ��ƿ飬ʵ�������л�
void vTaskSwitchContext( void )
{    
    /* �������������л� */
    if( pxCurrentTCB == &Task1TCB )
    {
        pxCurrentTCB = &Task2TCB;
    }
    else
    {
        pxCurrentTCB = &Task1TCB;
    }
}



