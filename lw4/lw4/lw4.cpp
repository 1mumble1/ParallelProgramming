#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <fstream>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

struct ThreadData
{
    cv::Mat* image;
    int startCol;
    int endCol;
    std::vector<DWORD>* duration;
};

DWORD startTime;

DWORD WINAPI BlurImagePart(LPVOID lpParam)
{
    ThreadData* data = (ThreadData*)lpParam;
    cv::Mat roi = (*data->image)(cv::Range::all(), cv::Range(data->startCol, data->endCol));

    //auto start = std::chrono::high_resolution_clock::now();
    for (int y = 1; y < roi.rows - 1; ++y) {
        for (int x = 1; x < roi.cols - 1; ++x) {
            // Создаем небольшое изображение, содержащее только интересующий пиксель и его соседей
            cv::Rect pixels(x - 1, y - 1, 3, 3);
            cv::Mat smallImage = roi(pixels);

            // Применяем cv::blur к небольшому изображению
            for (int i = 0; i < 20; i++)
            {
                cv::blur(smallImage, smallImage, cv::Size(3, 3));
            }

            // Копируем результат обратно в исходное изображение
            smallImage.copyTo(roi(pixels));

            //auto end = std::chrono::high_resolution_clock::now();
            //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            DWORD currentTime = timeGetTime();
            data->duration->push_back(currentTime - startTime);
        }
    }

    ExitThread(0);
}

void ProcessImage(const std::string& inputPath, const std::string& outputPath, int numThreads, int numCores, const std::vector<int>& priorities)
{
    // Загрузка изображения
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image: " << inputPath << std::endl;
        return;
    }

    // Разделение изображения на вертикальные полосы
    int width = image.cols;
    int stripWidth = width / numThreads;

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);
    std::vector<std::vector<DWORD>> durations(numThreads);

    auto start = std::chrono::high_resolution_clock::now();
    startTime = timeGetTime();
    // Создание потоков для обработки каждой полосы
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].image = &image;
        threadData[i].startCol = i * stripWidth;
        threadData[i].endCol = (i == numThreads - 1) ? width : (i + 1) * stripWidth;
        threadData[i].duration = &durations[i];

        threads[i] = CreateThread(NULL, 0, BlurImagePart, &threadData[i], 0, NULL);
        if (threads[i] == NULL)
        {
            std::cerr << "Failed to create thread " << i << std::endl;
            return;
        }

        SetThreadAffinityMask(threads[i], 1 << (i % numCores));
        // Установка маски сходства для эмуляции количества ядер

        if (!SetThreadPriority(threads[i], priorities[i]))
        {
            std::cerr << "Error setting thread priority" << std::endl;
            return;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < numThreads; ++i)
    {
        CloseHandle(threads[i]);
    }

    std::ofstream log("log.txt");
    for (int i = 0; i < numThreads; ++i)
    {
        for (int j = 0; j < durations[i].size(); j++)
        {
            //запись в файлы
            log << /*i << " " <<*/ durations[i][j] << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Сохранение обработанного изображения
    if (!cv::imwrite(outputPath, image))
    {
        std::cerr << "Could not write the image to: " << outputPath << std::endl;
    }

    std::cout << "Processing time: " << duration << " ms" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads> <num_cores>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    int numThreads = std::stoi(argv[3]);
    int numCores = std::stoi(argv[4]);

    std::cout << "Enter priority of threads:\n-1 - below_normal\n0 - normal\n1 - above_normal\n";
    std::vector<int> priorities;
    int priority;
    for (int i = 0; i < numThreads; i++)
    {
        std::cin >> priority;
        priorities.push_back(priority);
    }

    ProcessImage(inputPath, outputPath, numThreads, numCores, priorities);

    return 0;
}