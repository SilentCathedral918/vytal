#include "delay.h"

#include <stdio.h>
#if defined(_MSC_VER)
#    include <Windows.h>

#elif defined(__clang__) || defined(__GNUC__)
#    include <time.h>
#    include <unistd.h>

#endif

void delay(Flt32 seconds) {
#if defined(_MSC_VER)
    LARGE_INTEGER frequency_;
    LARGE_INTEGER start_, end_;
    Flt32         elapsed_ = 0.0f;

    QueryPerformanceFrequency(&frequency_);
    QueryPerformanceCounter(&start_);

    do {
        QueryPerformanceCounter(&end_);
        elapsed_ = VT_CAST(Flt32, end_.QuadPart - start_.QuadPart) / VT_CAST(Flt32, frequency_.QuadPart);

    } while (elapsed_ < seconds);

#elif defined(__clang__) || defined(__GNUC__)
    struct timespec start_, current_;
    Flt32           elapsed_ = 0.0f;

    clock_gettime(CLOCK_MONOTONIC, &start_);

    do {
        clock_gettime(CLOCK_MONOTONIC, &current_);
        elapsed_ = (current_.tv_sec - start_.tv_sec) + (current_.tv_nsec - start_.tv_nsec) / 1e9f;

    } while (elapsed_ < seconds);

#endif
}
