#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <windows.h>

struct ThreadData {
    cv::Mat* image;
    int startCol;
    int endCol;
};

DWORD WINAPI blurImagePart(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    cv::Mat roi = (*data->image)(cv::Range::all(), cv::Range(data->startCol, data->endCol));

    for (int i = 0; i < 7; i++)
    {
        cv::blur(roi, roi, cv::Size(5, 5)); // Используем фиксированный размер ядра размытия 5x5
    }
    
    return 0;
}

void processImage(const std::string& inputPath, const std::string& outputPath, int numThreads, int numCores) {
    // Загрузка изображения
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image: " << inputPath << std::endl;
        return;
    }

    // Разделение изображения на вертикальные полосы
    int width = image.cols;
    int height = image.rows;
    int stripWidth = width / numThreads;

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);

    auto start = std::chrono::high_resolution_clock::now();

    // Создание потоков для обработки каждой полосы
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].image = &image;
        threadData[i].startCol = i * stripWidth;
        threadData[i].endCol = (i == numThreads - 1) ? width : (i + 1) * stripWidth;

        threads[i] = CreateThread(NULL, 0, blurImagePart, &threadData[i], 0, NULL);
        if (threads[i] == NULL) {
            std::cerr << "Failed to create thread " << i << std::endl;
            return;
        }

        // Установка маски сходства для эмуляции количества ядер
        SetThreadAffinityMask(threads[i], 1 << (i % numCores));
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Сохранение обработанного изображения
    if (!cv::imwrite(outputPath, image)) {
        std::cerr << "Could not write the image to: " << outputPath << std::endl;
    }

    std::cout << "Processing time: " << duration << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads> <num_cores>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    int numThreads = std::stoi(argv[3]);
    int numCores = std::stoi(argv[4]);

    processImage(inputPath, outputPath, numThreads, numCores);

    return 0;
}