#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

DWORD startTime;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	int threadNumber = *static_cast<int*>(lpParam);

	std::string fileName = (threadNumber == 1) ? "output1.txt" : "output2.txt";
	std::ofstream out(fileName, std::ios::app);

	int countOfOperations = 15;

	for (int i = 0; i < countOfOperations; i++)
	{
		for (int j = 0; j < 10'000; j++)
		{
			for (int k = 0; k < 10'000; k++)
			{

			}
		}

		DWORD currentTime = timeGetTime();

		out << threadNumber << "|" << currentTime - startTime << std::endl;
		//if (threadNumber == 1)
		//{
		//	out1 << threadNumber << "|" << currentTime << std::endl;
		//}
		//else
		//{
		//	out2 << threadNumber << "|" << currentTime << std::endl;
		//}
	}

	ExitThread(0); // функция устанавливает код завершения потока в 0
}

int main()
{
	SetConsoleOutputCP(1251);

	int countOfThreads = 2;

	// создание потоков
	HANDLE* handles = new HANDLE[countOfThreads];
	int* threadNumbers = new int[countOfThreads];


	std::cout << "Press Enter to continue..." << std::endl;
	std::cin.get();

	startTime = timeGetTime();

	for (int i = 0; i < countOfThreads; i++)
	{
		threadNumbers[i] = i + 1;
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], 0, NULL);
	}

	if (!SetThreadPriority(handles[0], THREAD_PRIORITY_HIGHEST)) {
		std::cerr << "Error setting thread priority" << std::endl;
		return 1;
	}

	if (!SetThreadPriority(handles[1], THREAD_PRIORITY_NORMAL)) {
		std::cerr << "Error setting thread priority" << std::endl;
		return 1;
	}

	// ожидание окончания работы потоков
	WaitForMultipleObjects(countOfThreads, handles, true, INFINITE);

	for (int i = 0; i < countOfThreads; ++i)
	{
		CloseHandle(handles[i]);
	}

	return 0;
}
