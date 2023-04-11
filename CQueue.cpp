#include "CQueue.h"

#ifdef __QUEUE__
#ifdef __COARSE_GRAINED_SYNCHRONIZATION__
CQueue::CQueue()
{
	head = tail = new CNode(0);
}
CQueue::~CQueue() {}

void CQueue::Init()
{
	// �˰��� ����ȭ�� ���� ���ʳ�� �ϳ� �α�
	CNode* ptr;
	while (head->next != nullptr) {
		ptr = head->next;
		head->next = head->next->next;
		delete ptr;
	}
	tail = head;
}

void CQueue::Enq(int key)
{
	CNode* e = new CNode(key);
	glock.lock();
	tail->next = e;
	tail = e;
	glock.unlock();
}

int CQueue::Deq()
{
	glock.lock();
	if (nullptr == head->next) {
		cout << "QUEUE EMPTY!!\n";
		while (true);
	}
	int result = head->next->key;
	CNode* temp = head;
	head = head->next;
	glock.unlock();
	delete temp;
	return result;
}

void CQueue::Display20()
{
	int c = 20;
	CNode* p = head->next;
	while (p != nullptr)
	{
		cout << p->key << ", ";
		p = p->next;
		c--;
		if (c == 0) break;
	}
	cout << endl;
}
#endif
#ifdef __LOCK_FREE__
CLockFreeQueue::CLockFreeQueue()
{
	m_head = m_tail = new CNode(0);
}

CLockFreeQueue::~CLockFreeQueue()
{

}

void CLockFreeQueue::Init()
{
	CNode* ptr;
	while (m_head->m_next != nullptr)
	{
		ptr = m_head->m_next;
		m_head->m_next = m_head->m_next->m_next;
		delete ptr;
	}
	m_tail = m_head;
}

bool CLockFreeQueue::CAS(CNode* volatile* _addr, CNode* _oldNode, CNode* _newNode)
{
	return atomic_compare_exchange_strong(
		reinterpret_cast<volatile atomic_int*>(_addr),
		reinterpret_cast<int*>(&_oldNode),
		reinterpret_cast<int>(_newNode));
}

/*
- Enqueue�� �⺻ ����
1) Tail�� ����Ű�� node�� CAS�� �� ��带 �߰�
2) �����ϸ� Next�� �̵��� �� �߰�
3) �����ϸ� Tail�� �̵�
*/
void CLockFreeQueue::Enq(int _key)
{
	/*
	�������� ����
	�����ϸ� tail�� �о�ְ� ����, �����ϸ� ó������ �ٽ�

	CNode* temp = new CNode(_key);
	while (true)
	{
		CNode* last = m_tail;

		// tail�� ����Ű�� ���� next�� NULL�̸� temp�� ����.
		if (CAS(&last->m_next, NULL, temp))
		{
			// ����
			// tail�� �̵�(�о�ֱ�)
			//m_tail = temp;

			// ������ ������ ������ �߻��Ѵ�.
			// �ٸ� �����尡 �ͼ� ���� ������ ���� ��� �׾Ƴ��Ҵ�. �׷� ��� �����ϴٰ� �ڴʰ� �����?
			// �� �ڷ� ���� tail�� ������ �� ���ܿ��� �ȴ�. �� ��Ȳ���� deq�� �ż� ������ ���ư��� ���̴�.
			CAS(&m_tail, last, temp)
			// �̷��� �ϸ�? �� ū ������ �߻��Ѵ�.
			// ������ ù��° CAS�� �����ϰ� �ι�° CAS �����߿� �ٸ� �����尡 enq�Ϸ��� CAS�� �ϰԵǸ� �� CAS�� ������ �����Ѵ�.(tail�� next�� null�� �ƴϹǷ�)
			// �̰� ����ŷ ���α׷��� �ƴϴ�.
			// ���� �� �������� 2��° CAS�� �����ɶ����� �ڿ� �� ������� ��� ��ٷ����Ѵ�.
			return;
		}
		// ����(���� ���� ��ġ���ؼ� ��������)
		// ó������ �ٽ�
	}
	*/

	/*
	���� �ذ�å -> tail�� ������ ��� �����忡�� �����ϰ� �Ѵ�.
	linkedList���� CAS �� �� �Ҷ� �ڷᱸ���� �̻�������. �̻��� �ڷᱸ���� ����ϰ� �߾���. �� ���� ����������.
	tail ��� ���� �ٿ��µ� tail �����Ͱ� �������� ���������� �˰����� ���������� �����ϵ��� ������
	�����ϸ� ����, �����ϸ� ��� �������� �ƴ϶� �׳� �Ѿ�� �ٸ� �����尡 tail�� ������Ű��.
	CAS(&m_tail, last, next);
	�� �������, �׸��� CNode* last = m_tail�� ���ư���.
	*/
	CNode* temp = new CNode(_key);
	while (true)
	{
		// tail�� last�� ������ ��� ���ε� �� ��尡 ��� �����ϰ� ��������� ������ ����.
		// ����Ǹ� ���ư����� �� �ִ�. �׷��Ƿ� next���� �̸� �о�д�.
		// last�� lastNext�� �̸� �о�ΰ� �� next�� ����� �� next���� Ȯ���Ѵ�. ���� ���� tail ������ ������� �ʾҴٸ�
		// next�� ����� ���� ���̰� �ƴϸ� �� ���̿� �� tail�� �ٲ���ٴ� ���� �浹�� ���̴ϱ� ó������ �ٽ� �Ѵ�.
		CNode* last = m_tail;
		CNode* next = last->m_next;

		if (last != m_tail) continue;
		// ���� next�� nullptr�̴�. null�� �ƴϸ� �ٸ� �����尡 ������ tail�� ����־��� ������ ���� �ȵ� ���̴�.
		if (next != nullptr)
		{
			// tail�� ��� ������������Ѵ�.
			CAS(&m_tail, last, next);
			continue;
		}

		// next�� nullptr�̴ϱ� �ٸ� �����尡 ������� �ʾ����Ƿ� next�� �ٲٰ�
		if (CAS(&last->m_next, nullptr, temp) == false) continue;

		// ������Ų��.(�����ϵ� ����)
		CAS(&m_tail, last, temp);

		return;
	}
}

int CLockFreeQueue::Deq()
{
	/*
	// 1�� ����
	// deq�� head���� ������ ���̴�. ���� �����ϰ� head�� �����ؾ��Ѵ�.
	// �翬�� ������ CAS�� �ؾ��Ѵ�.
	while(true)
	{
		CNode* first = m_head;
		if (!CAS(&m_head, first, first->m_next)) continue;
		// �����ϸ� ���� �о� ����
		int value = first->m_next->m_key;
		delete first;
		return value;
	}
	// �̷��� �о �Ǵ°�? �����ϴ�.
	// first�� ���� �� first->next�� 'a'�ε� �ٸ� �����尡 first�� ���� Ȯ����? ����. first�� ���� �����ִ�.
	// �׷��� first�� �� ���������� �� next�� �׳� ������ �ǳ�? �װ� �ȵȴ�.
	// ����� �Ǿ��ִ� ���� �ٸ� �����尡 �ǵ�°� �ƴ����� ������ 'a'�̴�. ���� �ٸ� �����尡 delete�� ���� ���� �� �ִ�.
	*/

	/*
	// 2�� ����
	while (true)
	{
		CNode* first = m_head;
		// next�� �߰��ߴ�.
		// first�� �ٲ���� �ȹٲ���� head�� ������ ����, �ٲ���ٸ� next�� ����ε� ������ Ȯ���� �� ����.
		// next�� ����� �� ������ �ƴ�����, head�� first ���� �����ϰ� �ֳ� �ƴѰ��� ���� �ȴ�.
		// �����ϰ� �ִٸ� ����� �� ���� �����ǰ� �ִ� �����̹Ƿ� �̶� value�� �д´�.
		CNode* next = first->m_next;
		if (first != m_head) continue;
		// ť�� Empty�� ���� Ȯ���ؾ��Ѵ�. first �� head�� next�� null�̶�� ����ִ� ť��.(���� ��常 �ִ�.) �׷��� �׳� ������.
		// ���� ť�̹Ƿ� �ش� ���� ���α׷��� �״��� �����ڵ带 ������ �ؾ��Ѵ�.
		if (first->m_next == NULL) return EmptyError();
		int value = next->m_key;
		if (CAS(&m_head, first, next)) continue;
		delete first;
		return value;
	}
	
	���α׷��� �� ������ ��� ���� �̸� �����ϰ� �� ���� �ٲ���� �����˻簡 �־ �׷���. 
	�ٽ��� �����̴�. value�� �а� CAS�ϴ� ��. �����ϸ� ����, �����ϸ� �����̴�.
	�ٵ� �̰� ���� �ƴϴ�. ������ ������ �ϳ� �����Ѵ�. ���� ���̽��� �ִ�.
	- int value = first->next->key
		�ٸ� �����忡�� first�� ���� free ��Ű�� � ���� �������� �� �� ����.
		Segment Fault�� ���ɼ��� �ִ�.
	- Tail�� Head�� ������ ���
		Tail�� enq�ϴ� ���� deq�� �Ͼ tail�� ������ ���� ��带 ����Ű�� ������ �����.
	enq�� deq�� ���� ��� ����� ����. ������ ť�� ������� ��� enq, deq�� ���ÿ� ȣ��Ǹ� ���� ���δ�. �� ��쵵 �˻��ؾ��Ѵ�.
	deq�� ��� ���� CAS �ѹ����� ������. ������ enq�� CAS�� 2���ϰ� ù��° CAS�� �����ϰ� �ι�° CAS�� ���� �ʰ� �ִ� �� ª�� ������ 
	ť�� �̻��ϰ� ���̴� ������ �����Ѵ�.

	(���ҽ� ���� �ٿ�ε�.png ����)
	Tail�� ���ʳ�带 ����Ű���ִٸ� ť�� ����ִ� ���´�. �׶� �ٸ� �����尡 �ͼ� enq�� �Ѵ�.
	enq�� �ϸ� �� ��带 ���� ��忡 �����ϰ� tail�� ������Ű�°� ������. 
	������ ������ enq�ϴ� �����尡 ���� ��忡 ���ο� ��� b�� �׸��� ���� ������ �س���.
	������� OK���� tail�� ������Ű�� ���ϰ� ���� �� �ٸ� �����尡 deq�� �ϴ� ���̴�. deq�� head�� ����.
	head�� next�� a�� ����Ű�� �ְ� first�� ���� head�� �ְ�n ext�� a�� ������ nullptr�� �ƴϴ�.
	�׷��� ����Ǿ�������(ť�� �����Ͱ� ������) deq�� �Ѵ�. deq���� CAS�� �Ѵ�. head�� ������Ų��. ���� tail ���� delete �ϰ� �ȴ�.
	A�� first�̰� head�� ���� �������� ���������� deq�� �ϰ� a�� �����Ѵ�.�׷� tail�� delete�� ���� ����Ű�� �ȴ�.
	���� b�� ����Ű�� ���ϰ� ������ ���ϰ� �ִµ� tail�� ����Ű�� ��尡 ���󰡹��ȴ�. �׷� �ٸ� �����尡 �ͼ� enq�� �� ��
	null�̴ϱ� �����ؼ� �ٿ����ϴµ� delete�Ǿ� �� ������ �� ���� ���� ���α׷��� ������.

	�̷��� tail�� ���� �������� ���ߴµ� deq �ϸ� �ȵȴ�.head�ϰ� tail�� ���� ��� deq ���� �ʴ� �͵� ���� �ȵȴ�.
	���� ���� ����� ���� ������ ���� enq�Ǿ� ���� ���� �ִ�. �׷��� b �����͸� ����������ϴµ� ��� �ϳ�?
	head�� tail�� ������ empty�� �ƴϴ�? �׷� tail�� ������Ű�� ���ָ�ȴ�. �׷��� enq���� tail�� ������ �������ִ� �ڵ尡 �ʿ��ϴ�.
	*/

	//�װ��� �� �ڵ��.
	while (true)
	{
		//head�� tail ���� �о� �д�.
		CNode* first = m_head;
		CNode* firstNext = first->m_next;
		CNode* last = m_tail;
		CNode* lastNext = last->m_next;
		if (first != m_head) continue;
		// first�� last�� ����.
		if (last == first)
		{
			// last�� next�� nullptr�̸� �� ��¿�� ����. ���� ����ִ� ���̴�.
			if (lastNext == nullptr)
			{
				return EmptyError();
			}
			// �ٵ� nullptr�� �ƴϴ�? �� ���� �Դµ� tail�� �������� ���� ����.
			else
			{
				// tail�� ���������ش�.
				CAS(&m_tail, last, lastNext);
				// �׸��� ó������ �ٽ�
				// �״�� �����Ϸ��ϸ� ���� ���� ��츦 �������Ѵ�. ���� ó������ �ٽ� �ϴ°� �����ϴ�.
				continue;
			}
		}
		if (firstNext == nullptr) continue;
		int result = firstNext->m_key;
		if (CAS(&m_head, first, firstNext) == false) continue;
		first->m_next = nullptr;
		// �������� �� ������ ����(ABA �������� �ٷ� ��)
		//delete first;
		return result;
	}
	// �̷��� ���� tail�� delete�� ��带 ����Ű�� ��찡 ������ �ʴ´�. �̰��� ������ �����̴�.
	// ���α׷��� �����ϵ� ���� ��� ��쿡�� ������ �ʰ� �����Ѵ�.
	// �׸��� �������� �ִ�. (�����Ϸ� ����ȭ ����)
	// head�� �а� head�� ���Ѵ�.
	// �̱۾����忡���� �ǹ̰� ����. ������ ��Ƽ�����忡���� �ƴϴ�. ū �ǹ̰� �ִ�.
	// �����Ϸ��� ������ �� �д´ٰ� ������ false�Ѵ�. ������ ���ٰ� �����ϰ� �������� �õ������� �ʴ´�.
	// �׷��� ��Ƽ�����忡�� ���ư��� �ϱ� ���� head�� volatile�� �������ش�.

	//CNode* volatile m_head;
	//CNode* volatile m_tail;

	//�׸��� volatile�� ������ �߿��ϴ�. head�� tail�� ����Ű�� ��尡 volatile�� �ƴ϶� �ּ� ��ü�� volatile�� ��ƾ� �Ѵ�.
}

int CLockFreeQueue::EmptyError()
{
	cout << "EMPTY \n";
	this_thread::sleep_for(1ms);
	return -1;
}

void CLockFreeQueue::Display20()
{
	int c = 20;
	CNode* p = m_head->m_next;
	while (p != nullptr)
	{
		cout << p->m_key << ", ";
		p = p->m_next;
		c--;
		if (c == 0) break;
	}
	cout << endl;
}
#endif
#endif