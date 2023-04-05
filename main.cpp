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

3. __OPTIMISTIC_SYNCHRONIZATION__ : ��õ���� ����ȭ

4. __LAZY_SYNCHRONIZATION__

5. __NONBLOCKING_SYNCHRONIZATION__
*/