#include "list.h"

//������ڵ�
struct xLIST List_Test;
//�������ݽڵ�
struct xLIST_ITEM  List_Item1;
struct xLIST_ITEM  List_Item2;
struct xLIST_ITEM  List_Item3;
struct xLIST_ITEM  List_Item4;


int main(void)
{
	/* ������ڵ��ʼ�� */
	vListInitialise( &List_Test );
	
	
	
	/* ��ʼ���������ݽڵ� */
	//�ڵ�1��ʼ��
	vListInitialiseItem( &List_Item1 );
	List_Item1.xItemValue = 1;
	//�ڵ�2��ʼ��   
	vListInitialiseItem( &List_Item2 );
	List_Item2.xItemValue = 2;
	//�ڵ�3��ʼ��
	vListInitialiseItem( &List_Item3 );
	List_Item3.xItemValue = 3;
	//�ڵ�4��ʼ��
	vListInitialiseItem( &List_Item3 );
	List_Item4.xItemValue = 4;
	
	
	
	/* ���ڵ�һ������ĩβ���� */
	//vListInsertEnd( &List_Test, &List_Item3 );    
	//vListInsertEnd( &List_Test, &List_Item2 );
	//vListInsertEnd( &List_Test, &List_Item1 ); 
	//vListInsertEnd( &List_Test, &List_Item4 ); 
	
	/* ���ڵ㰴���������в��뵽���� */
	//vListInsert( &List_Test, &List_Item3 );    
	//vListInsert( &List_Test, &List_Item2 );
	//vListInsert( &List_Test, &List_Item1 ); 
	//vListInsert( &List_Test, &List_Item4 ); 
	
	while(1);
}

