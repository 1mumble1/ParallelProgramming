#include <windows.h>
#include <string>
#include <iostream>
#include "tchar.h"
#include <fstream>

//CRITICAL_SECTION FileLockingCriticalSection;
HANDLE FileMutex;

int ReadFromFile() {
    WaitForSingleObject(FileMutex, INFINITE);
    //EnterCriticalSection(&FileLockingCriticalSection);
    std::fstream myfile("balance.txt", std::ios_base::in);
    int result;
    myfile >> result;
    myfile.close();
    //LeaveCriticalSection(&FileLockingCriticalSection);
    ReleaseMutex(FileMutex);

    return result;
}

void WriteToFile(int data) {

    WaitForSingleObject(FileMutex, INFINITE);
    //EnterCriticalSection(&FileLockingCriticalSection);
    std::fstream myfile("balance.txt", std::ios_base::out);
    myfile << data << std::endl;
    myfile.close();
    //LeaveCriticalSection(&FileLockingCriticalSection);
    ReleaseMutex(FileMutex);
}

int GetBalance() {
    int balance = ReadFromFile();
    return balance;
}

void Deposit(int money) {
    WaitForSingleObject(FileMutex, INFINITE);
    //EnterCriticalSection(&FileLockingCriticalSection);
    int balance = GetBalance();
    balance += money;

    WriteToFile(balance);

    //LeaveCriticalSection(&FileLockingCriticalSection);
    ReleaseMutex(FileMutex);

    printf("Balance after deposit: %d\n", balance);
}

void Withdraw(int money) {
    WaitForSingleObject(FileMutex, INFINITE);
    //EnterCriticalSection(&FileLockingCriticalSection);
    int balance = GetBalance();
    if (balance < money) {
        //LeaveCriticalSection(&FileLockingCriticalSection);
        ReleaseMutex(FileMutex);
        printf("Cannot withdraw money, balance lower than %d\n", money);
        return;
    }

    Sleep(20);
    balance -= money;
    WriteToFile(balance);
    //LeaveCriticalSection(&FileLockingCriticalSection);
    ReleaseMutex(FileMutex);

    printf("Balance after withdraw: %d\n", balance);
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter) {
    Deposit((int)lpParameter);
    ExitThread(0);
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter) {
    Withdraw((int)lpParameter);
    ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[]) {
    HANDLE* handles = new HANDLE[49];

    //InitializeCriticalSection(&FileLockingCriticalSection);
    FileMutex = CreateMutex(NULL, FALSE, L"FileMutex");
    if (FileMutex == NULL) {
        return -1;
    }

    WriteToFile(0);

    SetProcessAffinityMask(GetCurrentProcess(), 1);
    for (int i = 0; i < 50; i++) {
        handles[i] = (i % 2 == 0)
            ? CreateThread(NULL, 0, &DoDeposit, (LPVOID)230, CREATE_SUSPENDED, NULL)
            : CreateThread(NULL, 0, &DoWithdraw, (LPVOID)1000, CREATE_SUSPENDED, NULL);
        ResumeThread(handles[i]);
    }

    // ожидание окончания работы двух потоков
    WaitForMultipleObjects(50, handles, true, INFINITE);
    printf("Final Balance: %d\n", GetBalance());

    getchar();

    //DeleteCriticalSection(&FileLockingCriticalSection);
    CloseHandle(FileMutex);

    return 0;
}