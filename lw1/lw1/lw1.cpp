#include <windows.h>
#include <string>
#include <iostream>

HANDLE mtx;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	int* threadNumber = static_cast<int*>(lpParam);

	WaitForSingleObject(mtx, INFINITE);
	
	std::cout << "����� �" << *threadNumber << " ��������� ���� ������." << std::endl;
	
	ReleaseMutex(mtx);
	ExitThread(0); // ������� ������������� ��� ���������� ������ � 0
}

int main()
{
	SetConsoleOutputCP(1251);

	int countOfThreads;
	std::cin >> countOfThreads;
	
	// �������� �������
	HANDLE* handles = new HANDLE[countOfThreads];
	int* threadNumbers = new int[countOfThreads];

	// ������� �������
	mtx = CreateMutex(NULL, FALSE, NULL);

	for (int i = 0; i < countOfThreads; i++)
	{
		threadNumbers[i] = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], 0, NULL);
	}

	// �������� ��������� ������ �������
	WaitForMultipleObjects(countOfThreads, handles, true, INFINITE);
	return 0;
}
