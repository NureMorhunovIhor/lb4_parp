#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

int readKeys(const string& filename, string* keys, int maxKeys) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: could not open file " << filename << endl;
        return -1;
    }

    int count;
    file >> count;
    if (count > maxKeys) {
        cout << "Error: exceeded maximum number of keys." << endl;
        return -1;
    }

    for (int i = 0; i < count; ++i) {
        file >> keys[i];
    }

    return count;
}

// Search for keys and write results
void searchKeys(const string* sortedKeys, int keyCount, const string& queryFile, const string& outputFile) {
    ifstream queries(queryFile);
    if (!queries.is_open()) {
        cout << "Error: could not open file " << queryFile << endl;
        return;
    }

    ofstream results(outputFile);
    if (!results.is_open()) {
        cout << "Error: could not open file " << outputFile << endl;
        return;
    }

    string query;

    // Запись времени начала
    auto start = high_resolution_clock::now();

    while (queries >> query) {
        auto it = find(sortedKeys, sortedKeys + keyCount, query);
        if (it != sortedKeys + keyCount) {
            int index = distance(sortedKeys, it);
            results << query << " is found at index " << index << endl;
        }
        else {
            results << query << " is NOT FOUND" << endl;
        }
    }

    // Запись времени окончания
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;

    // Запись времени выполнения в файл
    ofstream timeFile("sequential_time.txt");
    if (timeFile.is_open()) {
        timeFile << duration.count() << endl; // Записываем время в файл
        timeFile.close();
    }

    cout << "Sequential search execution time: " << duration.count() << " секунд." << endl;
}

int main() {
    const int MAX_KEYS = 1000;
    string sortedKeys[MAX_KEYS];

    int keyCount = readKeys("..\\file1.txt", sortedKeys, MAX_KEYS);
    if (keyCount == -1) {
        return 1;
    }

    searchKeys(sortedKeys, keyCount, "..\\file2.txt", "results.txt");
    cout << "Sequential search completed." << endl;
    return 0;
}
