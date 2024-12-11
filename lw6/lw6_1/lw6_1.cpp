#include <fstream>
#include <omp.h>

const int MAX_ITERATIONS = 1'000'000'000;

double GetPIWithSimpleFor()
{
	double pi = 0.0;

	for (int i = 0; i < MAX_ITERATIONS; i++)
	{
		double temp = ((i % 2 == 0) ? 1.0 : (-1.0)) / (2 * i + 1);
		pi += temp;
	}

	return pi * 4.0;
}

double GetPIWithParallelFor()
{
	double pi = 0.0;

#pragma omp parallel for default(none) shared(pi)
	for (int i = 0; i < MAX_ITERATIONS; i++)
	{
		double temp = ((i % 2 == 0) ? 1.0 : (-1.0)) / (2 * i + 1);
		pi += temp;
	}

	return pi * 4.0;
}

double GetPIWithParallelForAndAtomic()
{
	double pi = 0.0;

#pragma omp parallel for default(none) shared(pi)
	for (int i = 0; i < MAX_ITERATIONS; i++)
	{
		double temp = ((i % 2 == 0) ? 1.0 : (-1.0)) / (2 * i + 1);
#pragma omp atomic
		pi += temp;
	}

	return pi * 4.0;
}

double GetPIWithParallelForAndReduction()
{
	double pi = 0.0;

#pragma omp parallel for default(none) reduction(+:pi)
	for (int i = 0; i < MAX_ITERATIONS; i++)
	{
		double temp = ((i % 2 == 0) ? 1.0 : (-1.0)) / (2 * i + 1);
		pi += temp;
	}

	return pi * 4.0;
}

int main()
{
	std::ofstream result("result.txt");
	double pi = 0.0;
	double time;

	time = omp_get_wtime();
	pi = GetPIWithSimpleFor();
	result << "1) simple 'for':" << std::endl;
	result << "PI: " << pi << std::endl
		<< "Time: " << omp_get_wtime() - time << std::endl;
	result << std::endl;

	time = omp_get_wtime();
	pi = GetPIWithParallelFor();
	result << "2) parallel 'for' (wrong version):" << std::endl;
	result << "PI: " << pi << std::endl
		<< "Time: " << omp_get_wtime() - time << std::endl;
	result << std::endl;

	time = omp_get_wtime();
	pi = GetPIWithParallelForAndAtomic();
	result << "3) parallel 'for' with atomic:" << std::endl;
	result << "PI: " << pi << std::endl
		<< "Time: " << omp_get_wtime() - time << std::endl;
	result << std::endl;

	time = omp_get_wtime();
	pi = GetPIWithParallelForAndReduction();
	result << "4) parallel 'for' with reduction:" << std::endl;
	result << "PI: " << pi << std::endl
		<< "Time: " << omp_get_wtime() - time << std::endl;
	result << std::endl;

	return 0;
}