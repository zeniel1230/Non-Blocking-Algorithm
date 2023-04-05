#include "CLinkedList.h"

#ifdef __COARSE_GRAINED_SYNCHRONIZATION__
CLinkedList::CLinkedList()
{
	// Head�� -���Ѵ�(int �� ���� ���� ��)
	m_head.m_key = 0x80000000;
	// Tail�� +���Ѵ�(int �� ���� ū ��)
	m_tail.m_key = 0x7fffffff;
	m_head.m_next = &m_tail;
}

CLinkedList::~CLinkedList()
{

}

void CLinkedList::Init()
{
	CNode* ptr;
	while (m_head.m_next != &m_tail)
	{
		ptr = m_head.m_next;
		m_head.m_next = m_head.m_next->m_next;
		delete ptr;
	}
}

bool CLinkedList::Add(int _key)
{
	// predication(���� �� �����ϴ� ����) (�Լ������� ����)
	CNode* pred;
	CNode* curr;

	// �ȿ� �־ ������ ���� ���� ������ ����.(���� ��� ������ �ڵ�� ���� ���� ����.)
	// ����� ���� �ٸ� �����尡 �ٲ� �� ������ ����� �ּҴ� ���� �ٲ��� �ʴ´�.
	// �� ��ü�� ������� �ʴ� ���� ����� �ٲ��� �ʴ� ���� pred�� �����Ƿ� lock �ȿ� ���� ������ ����.
	pred = &m_head;
	m_lock.lock();

	curr = pred->m_next;

	// _key���� ������ ��� ����
	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	// �̹� ���� ��.
	if (_key == curr->m_key)
	{
		m_lock.unlock();

		return false;
	}
	// ���ο� ��带 ����� �߰�
	else
	{
		CNode* node = new CNode(_key);
		node->m_next = curr;
		pred->m_next = node;
		m_lock.unlock();

		return true;
	}
}

bool CLinkedList::Remove(int _key)
{
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	m_lock.lock();

	curr = pred->m_next;
	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	if (_key == curr->m_key)
	{
		pred->m_next = curr->m_next;
		delete curr;
		m_lock.unlock();
		return true;
	}
	else
	{
		m_lock.unlock();
		return false;
	}

	return 0;
}

bool CLinkedList::Contains(int _key)
{
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	m_lock.lock();

	curr = pred->m_next;
	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	if (_key == curr->m_key)
	{
		m_lock.unlock();
		return true;
	}
	else
	{
		m_lock.unlock();
		return false;
	}

	return 0;
}
#endif //__COARSE_GRAINED_SYNCHRONIZATION__

#ifdef __FINE_GRAINED_SYNCHRONIZATION__
CLinkedList::CLinkedList()
{
	m_head.m_key = 0x80000000;
	m_tail.m_key = 0x7fffffff;
	m_head.m_next = &m_tail;
}

CLinkedList::~CLinkedList()
{

}

void CLinkedList::Init()
{
	CNode* ptr;
	while (m_head.m_next != &m_tail)
	{
		ptr = m_head.m_next;
		m_head.m_next = m_head.m_next->m_next;
		delete ptr;
	}
}

bool CLinkedList::Add(int _key)
{
	// predication(���� �� �����ϴ� ����) (�Լ������� ����)
	CNode* pred;
	CNode* curr;

	// head���� �̵��� �� lock�� �ɰ� head�� ����.
	m_head.Lock();
	pred = &m_head;
	// curr�� pred next�ϰ� ���� �ɾ�д�.
	curr = pred->m_next;
	curr->Lock();

	while (curr->m_key < _key)
	{
		// while���� ���� pred�� ����ϰ� curr�� ����ִ´�.
		// ������ �̵��ؾ��ϹǷ� �ʿ� ��� ����ϴ� ���̴�.
		pred->Unlock();
		pred = curr;
		//curr�� �ٽ� ��װ� next�� �ٸ� �ְ� �ٲ��� ���ϰ� �����.
		curr = curr->m_next;
		curr->Lock();
	}

	if (_key == curr->m_key)
	{
		curr->Unlock();
		pred->Unlock();

		return false;
	}
	else
	{
		CNode* node = new CNode(_key);
		node->m_next = curr;
		pred->m_next = node;
		curr->Unlock();
		pred->Unlock();

		return true;
	}
}

bool CLinkedList::Remove(int _key)
{
	CNode* pred;
	CNode* curr;

	m_head.Lock();
	pred = &m_head;
	curr = pred->m_next;
	curr->Lock();

	while (curr->m_key < _key)
	{
		pred->Unlock();
		pred = curr;
		curr = curr->m_next;
		curr->Lock();
	}

	if (_key == curr->m_key)
	{
		pred->m_next = curr->m_next;
		curr->Unlock();
		delete curr;
		pred->Unlock();
		return true;
	}
	else
	{
		curr->Unlock();
		pred->Unlock();
		return false;
	}

	return 0;
}

bool CLinkedList::Contains(int _key)
{
	CNode* pred;
	CNode* curr;

	m_head.Lock();
	pred = &m_head;
	curr = pred->m_next;
	curr->Lock();

	while (curr->m_key < _key)
	{
		pred->Unlock();
		pred = curr;
		curr = curr->m_next;
		curr->Lock();
	}

	if (_key == curr->m_key)
	{
		curr->Unlock();
		pred->Unlock();
		return true;
	}
	else
	{
		curr->Unlock();
		pred->Unlock();
		return false;
	}

	return 0;
}
#endif //__FINE_GRAINED_SYNCHRONIZATION__

#ifdef __OPTIMISTIC_SYNCHRONIZATION__
#endif //__OPTIMISTIC_SYNCHRONIZATION__

#ifdef __LAZY_SYNCHRONIZATION__
#endif //__LAZY_SYNCHRONIZATION__

#ifdef __NONBLOCKING_SYNCHRONIZATION__
#endif //__NONBLOCKING_SYNCHRONIZATION__