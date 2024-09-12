#include <iostream>
#include <thread>
#include "Windows.h"

void ThreadProc(int id)
{
    std::cout << "Поток №" << id << " выполянет свою работу." << std::endl;
}

int main()
{
    SetConsoleOutputCP(1251);

    std::cout << "Введите количество потоков: ";
    int countOfThreads;
    std::cin >> countOfThreads;

    for (int i = 1; i <= countOfThreads; i++)
    {
        std::thread thread(ThreadProc, i);
        thread.join();
    }

    return 0;
}
