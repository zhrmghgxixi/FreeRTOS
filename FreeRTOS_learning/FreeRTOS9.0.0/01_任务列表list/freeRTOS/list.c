#include "list.h"

/*�ڵ��ʼ��*/
//����1 --- ĳһ���������ݽڵ�
void vListInitialiseItem(ListItem_t * const pxItem)
{
	//��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ�����
	pxItem->pvContainer = NULL;
}



/*������ڵ��ʼ��*/
//����1 --- ������ڵ�
void vListInitialise(List_t * const pxList)
{
	//����������ָ��ָ�����һ���ڵ�
	pxList->pxIndex = (ListItem_t *) &(pxList->xListEnd);

	//���������һ���ڵ�ĸ��������ֵ����Ϊ���ȷ���ýڵ������������ڵ�
	pxList->xListEnd.xItemValue = portMAX_DELAY;

	//�����һ���ڵ��pxNext��pxPreviousָ���ָ��ڵ�������ʾ������
	pxList->xListEnd.pxNext = (ListItem_t *) &(pxList->xListEnd);
	pxList->xListEnd.pxPrevious = (ListItem_t *) &(pxList->xListEnd);

	//��ʼ������ڵ��������ֵΪ0����ʾ��
	pxList->uxNumberOfItems = (UBaseType_t) 0U;
}



/*���ڵ���뵽�����β��*/
//����1 --- Ҫ�����Ŀ������(��Ŀ��������ڵ�)
//����2 --- ׼������Ŀ������ĩβ���������ݽڵ�
void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem)
{
	//pxList->pxIndexָ��ĵ�ַ��MiniListItem_t xListEnd
	//����Ĳ������Ƕ�xListEnd���е�
	ListItem_t * const pxIndex = pxList->pxIndex;

	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;

	// ��ס�ýڵ����ڵ�����
	pxNewListItem->pvContainer = (void *) pxList;

	//����ڵ������++
	pxList->uxNumberOfItems++;
}




/*���ڵ㰴���������в��뵽����*/
//����1 --- Ҫ�����Ŀ������(��Ŀ��������ڵ�)
//����2 --- ׼������Ŀ��������������ݽڵ�
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem)
{
	ListItem_t *pxIterator;

	//��ȡ�ڵ��������ֵ
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

	//Ѱ�ҽڵ�Ҫ�����λ��
	if(xValueOfInsertion == portMAX_DELAY)
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	else
	{
		for( pxIterator = (ListItem_t *) &(pxList->xListEnd);
			pxIterator->pxNext->xItemValue <= xValueOfInsertion; 
			pxIterator = pxIterator->pxNext)
		{
			/* û��������������ϵ���ֻΪ���ҵ��ڵ�Ҫ�����λ�� */			
		}
	}

	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;

	//��ס�ýڵ����ڵ�����
	pxNewListItem->pvContainer = (void *) pxList;

	//����ڵ������++
	(pxList->uxNumberOfItems)++;
}



/*���ڵ��������ɾ��*/
//����1 --- Ҫɾ�����������ݽڵ�
//��ע�⡿
//       ���ǲ���Ҫ֪��������ݽڵ����ĸ������
//       ��Ϊ������ݽڵ��ھʹ��нڵ�����������Ϣ
UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove)
{
	//��ȡ�ڵ����ڵ�����
	List_t * const pxList = (List_t *) pxItemToRemove->pvContainer;

	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

	//ȷ����������ָ����Ч���� 
	if(pxList->pxIndex == pxItemToRemove)
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}

	/* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
	pxItemToRemove->pvContainer = NULL;
	
	/* ����ڵ������-- */
	(pxList->uxNumberOfItems)--;

	/* ����������ʣ��ڵ�ĸ��� */
	return pxList->uxNumberOfItems;
}






