#include "hires.h"

#define SEC_MILLISEC_MULT_FACTOR (1000)
#define SEC_MICROSEC_MULT_FACTOR (1000000)
#define SEC_NANOSEC_MULT_FACTOR (1000000000L)

VYTAL_API void clock_hires_init(HiResClock *clock) {
#if defined(_MSC_VER)
    QueryPerformanceFrequency(&(clock->_counter));
    QueryPerformanceCounter(&(clock->_timestamp));

#elif defined(__clang__) || defined(__GNUC__)
    clock_gettime(CLOCK_MONOTONIC, (HiResTimeSpec *)&(clock->_timestamp));

#endif
}

VYTAL_API Flt64 clock_hires_elapsed_seconds(HiResClock *clock) {
#if defined(_MSC_VER)
    HiResTick current_;
    QueryPerformanceCounter(&current_);

    HiResInterval interval_ = current_.QuadPart - clock->_timestamp.QuadPart;
    return (Flt64)interval_.QuadPart / clock->_counter.QuadPart;

#elif defined(__clang__) || defined(__GNUC__)
    HiResTimeSpec current_;
    if (clock_gettime(CLOCK_MONOTONIC, &current_) != 0) return 0.0;

    HiResInterval interval_ = (current_.tv_sec - clock->_timestamp.tv_sec) * SEC_NANOSEC_MULT_FACTOR + (current_.tv_nsec - clock->_timestamp.tv_nsec);
    return (Flt64)interval_ / SEC_NANOSEC_MULT_FACTOR;

#endif
}

VYTAL_API Flt64 clock_hires_elapsed_milliseconds(HiResClock *clock) {
    return (clock_hires_elapsed_seconds(clock) * (Flt64)SEC_MILLISEC_MULT_FACTOR);
}

VYTAL_API Flt64 clock_hires_elapsed_microseconds(HiResClock *clock) {
    return (clock_hires_elapsed_seconds(clock) * (Flt64)SEC_MICROSEC_MULT_FACTOR);
}

VYTAL_API Flt64 clock_hires_elapsed_nanoseconds(HiResClock *clock) {
    return (clock_hires_elapsed_seconds(clock) * (Flt64)SEC_NANOSEC_MULT_FACTOR);
}

VYTAL_API Flt64 clock_hires_frequency(HiResClock *clock) {
#if defined(_MSC_VER)
    if (!clock->_counter.QuadPart && !QueryPerformanceFrequency(&clock->_counter))
        return 0.0;

    return (Flt64)clock->_counter.QuadPart;  // frequency in ticks per second

#elif defined(__clang__) || defined(__GNUC__)
    struct timespec ts_;

    if (!clock_getres(CLOCK_MONOTONIC, &ts_))
        return (Flt64)SEC_NANOSEC_MULT_FACTOR / (ts_.tv_nsec ? ts_.tv_nsec : 1);
    else
        return 0.0;

#endif
}

VYTAL_API Flt64 clock_hires_frequency_khz(HiResClock *clock) {
    return (clock_hires_frequency(clock) * (Flt64)SEC_MILLISEC_MULT_FACTOR);
}

VYTAL_API Flt64 clock_hires_frequency_mhz(HiResClock *clock) {
    return (clock_hires_frequency(clock) * (Flt64)SEC_MICROSEC_MULT_FACTOR);
}

VYTAL_API Flt64 clock_hires_frequency_ghz(HiResClock *clock) {
    return (clock_hires_frequency(clock) * (Flt64)SEC_NANOSEC_MULT_FACTOR);
}
