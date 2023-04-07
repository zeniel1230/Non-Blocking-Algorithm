#include "CLinkedList.h"

#ifdef __LINKEDLIST__
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
}
#endif //__FINE_GRAINED_SYNCHRONIZATION__

#ifdef __OPTIMISTIC_SYNCHRONIZATION__
CLinkedList::CLinkedList()
{
	m_head.m_key = 0x80000000;
	m_tail.m_key = 0x7FFFFFFF;
	m_head.m_next = &m_tail;

	m_freeTail.m_key = 0x7FFFFFFF;
	m_freeList = &m_freeTail;
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

	pred = &m_head;
	curr = pred->m_next;

	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	// ��ȿ�� �˻� ���� pred�� curr�� lock
	pred->Lock();
	curr->Lock();

	if (Validate(pred, curr))
	{
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

	curr->Unlock();
	pred->Unlock();

	return false;
}

bool CLinkedList::Remove(int _key)
{
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	curr = pred->m_next;

	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	pred->Lock();
	curr->Lock();

	if (Validate(pred, curr))
	{
		if (_key == curr->m_key)
		{
			pred->m_next = curr->m_next;

			// �߰�
			m_lock.lock();
			curr->m_next = m_freeList;
			m_freeList = curr;
			m_lock.unlock();

			pred->Unlock();
			curr->Unlock();
			//delete curr;

			return true;
		}
		else
		{
			pred->Unlock();
			curr->Unlock();

			return false;
		}
	}

	curr->Unlock();
	pred->Unlock();

	return false;
}

bool CLinkedList::Contains(int _key)
{
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	curr = pred->m_next;

	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	pred->Lock();
	curr->Lock();

	if (Validate(pred, curr))
	{
		curr->Unlock();
		pred->Unlock();

		return _key == curr->m_key;
	}

	curr->Unlock();
	pred->Unlock();

	return false;
}

bool CLinkedList::Validate(CNode* _pred, CNode* _curr)
{
	// ��ȿ�� �˻縦 ��� �ϴ� ���� �߿��ϴ�.
	// ���� ������ �� ���󰣴�.
	CNode* node = &m_head;

	while (node->m_key <= _pred->m_key)
	{
		// ã�Ҵ�.
		if (node == _pred)
		{
			// pred ���� ��尡 curr���� üũ
			return _pred->m_next == _curr;
		}

		node = node->m_next;
	}

	// �� �̵��� �ϴٺ��� pred�� ������ ���ߴµ� key�� pred���� ũ�ٸ� ����.
	return false;
}

/*
- ��õ���� ����ȭ�� ����
1. ���� delete�� �ϴ°�?

-�ذ�å
1. freeList�� ����� �ű⿡ �־� ���´�. (�� ��Ƶΰ� ���߿� delete�� �뵵)
2. next field�� ���� �� exception�� �߻���Ű�� �ʵ��ϸ� �ϸ� �ȴ�.
	[1] freeList�� ������ node�� MAXINT�� ���´�.
*/
void CLinkedList::Recyle_FreeList()
{
	CNode* a = m_freeList;
	while (a != &m_freeTail)
	{
		CNode* b = a->m_next;
		delete a;
		a = b;
	}

	m_freeList = &m_freeTail;
}
#endif //__OPTIMISTIC_SYNCHRONIZATION__

#ifdef __LAZY_SYNCHRONIZATION__
CLinkedList::CLinkedList()
{
#ifdef __Shared_Ptr__
	m_head = make_shared<CNode>();
	m_tail = make_shared<CNode>();
	m_head->m_key = 0x80000000;
	m_tail->m_key = 0x7FFFFFFF;
	m_head->m_next = m_tail;
#else
	m_head.m_key = 0x80000000;
	m_tail.m_key = 0x7FFFFFFF;
	m_head.m_next = &m_tail;

	m_freeTail.m_key = 0x7FFFFFFF;
	m_freeList = &m_freeTail;
#endif //__Shared_Ptr__

}

CLinkedList::~CLinkedList()
{

}

void CLinkedList::Init()
{
#ifdef __Shared_Ptr__
	m_head->m_next = m_tail;
#else
	CNode* ptr;
	while (m_head.m_next != &m_tail)
	{
		ptr = m_head.m_next;
		m_head.m_next = m_head.m_next->m_next;
		delete ptr;
	}
#endif //__Shared_Ptr__
}

bool CLinkedList::Add(int _key)
{
#ifdef __Shared_Ptr__
	shared_ptr<CNode> pred, curr;

	pred = m_head;
	curr = pred->m_next;
#else
	// predication(���� �� �����ϴ� ����) (�Լ������� ����)
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	curr = pred->m_next;
#endif //__Shared_Ptr__

	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	pred->Lock();
	curr->Lock();

	if (Validate(pred, curr))
	{
		if (_key == curr->m_key)
		{
			curr->Unlock();
			pred->Unlock();

			return false;
		}
		else
		{
#ifdef __Shared_Ptr__
			shared_ptr<CNode> node = make_shared<CNode>(_key);
#else
			CNode* node = new CNode(_key);
#endif //__Shared_Ptr__
			node->m_next = curr;
			pred->m_next = node;
			curr->Unlock();
			pred->Unlock();

			return true;
		}
	}

	curr->Unlock();
	pred->Unlock();

	return false;
}

bool CLinkedList::Remove(int _key)
{
#ifdef __Shared_Ptr__
	shared_ptr<CNode> pred, curr;

	pred = m_head;
	curr = pred->m_next;
#else
	// predication(���� �� �����ϴ� ����) (�Լ������� ����)
	CNode* pred;
	CNode* curr;

	pred = &m_head;
	curr = pred->m_next;
#endif //__Shared_Ptr__

	while (curr->m_key < _key)
	{
		pred = curr;
		curr = curr->m_next;
	}

	pred->Lock();
	curr->Lock();

	if (Validate(pred, curr))
	{
		if (_key == curr->m_key)
		{
#ifdef __Shared_Ptr__
			pred->m_next = curr->m_next;
#else
			curr->m_removed = true;
			pred->m_next = curr->m_next;

			m_lock.lock();
			curr->m_next = m_freeList;
			m_freeList = curr;
			m_lock.unlock();
#endif //__Shared_Ptr__

			pred->Unlock();
			curr->Unlock();

			return true;
		}
		else
		{
			pred->Unlock();
			curr->Unlock();

			return false;
		}
	}

	curr->Unlock();
	pred->Unlock();

	return false;
}

bool CLinkedList::Contains(int _key)
{
#ifdef __Shared_Ptr__
	shared_ptr<CNode> curr;

	curr = m_head;
#else
	CNode* curr;

	curr = &m_head;
#endif //__Shared_Ptr__

	while (curr->m_key < _key)
	{
		curr = curr->m_next;
	}

	return curr->m_key == _key && !curr->m_removed;
}

#ifdef __Shared_Ptr__
bool CLinkedList::Validate(shared_ptr<CNode> _pred, shared_ptr<CNode> _curr)
{
	return !_pred->m_removed && !_curr->m_removed && _pred->m_next == _curr;
}
#else
bool CLinkedList::Validate(CNode* _pred, CNode* _curr)
{
	return !_pred->m_removed && !_curr->m_removed && _pred->m_next == _curr;
}

void CLinkedList::Recyle_FreeList()
{
	CNode* a = m_freeList;
	while (a != &m_freeTail)
	{
		CNode* b = a->m_next;
		delete a;
		a = b;
	}

	m_freeList = &m_freeTail;
}
#endif //__Shared_Ptr__
#endif //__LAZY_SYNCHRONIZATION__

#ifdef __NONBLOCKING_SYNCHRONIZATION__
LFList::LFList()
{
	m_head.m_key = 0x80000000;
	m_tail.m_key = 0x7FFFFFFF;
	m_head.m_next.Set(&m_tail, false);
	m_freeTail.m_key = 0x7FFFFFFF;
	m_freeList = &m_freeTail;
}
LFList::~LFList() {}

void LFList::Init()
{
	LFNode* ptr;

	while (m_head.m_next.GetPtr() != &m_tail)
	{
		ptr = m_head.m_next.GetPtr();
		m_head.m_next = m_head.m_next.GetPtr()->m_next;
		delete ptr;
	}
}

void LFList::RecycleFreeList()
{
	LFNode* ptr1 = m_freeList;
	while (ptr1 != &m_freeTail)
	{
		LFNode* ptr2 = ptr1->m_next.GetPtr();
		delete ptr1;
		ptr1 = ptr2;
	}

	m_freeList = &m_freeTail;
}

void LFList::Find(int _key, LFNode* (&_pred), LFNode* (&_curr))
{
retry:
	_pred = &m_head;
	_curr = _pred->m_next.GetPtr();

	while (true)
	{
		bool removed;
		LFNode* succ = _curr->m_next.GetPtr(&removed);

		while (removed == true)
		{
			if (_pred->m_next.CAS(_curr, succ, false, false))
				goto retry;

			_curr = succ;
			succ = _curr->m_next.GetPtr(&removed);
		}

		if (_curr->m_key >= _key) return;

		_pred = _curr;
		_curr = _curr->m_next.GetPtr();
	}
}

bool LFList::Add(int _key)
{
	LFNode* pred = NULL;
	LFNode* curr = NULL;

	while (true)
	{
		Find(_key, pred, curr);

		if (curr->m_key == _key) return false;
		else
		{
			LFNode* node = new LFNode(_key);
			node->m_next.Set(curr, false);

			if (pred->m_next.CAS(curr, node, false, false)) return true;
		}
	}
}

bool LFList::Remove(int _key)
{
	LFNode* pred;
	LFNode* curr;
	bool snip;

	while (true)
	{
		Find(_key, pred, curr);

		if (curr->m_key != _key) return false;
		else
		{
			LFNode* succ = curr->m_next.GetPtr();
			snip = curr->m_next.TryMarking(succ, true);
			if (!snip) continue;
			pred->m_next.CAS(curr, succ, false, false);

			return true;
		}
	}
}

bool LFList::Contains(int _key)
{
	LFNode* curr;
	bool marked = false;
	curr = &m_head;

	while (curr->m_key < _key)
	{
		curr = curr->m_next.GetPtr();
		LFNode* succ = curr->m_next.GetPtr(&marked);
	}
	return curr->m_key == _key && !marked;
}

void LFList::Display(int _num)
{
	int c = _num;
	LFNode* p = m_head.m_next.GetPtr();

	while (p != &m_tail)
	{
		cout << p->m_key << ", ";
		p = p->m_next.GetPtr();
		c--;
		if (c == 0) break;
	}
	cout << endl;
}
#endif //__NONBLOCKING_SYNCHRONIZATION__
#endif