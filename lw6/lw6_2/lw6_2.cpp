#include <omp.h>
#include <iostream>

int main()
{
    int x = 44;
    // создает копию для каждого потока
#pragma omp parallel for private(x)
    for (int i = 0; i <= 10; i++)
    {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }

    printf("x is %d\n", x);
    // создает копию для каждого потока и после завершения всех потоков присваивает значение до начала параллельного выполнения
#pragma omp parallel for firstprivate(x)
    for (int i = 0; i <= 10; i++)
    {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }

    printf("x is %d\n", x);
    // создает копию для каждого потока и после завершения всех потоков присваивает значение после выполнения параллельного участка кода
#pragma omp parallel for lastprivate(x)
    for (int i = 0; i <= 10; i++)
    {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }

    printf("x is %d\n", x);

    return 0;
}
