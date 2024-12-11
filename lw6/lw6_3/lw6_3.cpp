#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <omp.h>

typedef std::vector<std::vector<int>> Matrix;
typedef std::vector<int> LineMatrix;

Matrix ReadMatrixFromFile(const std::string& fileName)
{
	std::ifstream input(fileName);
	Matrix matrix;
	std::string line;

	while (std::getline(input, line))
	{
		std::stringstream ss(line);
		LineMatrix lineMatrix;
		int cell;
		while (!ss.eof())
		{
			if (ss >> cell)
			{
				lineMatrix.push_back(cell);
			}
		}
		matrix.push_back(lineMatrix);
	}

	return matrix;
}

void PrintMatrix(const Matrix& mat, std::ostream& output)
{
	for (const auto& line : mat)
	{
		for (const auto& cell : line)
		{
			output << cell << " ";
		}
		output << std::endl;
	}
}

Matrix MulMatrix(const Matrix& first, const Matrix& second)
{
	size_t size = first.size();
	Matrix result(size, std::vector<int>(size));

	int cell = 0;
#pragma omp parallel for default(none) shared(size) reduction(+:cell)
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cell = 0;
			for (int k = 0; k < size; k++)
			{
				cell += first[i][k] * second[k][j];
			}
			result[i][j] = cell;
		}
	}

	return result;
}

int main()
{
	std::string inputFirstFileName;
	std::string inputSecondFileName;

	std::cout << "Enter file name of 1st matrix: ";
	std::getline(std::cin, inputFirstFileName);
	Matrix firstMatrix = ReadMatrixFromFile(inputFirstFileName);

	std::cout << "Enter file name of 2nd matrix: ";
	std::getline(std::cin, inputSecondFileName);
	Matrix secondMatrix = ReadMatrixFromFile(inputSecondFileName);

	PrintMatrix(MulMatrix(firstMatrix, secondMatrix), std::cout);

	return 0;
}
