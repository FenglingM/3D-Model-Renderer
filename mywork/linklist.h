#ifndef QLINKLIST_H
#define QLINKLIST_H

#define NULL 0 

template < class T > class QLinkList;  

template < class T >
class ListNode
{
	friend class QLinkList<T>;
public:
	ListNode<T>* back;  //前驱节点
	ListNode<T>* next;  //后继节点

	ListNode(const T& Data)  //每个节点的结构
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

template <class T >  //类模板，支持多种数据类型
class QLinkList
{
public:
	ListNode<T>* Head;
	ListNode<T>* Tail;
	int len;

	QLinkList(){   //构造函数对链表初始化
		Head = Tail = NULL; len = 0;
	};
	~QLinkList(){ ReMoveAll(); };

	BOOL InsertNodeHead(const T& Data) //头插法插入节点
	{
		ListNode<T>* newNode = new ListNode<T>(Data);

		if ( IsEmpty() )//判断是否为空
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

	BOOL DataExistInList(const T& Data)  //查找指定数据的节点
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

	BOOL InsertNodeTail(const T& Data)//往链表中添加新的节点，尾插法
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