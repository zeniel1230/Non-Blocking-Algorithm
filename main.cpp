#include "stdafx.h"
#include "CLinkedList.h"

const auto NUM_TEST = 4000000;
const auto KEY_RANGE = 1000;

CLinkedList clist;
volatile int sum;

void ThreadFunc(int _numThread)
{
	int key;

	for (int i = 0; i < NUM_TEST / _numThread; i++)
	{
		switch (rand() % 3)
		{
		case 0:
			key = rand() % KEY_RANGE;
			clist.Add(key);
			break;
		case 1:
			key = rand() % KEY_RANGE;
			clist.Remove(key);
			break;
		case 2:
			key = rand() % KEY_RANGE;
			clist.Contains(key);
			break;
		default:
			cout << "Error \n";
			exit(-1);
			break;
		}
	}
}

void main()
{
#ifdef __COARSE_GRAINED_SYNCHRONIZATION__
#endif //__COARSE_GRAINED_SYNCHRONIZATION__

#ifdef __FINE_GRAINED_SYNCHRONIZATION__
#endif //__FINE_GRAINED_SYNCHRONIZATION__

#ifdef __OPTIMISTIC_SYNCHRONIZATION__
#endif //__OPTIMISTIC_SYNCHRONIZATION__

#ifdef __LAZY_SYNCHRONIZATION__
#endif //__LAZY_SYNCHRONIZATION__

#ifdef __NONBLOCKING_SYNCHRONIZATION__
#endif //__NONBLOCKING_SYNCHRONIZATION__

	for (int i = 1; i <= 16; i *= 2)
	{
		clist.Init();
		sum = 0;
		vector<thread> threads;

		auto startTime = high_resolution_clock::now();

		for (int j = 0; j < i; j++)
		{
			threads.emplace_back(ThreadFunc, i);
		}

		for (auto& th : threads)
		{
			th.join();
		}

		auto endTime = high_resolution_clock::now();
		threads.clear();
		auto execTime = endTime - startTime;

		int execMs = duration_cast<milliseconds>(execTime).count();

		clist.display20();

#ifdef __OPTIMISTIC_SYNCHRONIZATION__
		clist.Recyle_FreeList();
#endif //__OPTIMISTIC_SYNCHRONIZATION__

		cout << "Threads[ " << i << " ] , sum = " << sum;
		cout << ", execTime =" << execMs << " msecs\n";
	}

	system("pause");
}

/*
1. __COARSE_GRAINED_SYNCHRONIZATION__ : ���� ����ȭ
- ����
1) ����Ʈ�� �ϳ��� ����� ���� ������, ��� �޼��� ȣ���� �� ����� ���� Ciritical Section���� ����ȴ�.
	[1] ��� �޼���� ����� ������ �ִ� ���ȿ��� ����Ʈ�������� �����Ѵ�.
- ������
1) ������ ���� ��� ū ������尡 ���� �� ����ȭ�� ���� ���������� ������ ������ ��� ������ ���ϵȴ�.(����� ����.)
2) Blocking�̴�.

������ �����ϰ� �˾ƺ��� ���쳪 ������ �ȳ���.(��Ƽ�����忡�� ���� ����� ����.)
*/

/*
2. __FINE_GRAINED_SYNCHRONIZATION__ : ������ ����ȭ
- ����
1) ��ü ����Ʈ�� �Ѳ����� ��״� �ͺ��� ��带 ��״� ���� ���༺�� ����ų �� �ִ�.
	[1] ��ü ����Ʈ�� ���� ����� �δ� ���� �ƴ϶� ������ ��忡 ����� �д�.
	[2] Node�� Lock()�� Unlock() �޼ҵ带 �����ؾ� �Ѵ�.
	[3] Node�� next field�� ������ ��쿡�� �ݵ�� Lock()�� ���� �� �����ؾ� �Ѵ�.

- ������
1) add�� remove ������ pred, curr�� ����Ű�� ���� locking�� �Ǿ� �־�� �Ѵ�.
2) head���� node �̵��� �� �� lock�� ��׸鼭 �̵��ؾ� �Ѵ�.
	[1] ���� ��� a�� ����� Ǯ�� ���� b(a->next����)�� ����� �Ѵٸ� �� ���̿� �ٸ� �����忡 ���� b�� ���ŵ� �� �ֱ� �����̴�.
	[2] ��, �̵� �� pred�� ��� ������ �� ���� curr�� ����� ȹ���Ѵ�.

- ������
1) ������ �ʹ� ������.(���� ���� �ɱ� ������)
	-> �׷� ���� ���� �ȰŴ� ����� �����ΰ�? -> ��õ���� ����ȭ
*/

/*
3. __OPTIMISTIC_SYNCHRONIZATION__ : ��õ���� ����ȭ
- ����
1) ������ ����ȭ�� ��� ����� ȹ��� ������ �ʹ� ����� ������ �ʹ� ������.
	[1] ����Ʈ�� ������� ��� ������ ������ ��������.

- �ذ���
1) �̵� �� ����� ���� �ʴ´�.
2) ����� ȹ������ �ʰ� �˻��� ��, pred�� curr�� ��װ�, ��� ��尡 ��Ȯ���� Ȯ���Ѵ�.
	[1] �幮 �������, ���� ������ ��尡 ��� ��� ����� �����ϰ� �ٽ� �����Ѵ�.
	[2] 2���� ��带 ��׹Ƿ� deadlock�� �����ؾ��Ѵ�. (lock�� �ɰ� �����ϴ� ������ ����)

- �Ҿȿ��
1) ����� ȹ������ �ʰ� �̵��� ��� ������ ���� �̵��ϴ� ��찡 �����?
ex)
while(curr->m_key <= _key) {
	pred = curr; curr = curr->m_next;
}
-> (assembly)
mov eax, curr->m_next
	< �� ������ �ٸ� �����尡 curr->next�� ���� >
mov curr, eax

* �̵��� �� ���� ���� �ʰ� �ϴµ� �ٸ� �����尡 �����ϸ�? -> ���α׷� ���۵�
next�� �о� �ٽ� next�� �ϰ� �� ���̿� �ٸ� �����尡 �ͼ� curr�� next�� ����? ���� ���� �������� �� ���� ����.(���۵�)
delete�� ���� ���α׷����� ���� ����ǹǷ� ��� �̵����� �𸥴�.

- �ذ�å
1) ���ŵ� ��带 ���� �̵��ϴ� ���� �������.
	[1] ���� = remove, ���� = delete
2) remove�� Node�� delete���� �ʴ´�.
	[1] delete�ϸ� next�� � ������ �ٲ��� �� �� ����.
	[2] ��Ȯ���� ������ �� ������ �������� ������ �� �ִ�.(��� ���α׷��� ���� �ʴ´�.)
3) �̵� ���� ��, pred�� next�� ����� �ڸ��� ��Ҵ��� �˻��ؾ� �Ѵ�.
	[1] remove�� node�� ��ģ �̵� �̵��� �߸��� ����� �߱��� �� �ִ�.
	[2] �߸��� ����� ������? ó������ �ٽ� �ؾ��Ѵ�.
* �׷� �̷��� �͵��� ��� �����ϴ°�? -> ��ȿ�� �˻縦 ����

- Validate() : ��ȿ�� �˻�
1) �ٽ� ó������ �̵��ؼ� ���� pred, curr�� �ٽ� �� �� �ִ��� Ȯ���Ѵ�.
2) pred->next == curr���� Ȯ���Ѵ�.
*���� �˻�� ��оȰ�?
	-> pred�� curr�� locking�� �Ǿ� �����Ƿ� ����ϴ�.

- ������
1) ��õ���� ����ȭ �˰����� ��Ƹ� ���� �� �ִ�.
	[1] ������� ���ο� ��尡 �ݺ��ؼ� �߰��ǰų� ���ŵǸ� ������ ������ �� �ִ�.
2) ��� ���¸� �޴� ���� ��ġ ���� ����̹Ƿ� �����δ� �� ������ ���ɼ��� ũ��.
3) lock�� Ƚ���� ��������� ����������, ����Ʈ�� �� �� ��ȸ�ؾ� �Ѵٴ� ���� ���̴� ������尡 �ִ�.
*/

/*
4. __LAZY_SYNCHRONIZATION__

5. __NONBLOCKING_SYNCHRONIZATION__
*/