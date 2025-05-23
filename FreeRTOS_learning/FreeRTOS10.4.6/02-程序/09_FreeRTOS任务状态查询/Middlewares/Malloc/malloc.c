#include "malloc.h"



/**
******************************************************************************
* @file      ��.\Middlewares\Malloc\malloc.c
*              .\Middlewares\Malloc\malloc.h
* @author    ��XRbin
* @version   ��V1.0
* @date      ��2023-12-16
* @brief     ���ڴ�������
******************************************************************************
* @attention
*   �ҵ�GitHub   ��https://github.com/XR-bin
*   �ҵ�gitee    ��https://gitee.com/xrbin
*   �ҵ�leetcode ��https://leetcode.cn/u/xrbin/
******************************************************************************
*/



#if !(__ARMCC_VERSION >= 6010050)   /* ����AC6����������ʹ��AC5������ʱ */

    /* �ڴ��(64�ֽڶ���) */
    static __align(64) uint8_t mem1base[MEM1_MAX_SIZE];                                     /* �ڲ�SRAM�ڴ�� */
    static __align(64) uint8_t mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));     /* ����SRAM�ڴ�� */
    /* �ڴ����� */
    static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                  /* �ڲ�SRAM�ڴ��MAP */
    static MT_TYPE mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000 + MEM2_MAX_SIZE)));  /* ����SRAM�ڴ��MAP */

#else   /* ��AC6��������ʹ��AC6������ʱ */

    /* mem2base�ĵ�ַ��0x68000000��AC6��֧��at�궨����ʽ���޸�Ϊ���µĺ궨����ʽ�� */
    /* �ڴ��(64�ֽڶ���) */
    static __ALIGNED(64) uint8_t mem1base[MEM1_MAX_SIZE];                                                       /* �ڲ�SRAM�ڴ�� */
    static __ALIGNED(64) uint8_t mem2base[MEM2_MAX_SIZE] __attribute__((section(".bss.ARM.__at_0X68000000")));  /* ����SRAM�ڴ�� */
    /* �ڴ����� */
    static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                          /* �ڲ�SRAM�ڴ��MAP */
    static MT_TYPE mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((section(".bss.ARM.__at_0X680F0C00")));     /* ����SRAM�ڴ��MAP */

#endif



/* �ڴ������� */
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE};   /* �ڴ���С(�����ƶ����ڴ��) */
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE};               /* �ڴ�ֿ��С(һ���ڴ��ռ�����ֽ��ڴ�ռ�) */
const uint32_t memsize[SRAMBANK]    = {MEM1_MAX_SIZE, MEM2_MAX_SIZE};                   /* �ڴ�ش�С(�����Է�����ڴ�ռ��С) */



/* �ڴ��������� */
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                    /* �ڴ��ʼ�� */
    my_mem_perused,                 /* �ڴ�ʹ���� */
    mem1base, mem2base,             /* �ڴ�� */
    mem1mapbase, mem2mapbase,       /* �ڴ����״̬�� */
    0, 0,                           /* �ڴ����δ���� */
};



/**********************************************************
* @funcName ��my_memcpy
* @brief    �������ڴ����������(��һ���ڴ�ռ��������ݵ���һ�ڴ�ռ���)
* @param    ��void *des   (Ŀ�ĵ�ַ)
* @param    ��void *src   (Դ��ַ)
* @param    ��uint32_t n  (��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ))
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void my_memcpy(void *des, void *src, uint32_t n)
{  
    /* һ�����ǲ����Ҫ�����Ĳ���ָ����в��� */
    /* ����ͨ��һ������ָ����Ϊ�н飬������Ϊ�˳��ڰ�ȫ��֤ */
    uint8_t *xdes = des;
    uint8_t *xsrc = src;

    while(n--) *xdes++ = *xsrc++;
}



/**********************************************************
* @funcName ��my_memset
* @brief    �������ڴ�(�����ڴ�ռ��ֵ��һ�������Կռ���0)
* @param    ��void *s         (�ڴ��׵�ַ)
* @param    ��uint8_t c       (Ҫ���õ�ֵ)
* @param    ��uint32_t count  (��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ))
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void my_memset(void *s, uint8_t c, uint32_t count)
{
    /* һ�����ǲ����Ҫ�����Ĳ���ָ����в��� */
    /* ����ͨ��һ������ָ����Ϊ�н飬������Ϊ�˳��ڰ�ȫ��֤ */
    uint8_t *xs = s;

    while(count--) *xs ++= c;
}



/**********************************************************
* @funcName ��my_mem_init
* @brief    ���ڴ�����ʼ��
* @param    ��uint8_t memx (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @retval   ��void
* @details  ��
* @fn       ����ʵ��ν�ĳ�ʼ�����ǰ��ڴ�غ��ڴ��(���ǵı��ʾ�������)��0
************************************************************/
void my_mem_init(uint8_t memx)
{
    uint8_t mttsize = sizeof(MT_TYPE);                               /* ��ȡmemmap��������ͳ���(16bit /32bit)*/
    my_memset(mallco_dev.memmap[memx], 0, memtblsize[memx]*mttsize); /* �ڴ�״̬���������� */
    mallco_dev.memrdy[memx] = 1;                                     /* �ڴ�����ʼ��OK ���ڴ�غ��ڴ����0�� */
}



/**********************************************************
* @funcName ��my_mem_perused
* @brief    ����ȡ�ڴ�ʹ����
* @param    ��uint8_t memx (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @retval   ��uint16_t --- (ʹ���ʣ�������10��,0~1000,����0.0%~100.0%)
* @details  ��
* @fn       ��
*                �Ƿ�ռ����ͨ���ж�mem1mapbase��mem2mapbase�������
*            Ա�Ƿ��0�������0��ռ�ã�֮�������Աֵ��һ�����壬��
*            ��ռ�˶��ٿ飬��ֵΪ10�����ʾ������������10���ڴ��
************************************************************/
uint16_t my_mem_perused(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;

    /* memtblsize���ڴ���С(һ���ڴ����) */
    /* �����ڴ������ */
    for(i=0;i<memtblsize[memx];i++)
    {
        /* mallco_dev.memmap[memx]���ڴ������ */
        /* ȡ��ÿ����Ա�ж��Ƿ��0 */
        /* ��0�������� */
        if(mallco_dev.memmap[memx][i])used++;
    }

    /* ʹ������/��������*100 = ʹ���� */
    return (used*100)/(memtblsize[memx]);
}



/**********************************************************
* @funcName ��my_mem_malloc
* @brief    ���ڴ����(�ڲ�����)------ȷ�����ڴ�ص�ƫ����
* @param    ��uint8_t memx  (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @param    ��uint32_t size (Ҫ������ڴ��С(�ֽ�))
* @retval   ��uint32_t --- (0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ)
* @details  ��
* @fn       ��ע�⣺�ڴ��ı����ǴӺ���ǰ��
************************************************************/
static uint32_t my_mem_malloc(uint8_t memx, uint32_t size)
{
    signed long offset=0;  /* ƫ�������� */
    uint32_t nmemb;        /* ��Ҫ���ڴ���� */
    uint32_t cmemb=0;      /* �������ڴ��������֤����������ڴ���������� */
    uint32_t i;

    /* δ��ʼ��,��ִ�г�ʼ�� */
    if(!mallco_dev.memrdy[memx]) mallco_dev.init(memx);

    /* ����Ҫ���� */
    if(size==0) return 0XFFFFFFFF;

    /* �ڴ���� = ����ռ��С(�ֽڵ�λ) / tһ���ڴ���С(�ֽڵ�λ) */
    nmemb=size/memblksize[memx];   /* ��ȡ��Ҫ����������ڴ���� */

    /* ����ռ��С(�ֽڵ�λ) / tһ���ڴ���С(�ֽڵ�λ) ��= 0 */
    /* �����0��Ҫ������һ���ڴ�� */
    if(size%memblksize[memx]) nmemb++;

    /* �ڴ��ı����ǴӺ���ǰ�� */
    for(offset=memtblsize[memx]-1;offset>=0;offset--)  /* ���������ڴ������ */
    {
        /* �жϸ��ڴ���Ƿ�ռ���� */ 
        if(!mallco_dev.memmap[memx][offset])
        {
            cmemb++;            /* �������ڴ�������� */
        }
        /* ��֤�ڴ��������� */
        else
        {
            cmemb=0;            /* �����ڴ������ */
        }

        /* ȷ���������ڴ��λ�ú� */
        if(cmemb==nmemb)            /* �ҵ�������nmemb�����ڴ�� */
        {
            /* ��ע�ڴ��ǿ� */
            for(i=0;i<nmemb;i++)
            {
                /* ��ʼ���ڴ�����ڴ�������λ�ñ�Ǹ��ڴ�鱻ռ�� */
                mallco_dev.memmap[memx][offset+i]=nmemb;
            }

            /* ȷ������ռ����ڴ������λ�� */
            /* ���ڴ������λ��*һ���ڴ���С(32�ֽ�) */
            return (offset*memblksize[memx]);   /* ����ƫ�Ƶ�ַ */
        }
    }

    return 0XFFFFFFFF;/* δ�ҵ����Ϸ����������ڴ�� */
}



/**********************************************************
* @funcName ��my_mem_free
* @brief    ���ͷ��ڴ�(�ڲ�����)------�ڴ��ƫ�����������ռ����ڴ���ռ�ñ�־
* @param    ��uint8_t memx    (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @param    ��uint32_t offset (�ڴ��ַƫ��(�ֽ�),Ҳ�������ڴ�������λ��)
* @retval   ��uint8_t --- (0,�ͷųɹ�; 1,�ͷ�ʧ��; 2,��������(ʧ��);)
* @details  ��
* @fn       ��
************************************************************/
static uint8_t my_mem_free(uint8_t memx,uint32_t offset)
{
    int i;
    int index;
    int nmemb;

    /* �ж��Ƿ��ʼ�� */
    if(!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);
        return 1;
    }

    /* �ж����ƫ�����Ƿ񳬳����ڴ�صĴ�С */
    if(offset<memsize[memx])   /* ƫ�����ڴ����. */
    {
        /* �ڴ��ƫ���� = �ڴ��ƫ����/һ���ڴ���С */
        index = offset / memblksize[memx];             /* ƫ�������ڴ����� */
        /* �ڴ�������Ա��ֵ��������Ŀ��� */
        nmemb = mallco_dev.memmap[memx][index];        /* �ڴ������ */

        for(i=0;i<nmemb;i++)                           /* �ڴ������ */
        {
            /* �������ռ����ڴ��ı�� */
            mallco_dev.memmap[memx][index+i] = 0;
        }
        return 0;
    }
    else
    {
        return 2;   /* ƫ�Ƴ�����. */
    }
}



/**********************************************************
* @funcName ��mymalloc
* @brief    �������ڴ�(�ⲿ����)
* @param    ��uint8_t memx  (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @param    ��uint32_t size (Ҫ������ڴ��С(�ֽ�))
* @retval   ��void* --- (���䵽���ڴ��׵�ַ.)
* @details  ��
* @fn       ��
************************************************************/
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;    /* ���ڴ�������ƫ�������� */

    /* ��ȡ���ڴ�������ƫ���� */
    offset=my_mem_malloc(memx,size);

    /* �����������򷵻ؿյ�ַ */
    if(offset==0XFFFFFFFF)
    {
        return NULL;
    }
    /* �������ɹ����򷵻�����ռ��׵�ַ */
    else
    {
        return (void*)((uint32_t)mallco_dev.membase[memx]+offset);
    }
}



/**********************************************************
* @funcName ��myfree
* @brief    ���ͷ��ڴ�(�ⲿ����)
* @param    ��uint8_t memx  (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @param    ��void *ptr (Ҫ�ͷŵ��ڴ�ռ��׵�ַ)
* @retval   ��void
* @details  ��
* @fn       ��
************************************************************/
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;

    if(ptr==NULL) return;    /* ��ַΪ0. */

    /* ȷ������ռ���ڴ��ƫ���� */
    offset=(uint32_t)ptr-(uint32_t)mallco_dev.membase[memx];

    /* �ͷ��ڴ�� */
    my_mem_free(memx,offset);     /* �ͷ��ڴ� */
}



/**********************************************************
* @funcName ��myrealloc
* @brief    �����·����ڴ�(�ⲿ����)
* @param    ��uint8_t memx  (�����ڴ�飬�����ڲ�SRAM�����ⲿSRAM���ڴ��)
* @param    ��void *ptr     (Ҫ�ͷŵ��ڴ�ռ��׵�ַ)
* @param    ��uint32_t size (Ҫ������ڴ��С(�ֽ�))
* @retval   ��void* --- (�·��䵽���ڴ��׵�ַ.)
* @details  ��
* @fn       ��
************************************************************/
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)  
{  
    uint32_t offset; 

    /* ����һ���µĿռ� */
    offset=my_mem_malloc(memx,size);

    if(offset==0XFFFFFFFF)     /* ������� */
    {
        return NULL;           /* ���ؿ�(0) */
    }
    else                       /* ����û����, �����׵�ַ */
    {
        /* �Ѿɿռ�����ݸ��Ƶ��¿ռ��� */
        my_memcpy((void*)((uint32_t)mallco_dev.membase[memx]+offset), ptr, size);  /* �������ڴ����ݵ����ڴ� */
        /* ɾ���ɿռ� */
        myfree(memx,ptr);        /* �ͷž��ڴ� */
        /* �����¿ռ��ַ */
        return (void*)((uint32_t)mallco_dev.membase[memx]+offset);   /* �������ڴ��׵�ַ */
    }
}

