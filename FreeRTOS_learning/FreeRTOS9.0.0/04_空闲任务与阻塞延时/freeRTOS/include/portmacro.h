#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"


/* ���������ض��� */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif


/****************************************
*  �жϿ���״̬�Ĵ�����0xe000ed04
*  Bit 28 PENDSVSET: PendSV ����λ
******************************************/
#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )

#define portSY_FULL_READ_WRITE		( 15 )

//�����л�
/********************************************************
*ISB��ָ��ͬ�����룬���ϸ�������ǰ���ָ�ִ�����֮�󣬲�ִ�к����ָ��
*DSB������ͬ�����룬��DMB�ϸ񣬽���ǰ����������ǰ�Ĵ洢�����ʲ�����ִ����Ϻ󣬲�ִ�����������ָ��(���Ǽ��κ�ָ�Ҫ�ȴ��洢�����ʲ������)
*DMB�����ݴ洢���룬DMSָ�֤����ǰ��������ǰ��Ĵ洢�����ʲ�����ִ����Ϻ󣬲��ύ(commit)��������洢�����ʲ���
*********************************************************/
#define portYIELD()																\
{																				          \
	/* ����PendSV�жϣ������������л� */							\
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;	\
	/* ��ֹ��ν����ж� */                           \
	__dsb( portSY_FULL_READ_WRITE );								\
	__isb( portSY_FULL_READ_WRITE );								\
}


/*********************************************************
*								        �ٽ籣��
**********************************************************/

#define portDISABLE_INTERRUPTS()				vPortRaiseBASEPRI()    //���ж�(����Ƕ���ж���)
#define portENABLE_INTERRUPTS()					vPortSetBASEPRI( 0 )   //���ж�

#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()

//��ע�⡿һ�㺯����/�궨���ĩβ����FROM_ISR������ʾ���������ж���
#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortRaiseBASEPRI()    //���ж�(��Ƕ�����ж���)
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortSetBASEPRI(x)    //ѡ���Կ��ж�

#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )  //��������һ������

#define portINLINE __inline

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE __forceinline
#endif

//��������ֵ�Ĺ��жϺ���(���ݲ�������)------����Ƕ�ף��������ж�����ʹ��
static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
	__asm
	{
		//ֱ�������ж�����ֵ������ulBASEPRIֵ���ж��޷�����
		msr basepri, ulBASEPRI
	}
}
//��������ֵ�Ĺ��жϺ���(����configMAX_SYSCALL_INTERRUPT_PRIORITY�궨������)------����Ƕ�ף��������ж�����ʹ��
static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}
}

//��������ֵ�Ŀ��жϺ���
static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
	__asm
	{
		/* Set BASEPRI to 0 so no interrupts are masked.  This function is only
		used to lower the mask in an interrupt, so memory barriers are not 
		used. */
		msr basepri, #0
	}
}

//������ֵ�Ĺ��жϺ���������Ƕ�ף��������ж�����ʹ��
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
	uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

	__asm
	{
		/* Set BASEPRI to the max syscall priority to effect a critical
		section. */
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
	}

	return ulReturn;
}


#endif /* PORTMACRO_H */


