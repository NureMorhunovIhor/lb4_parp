#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <omp.h>

using namespace std;

bool compareFiles(const string& file1, const string& file2) {
    ifstream f1(file1), f2(file2);
    string line1, line2;
    int lineNumber = 1;

    while (getline(f1, line1) && getline(f2, line2)) {
        if (line1 != line2) {
            cout << "Error on line " << lineNumber << ":\n"
                << "  " << file1 << ": " << line1 << "\n"
                << "  " << file2 << ": " << line2 << endl;
            return false;
        }
        ++lineNumber;
    }

    if (f1.eof() != f2.eof()) {
        cout << "Files have a different number of lines." << endl;
        return false;
    }

    cout << "Results match!" << endl;
    return true;
}

double readExecutionTime(const string& filename) {
    ifstream timeFile(filename);
    double time = 0.0;
    if (timeFile.is_open()) {
        string line;
        if (getline(timeFile, line)) {
            time = stod(line); // Преобразование строки в double
        }
        timeFile.close();
    }
    else {
        cout << "Error: could not open time file " << filename << endl;
    }
    return time;
}

int main() {
    // Выполнение последовательного поиска
    system("..\\x64\\Debug\\lb4_parp_seq.exe");
    double sequentialTime = readExecutionTime("..\\lb4_parp\\sequential_time.txt"); // Чтение времени из файла
    if (sequentialTime == 0.0) {
        cerr << "Error: Sequential time is 0. Check the output file." << endl;
        return 1;  // Выход, если время равно 0
    }

    // Выполнение параллельного поиска
    system("..\\x64\\Debug\\lb4_parp_parallel.exe");
    double parallelTime = readExecutionTime("..\\lb4_parp\\parallel_time.txt"); // Предполагаем, что у вас аналогично записано время для параллельного поиска
    if (parallelTime == 0.0) {
        cerr << "Error: Parallel time is 0. Check the output file." << endl;
        return 1;  // Выход, если время равно 0
    }

    bool isCorrect = compareFiles("..\\results.txt", "..\\results_parallel.txt");

    int numThreads = 16;
    double speedup = sequentialTime / parallelTime; // Используем значения времени из файлов
    double efficiency = speedup / numThreads;

    cout << "Speedup: " << speedup << endl;
    cout << "Efficiency: " << efficiency * 100 << " %" << endl;

    if (isCorrect) {
        cout << "All tests passed successfully!" << endl;
    }
    else {
        cout << "Error: Results do not match." << endl;
    }

    return 0;
}
