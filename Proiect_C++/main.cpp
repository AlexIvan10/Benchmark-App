#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <windows.h>
#include <iomanip>
#include <cstring>

#define nr_of_tests 100

using namespace std;

mutex mtx;
bool ready = false;
volatile DWORD thread_cpu = -1;

void measure_static_allocation(int size) {
    double time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {

        auto start = chrono::high_resolution_clock ::now();

        int array[size];

        auto end = chrono::high_resolution_clock ::now();
        time += static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
    }

    time /= nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Static allocation time (ns): " << time << endl;
    cout << time;
}

void measure_dynamic_allocation(int size) {
    double time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {

        auto start = chrono::high_resolution_clock ::now();

        int *array = new int[size];

        auto end = chrono::high_resolution_clock ::now();

        delete[] array;

        time += static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
    }

    time /= nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Dynamic allocation time (ns): " << time << endl;
    cout << time;
}

void measure_memory_access(int size) {
    double time = 0.0;
    int array[size];
    int x;
    for (int i = 0; i < size; ++i) {
        array[i] = i;
    }

    for (int i = 0; i < nr_of_tests; ++i) {

        auto start = chrono::high_resolution_clock ::now();

        for (int j = 0; j < size; ++j) {
            x = array[j];
        }

        auto end = chrono::high_resolution_clock ::now();
        time += static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
    }

    time /= nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Memory access time (ns): " << time << endl;
    cout << time;
}

void thread_function_create() {}

void create_thread(int size) {
    double time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {
        thread t[size];

        auto start = chrono::high_resolution_clock ::now();

        for (int j = 0; j < size; ++j) {
            t[j] = thread(thread_function_create);
        }

        auto end = chrono::high_resolution_clock ::now();

        for (int j = 0; j < size; ++j) {
            t[j].join();
        }

        time += static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
    }

    time /= nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Thread creation time (ns): " << time << endl;
    cout << time;
}

void thread_function_switch(bool &flag, long long &switch_time) {
    while (!flag) {}

    auto start = chrono::high_resolution_clock ::now();

    for (int i = 0; i < 100; ++i) {
        mtx.lock();
        mtx.unlock();
    }

    auto end = chrono::high_resolution_clock ::now();
    switch_time = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 100;
}

void context_switch(int size) {
    double total_switch_time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {
        thread t[size];
        long long switch_time[size];

        for (int j = 0; j < size; ++j) {
            switch_time[j] = 0;
        }

        ready = false;

        for (int j = 0; j < size; ++j) {
            t[j] = thread(thread_function_switch, ref(ready), ref(switch_time[j]));
        }

        ready = true;

        for (int j = 0; j < size; ++j) {
            t[j].join();
        }

        for (int j = 0; j < size; ++j) {
            total_switch_time += (double) switch_time[j];
        }
    }

    double time = total_switch_time / nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Thread context switch time (ns): " << time << endl;
    cout << time;
}

DWORD WINAPI thread_function_migration(LPVOID lpParam) {
    thread_cpu = GetCurrentProcessorNumber();
    return 0;
}

void thread_migration() {
    double time = 0.0;
    chrono::high_resolution_clock ::time_point start, end;

    for (int i = 0; i < nr_of_tests; ++i) {
        boolean good_CPU = false;

        while (!good_CPU) {
            HANDLE thread = CreateThread(
                    nullptr, 0,
                    thread_function_migration,
                    nullptr, 0, nullptr);

            if (thread == nullptr) {
                cerr << "Failed to create thread" << endl;
                return;
            }

            start = chrono::high_resolution_clock ::now();

            DWORD_PTR affinityMask = 1 << 1;
            SetThreadAffinityMask(thread, affinityMask);

            end = chrono::high_resolution_clock ::now();

            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);

            if(thread_cpu == 1)
                good_CPU = true;
        }

        time += static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(end - start).count());
    }

    time /= nr_of_tests;

    cout << fixed << setprecision(3);
//    cout << "Thread migration time (ns): " << time << endl;
    cout << time;
}

int main(int argc, char* argv[]) {
    int size;

    if(argc == 3)
        size = atoi(argv[2]);

    if (strcmp(argv[1], "static_memory") == 0) {
        measure_static_allocation(size);
    } else if (strcmp(argv[1], "dynamic_memory") == 0) {
        measure_dynamic_allocation(size);
    } else if (strcmp(argv[1], "memory_access") == 0) {
        measure_memory_access(size);
    } else if (strcmp(argv[1], "create_thread") == 0) {
        create_thread(size);
    } else if (strcmp(argv[1], "context_switch") == 0) {
        context_switch(size);
    } else if (strcmp(argv[1], "thread_migration") == 0) {
        thread_migration();
    } else {
        printf("Eroare: Operatie necunoscuta '%s'.\n\n", argv[1]);
        return 1;
    }

    return 0;
}
