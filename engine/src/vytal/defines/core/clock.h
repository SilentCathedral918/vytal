#pragma once

#include <time.h>

#include "types.h"

// wall clock ----------------------------------------------------------- //

typedef struct tm WallClockTimeInfo;
typedef time_t    WallClockTimestamp;

typedef enum WallClock_MonthOfYear {
    MONTH_JANUARY = 1,
    MONTH_FEBRUARY,
    MONTH_MARCH,
    MONTH_APRIL,
    MONTH_MAY,
    MONTH_JUNE,
    MONTH_JULY,
    MONTH_AUGUST,
    MONTH_SEPTEMBER,
    MONTH_OCTOBER,
    MONTH_NOVEMBER,
    MONTH_DECEMBER
} MonthOfYear;

typedef enum WallClock_DayOfWeek {
    DAY_MONDAY = 0,
    DAY_TUESDAY,
    DAY_WEDNESDAY,
    DAY_THURSDAY,
    DAY_FRIDAY,
    DAY_SATURDAY,
    DAY_SUNDAY
} DayOfWeek;

typedef struct WallClock_Handle {
    WallClockTimeInfo  _time_info;
    WallClockTimestamp _timestamp;
} WallClock;

typedef struct WallClock_Date {
    UInt32 _year;
    UInt32 _month;
    UInt32 _day;
} WallClockDate;

// high-resolution clock ------------------------------------------------ //

#if defined(_MSC_VER)
#    include <Windows.h>

typedef LARGE_INTEGER HiResTick, HiResInterval, HiResTimestamp;

typedef struct {
    LARGE_INTEGER _timestamp;
    LARGE_INTEGER _counter;
} HiResClock;

#elif defined(__clang__) || defined(__GNUC__)
#    include <unistd.h>

typedef UInt64          HiResTick, HiResInterval, HiResTimestamp;
typedef struct timespec HiResTimeSpec;

typedef struct {
    struct timespec _timestamp;
} HiResClock;

#else
#    error "Unsupported compiler or platform."

#endif
