#include "delay.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"



/***
*     �����delay������������OS�ģ���ΪOSʹ�õδ�ʱ��������������
* ���ǲ����޸ĵδ�����ֵ����������Ҫʹ��Ӳ��delay����˶�����иĶ���
*
* ע�⣺��ȻOS���������ò���ʼ���˵δ�ʱ��������������������������
*       ����ʱ�����ã�������ǲ��Լ��ȳ�ʼһ�εδ�ʱ������ô�ڳ�ʼ��
*       ĳ��Ӳ����Ҫdelay�������Ǿ�û��ʹ��Ӳ����delay�ˣ��������ǻ���
*       ���Լ����ֶ����õδ�ʱ����
*/



static uint16_t  g_fac_us = 0;      /* us��ʱ������ */



/**********************************************************
* @funcName ��SysTick_Init
* @brief    ���δ�ʱ����ʼ��
* @param    ��uint16_t sysclk (ʱ��Ƶ��Mhz)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void SysTick_Init(uint16_t sysclk)
{
    uint32_t reload;

    SysTick->CTRL = 0;         /* ��Systick״̬���Ա���һ�����裬������￪���жϻ�ر����ж� */
    SysTick->CTRL &= ~(1<<2);  /* ѡ��ʱ��Դ   д0���ⲿʱ��Դ   д1���ڲ�ʱ��Դ */

    g_fac_us = sysclk / 8;                      /* �����Ƿ�ʹ��OS,g_fac_us����Ҫʹ��,��Ϊ1us�Ļ���ʱ�� */

    reload = sysclk / 8;                        /* ÿ���ӵļ������� ��λΪM */
    reload *= 1000000 / configTICK_RATE_HZ;     /* ����delay_ostickspersec�趨���ʱ��
                                                * reloadΪ24λ�Ĵ���,���ֵ:16777216,��9M��,Լ��1.86s����
                                                */
    SysTick->CTRL |= 1 << 1;                    /* ����SYSTICK�ж� */
    SysTick->LOAD = reload;                     /* ÿ1/delay_ostickspersec���ж�һ�� */
    SysTick->CTRL |= 1 << 0;                    /* ����SYSTICK */
}



/**********************************************************
* @funcName ��delay_us
* @brief    ��ϵͳ�δ�ʱ��΢����ʱ
* @param    ��uint32_t us (΢��)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;     /* LOAD��ֵ---����ֵ */
    ticks = us * g_fac_us;      /* ��Ҫ�Ľ�����----Ҫ��ʱʱ�� */
    told = SysTick->VAL;        /* �ս���ʱ�ļ�����ֵ---�δ�ʱ����ǰֵ */

    while(1)
    {
        tnow = SysTick->VAL;    /* �δ�ʱ����ǰֵ */

        if(tnow != told)
        {
            if(tnow < told)
            {
                tcnt += told - tnow;    /* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����. */
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks) break;   /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�. */
        }
    }
}




/**********************************************************
* @funcName ��delay_ms
* @brief    ��ϵͳ�δ�ʱ��΢����ʱ(�޸İ�)
* @param    ��uint16_t ms (����)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void delay_ms(uint16_t ms)
{
    uint32_t i;

    for (i=0; i<ms; i++)
    {
        delay_us(1000);
    }
}



extern void xPortSysTickHandler(void);

/**********************************************************
* @funcName ��SysTick_Handler
* @brief    ��systick�жϷ�����
* @param    ��void
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void SysTick_Handler(void)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}

