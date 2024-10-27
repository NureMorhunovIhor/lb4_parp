#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <omp.h>
#include <sstream>
#include <chrono> // Для замера времени

using namespace std;

const int MAX_KEYS = 1000;      // Максимальное количество ключей
const int MAX_QUERIES = 1000;   // Максимальное количество запросов
const int MAX_THREADS = 16;      // Максимальное количество потоков (например, 16)

// Функция для чтения ключей из файла в массив
int readKeys(const string& filename, string* keys) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: could not open file " << filename << endl;
        return -1;  // Код ошибки
    }

    int count;
    file >> count;  // Чтение количества ключей
    if (count > MAX_KEYS) {
        cout << "Error: exceeded maximum number of keys." << endl;
        return -1;  // Код ошибки
    }

    for (int i = 0; i < count; ++i) {
        file >> keys[i];
    }

    return count;  // Возврат фактического количества прочитанных ключей
}

// Параллельный поиск ключей
void searchKeysParallel(const string* sortedKeys, int keyCount, const string& queryFile, const string& outputFile) {
    // Чтение запросов в массив
    string queries[MAX_QUERIES];
    ifstream queriesFile(queryFile);
    if (!queriesFile.is_open()) {
        cout << "Error: could not open file " << queryFile << endl;
        return;
    }

    int queryCount = 0;
    while (queryCount < MAX_QUERIES && queriesFile >> queries[queryCount]) {
        queryCount++;
    }

    ofstream results(outputFile);
    if (!results.is_open()) {
        cout << "Error: could not open file " << outputFile << endl;
        return;
    }

    // Массив для хранения результатов каждого потока
    ostringstream localResults[MAX_THREADS];

    // Запись времени начала
    auto start = chrono::high_resolution_clock::now();

    // Параллельная область для поиска
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        // Параллельный цикл для поиска
#pragma omp for
        for (int i = 0; i < queryCount; ++i) {
            auto it = find(sortedKeys, sortedKeys + keyCount, queries[i]);
            if (it != sortedKeys + keyCount) {
                int index = distance(sortedKeys, it);
                localResults[thread_id] << queries[i] << " is found at index " << index << endl;
            }
            else {
                localResults[thread_id] << queries[i] << " is NOT FOUND" << endl;
            }
        }
    }

    // Запись всех результатов в выходной файл
    for (int i = 0; i < MAX_THREADS; ++i) {
        results << localResults[i].str();  // Запись результатов каждого потока в файл
    }

    // Запись времени окончания
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    // Запись времени выполнения в файл
    ofstream timeFile("parallel_time.txt");
    if (timeFile.is_open()) {
        timeFile << duration.count() << endl; // Записываем время в файл
        timeFile.close();
    }

    // Вывод времени выполнения
    cout << "Parallel search execution time: " << duration.count() << " секунд." << endl;
}

int main() {
    string sortedKeys[MAX_KEYS];  // Массив для хранения ключей
    int keyCount = readKeys("..\\file1.txt", sortedKeys);
    if (keyCount == -1) {
        return 1;  // Выход из программы в случае ошибки
    }

    searchKeysParallel(sortedKeys, keyCount, "..\\file2.txt", "results_parallel.txt");
    cout << "Parallel search completed." << endl;
    return 0;
}
