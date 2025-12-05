#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <string>

using namespace std;

class ParallelSumCalculator {
private:
    long long totalSum;
    mutex sumMutex;

public:
    ParallelSumCalculator() : totalSum(0) {}

    void calculatePartialSum(long long start, long long end) {
        long long partialSum = 0;

        for (long long i = start; i <= end; i++) {
            partialSum += i;
        }

        lock_guard<mutex> lock(sumMutex);
        totalSum += partialSum;

        cout << "Поток " << this_thread::get_id()
            << ": сумма от " << start << " до " << end
            << " = " << partialSum << endl;
    }

  
    long long calculateTotalSum(long long n, int numThreads) {
        if (n <= 0) {
            throw invalid_argument("N должно быть положительным числом");
        }
        if (numThreads <= 0) {
            throw invalid_argument("Количество потоков должно быть положительным");
        }

        totalSum = 0;

        long long rangeSize = n / numThreads;
        long long remainder = n % numThreads;

        vector<thread> threads;
        long long start = 1;

        cout << "Запуск " << numThreads << " потоков для вычисления суммы..." << endl;

        for (int i = 0; i < numThreads; i++) {
            long long end = start + rangeSize - 1;

            if (i < remainder) {
                end++;
            }

            if (end > n) {
                end = n;
            }

            threads.emplace_back(&ParallelSumCalculator::calculatePartialSum,
                this, start, end);

            start = end + 1;

            if (start > n) {
                break;
            }
        }

        for (auto& t : threads) {
            t.join();
        }

        return totalSum;
    }
};

long long getInputNumber(const string& prompt) {
    long long value;
    cout << prompt;

    while (!(cin >> value) || value <= 0) {
        cout << "Ошибка! Пожалуйста, введите положительное целое число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return value;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    try {
        long long n;
        const int DEFAULT_THREADS = 4;

        if (argc == 2) {
            try {
                n = stoll(argv[1]);
                if (n <= 0) {
                    throw invalid_argument("");
                }
            }
            catch (...) {
                cout << "Некорректный аргумент командной строки. Используется интерактивный режим." << endl;
                n = getInputNumber("Введите число N: ");
            }
        }
        else {
            n = getInputNumber("Введите число N: ");
        }

        ParallelSumCalculator calculator;

        long long result = calculator.calculateTotalSum(n, DEFAULT_THREADS);

        cout << "\n======================================" << endl;
        cout << "Общая сумма от 1 до " << n << ": " << result << endl;

        long long expected = n * (n + 1) / 2;
        cout << "Ожидаемая сумма (формула): " << expected << endl;

        if (result == expected) {
            cout << "Результат верный!" << endl;
        }
        else {
            cout << "Ошибка: результат не соответствует ожидаемому!" << endl;
        }

    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}