#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>
#include <stdbool.h>

#define nr_of_tests 100

pthread_mutex_t mtx;
int ready = 0;
volatile DWORD thread_cpu = -1;

double convert_in_nanoseconds(struct timespec start, struct timespec end){
    return (double) ((end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec));
}

void measure_static_allocation(int size) {
    double time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        int array[size];

        clock_gettime(CLOCK_MONOTONIC, &end);

        time += convert_in_nanoseconds(start, end);
    }

    time /= nr_of_tests;
//    printf("Static allocation time (ns): %.3lf\n", time);
    printf("%.3lf\n", time);
}

void measure_dynamic_allocation(int size) {
    double time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        int *array = (int *) malloc(size * sizeof(int));

        clock_gettime(CLOCK_MONOTONIC, &end);

        free(array);
        time += convert_in_nanoseconds(start, end);
    }

    time /= nr_of_tests;
//    printf("Dynamic allocation time (ns): %.3lf\n", time);
    printf("%.3lf\n", time);
}

void measure_memory_access(int size) {
    double time = 0.0;
    int array[size];

    for (int i = 0; i < size; ++i) {
        array[i] = i;
    }

    for (int i = 0; i < nr_of_tests; ++i) {
        struct timespec start, end;
        int x;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int j = 0; j < size; ++j) {
            x = array[j];
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        time += convert_in_nanoseconds(start, end);
    }

    time /= nr_of_tests;
//    printf("Memory access time (ns): %.3lf\n", time);
    printf("%.3lf\n", time);
}

void *thread_function_create(void *arg) {
    return NULL;
}

void create_thread(int size) {
    double time = 0.0;
    pthread_t thread[size];

    for (int i = 0; i < nr_of_tests; ++i) {
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int j = 0; j < size; ++j) {
            pthread_create(&thread[j], NULL, thread_function_create, NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        for (int j = 0; j < size; ++j) {
            pthread_join(thread[j], NULL);
        }

        time += convert_in_nanoseconds(start, end);
    }

    time /= nr_of_tests;
//    printf("Thread creation time (ns): %.3lf\n", time);
    printf("%.3lf\n", time);
}

void *thread_function_switch(void *arg) {
    long long *switch_time = (long long *) arg;

    while (!ready) {}

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < 100; ++i) {
        pthread_mutex_lock(&mtx);
        pthread_mutex_unlock(&mtx);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    *switch_time = convert_in_nanoseconds(start, end) / 100;

    return NULL;
}

void context_switch(int size) {
    double total_switch_time = 0.0;

    for (int i = 0; i < nr_of_tests; ++i) {
        pthread_t thread[size];
        long long switch_time[size];

        for (int j = 0; j < size; ++j) {
            switch_time[j] = 0;
        }

        ready = 0;

        for (int j = 0; j < size; ++j) {
            pthread_create(&thread[j], NULL, thread_function_switch, (void *) &switch_time[j]);
        }

        ready = 1;

        for (int j = 0; j < size; ++j) {
            pthread_join(thread[j], NULL);
        }

        for (int j = 0; j < size; ++j) {
            total_switch_time += (double) switch_time[j];
        }
    }

    double time = total_switch_time / nr_of_tests;
//    printf("Thread context switch time (ns): %.3f\n", time);
    printf("%.3lf\n", time);
}

DWORD WINAPI thread_function_migration(LPVOID lpParam) {
    thread_cpu = GetCurrentProcessorNumber();
    return 0;
}

void thread_migration() {
    double time = 0.0;
    struct timespec start, end;

    for (int i = 0; i < nr_of_tests; ++i) {
        bool good_CPU = false;

        while (!good_CPU) {
            thread_cpu = -1;

            HANDLE thread = CreateThread(
                    NULL, 0,
                    thread_function_migration,
                    NULL, 0, NULL
            );

            if (thread == NULL) {
                fprintf(stderr, "Failed to create thread\n");
                return;
            }

            clock_gettime(CLOCK_MONOTONIC, &start);

            DWORD_PTR affinityMask = 1 << 1;
            SetThreadAffinityMask(thread, affinityMask);

            clock_gettime(CLOCK_MONOTONIC, &end);

            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);

            if(thread_cpu == 1)
                good_CPU = true;
        }

        time += convert_in_nanoseconds(start, end);
    }

    time /= nr_of_tests;
//    printf("Thread migration time (ns): %.3f\n", time);
    printf("%.3lf\n", time);
}

int main(int argc, char *argv[]) {
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
