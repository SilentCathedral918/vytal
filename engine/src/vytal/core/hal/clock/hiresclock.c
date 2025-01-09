#include "hiresclock.h"

#define SEC_NANOSEC_SUBMULT 1000000000L

void hal_hiresclock_init(HiResClock *clock) {
#if defined(_MSC_VER)
    QueryPerformanceFrequency(&(clock->_counter));
    QueryPerformanceCounter(&(clock->_timestamp));

#elif defined(__clang__) || defined(__GNUC__)
    clock_gettime(CLOCK_MONOTONIC, VT_CAST(HiResTimeSpec *, &(clock->_timestamp)));

#endif
}

Flt64 hal_hiresclock_getelapsed_sec(HiResClock *clock) {
#if defined(_MSC_VER)
    HiResTick current_;
    QueryPerformanceCounter(&current_);

    HiResInterval interval_ = current_.QuadPart - clock->_timestamp.QuadPart;

    return VT_CAST(Flt64, interval.QuadPart) / clock->_counter.QuadPart;

#elif defined(__clang__) || defined(__GNUC__)
    HiResTimeSpec current_;
    clock_gettime(CLOCK_MONOTONIC, &current_);

    HiResInterval interval_ = (current_.tv_sec - clock->_timestamp.tv_sec) * SEC_NANOSEC_SUBMULT + (current_.tv_nsec - clock->_timestamp.tv_nsec);

    return VT_CAST(Flt64, interval_) / SEC_NANOSEC_SUBMULT;

#endif
}

VT_API Flt64 hal_hiresclock_getfrequency(HiResClock *clock) {
#if defined(_MSC_VER)
    if (clock->_counter.QuadPart == 0) {
        if (!QueryPerformanceFrequency(&clock->_counter)) {
            return 0.0;
        }
    }
    return VT_CAST(Flt64, clock->_counter.QuadPart); // frequency in ticks per second

#elif defined(__clang__) || defined(__GNUC__)
    struct timespec ts_;
    if (clock_getres(CLOCK_MONOTONIC, &ts_) == 0) {
        return 1.0 / (ts_.tv_sec + ts_.tv_nsec / 1000000000.0);
    } else {
        return 0.0;
    }
#endif
}