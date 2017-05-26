#ifndef QLINKLIST_H
#define QLINKLIST_H

#define NULL 0 

template < class T > class QLinkList;  

template < class T >
class ListNode
{
	friend class QLinkList<T>;
public:
	ListNode<T>* back;  //ǰ���ڵ�
	ListNode<T>* next;  //��̽ڵ�

	ListNode(const T& Data)  //ÿ���ڵ�Ľṹ
	{
		data = Data;
		back = next = NULL;
	}

	T GetData() const
	{
		return data;
	}
	T data;
};

template <class T >  //��ģ�壬֧�ֶ�����������
class QLinkList
{
public:
	ListNode<T>* Head;
	ListNode<T>* Tail;
	int len;

	QLinkList(){   //���캯���������ʼ��
		Head = Tail = NULL; len = 0;
	};
	~QLinkList(){ ReMoveAll(); };

	BOOL InsertNodeHead(const T& Data) //ͷ�巨����ڵ�
	{
		ListNode<T>* newNode = new ListNode<T>(Data);

		if ( IsEmpty() )//�ж��Ƿ�Ϊ��
			Head = Tail = newNode;
		else
		{
			Head->back = newNode;
			newNode->next = Head;
			Head = newNode;
		}
		len++;
		return true;
	};

	BOOL DataExistInList(const T& Data)  //����ָ�����ݵĽڵ�
	{
		if ( IsEmpty() ) return FALSE;
		ListNode<T>* current = Head;
		while(current)
		{
			if ( current->data == Data )
				return TRUE;
			current = current->next;
		}
		if( !current ) return FALSE;
		else
			return true;
	}

	BOOL InsertNodeTail(const T& Data)//������������µĽڵ㣬β�巨
	{
		ListNode<T>* newNode = new ListNode<T>(Data);

		if ( IsEmpty() )
			Head = Tail = newNode;
		else
		{
			Tail->next = newNode;
			newNode->back = Tail;
			Tail = newNode;
		}
		len++;
		return true;
	};

	BOOL IsEmpty()
	{
		return (!Head || !Tail) ? true : false;
	};

	BOOL ReMoveAll()
	{
		len = 0;
		if ( IsEmpty() ) return true;

		ListNode<T>* current = Head;

		while(current != Tail)
		{
			ListNode<T>* temp = current;
			current = current->next;
			delete temp;
		}

		delete Tail;

		Head = Tail = NULL;

		return true;
	};
};
#endif