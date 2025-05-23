#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

	#include "FreeRTOSConfig.h"
	#include "projdefs.h"
	#include "portable.h"
	#include "list.h"


	//������ƿ�(TCB)�ṹ��
	typedef struct tskTaskControlBlock
	{
		volatile StackType_t    *pxTopOfStack;                            //ջ��ָ��
		ListItem_t			        xStateListItem;                           //����ڵ�
		StackType_t             *pxStack;                                 //����ջ��ʼ��ַ
		char                    pcTaskName[ configMAX_TASK_NAME_LEN ];    //�������ƣ��ַ�����ʽ
	} tskTCB;
	typedef tskTCB TCB_t;


#endif /* INC_FREERTOS_H */
