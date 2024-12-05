#include "application.h"

#include "vytal/core/hal/clock/wallclock.h"

#include <emmintrin.h>
#include <pthread.h>
#include <stdio.h>

void _print_time(ConstStr title, WallClock clock) {
    printf("%s Time: %d-%02d-%02d %02d:%02d:%02d\n", title, clock._time_info.tm_year + 1900, clock._time_info.tm_mon + 1,
           clock._time_info.tm_mday, clock._time_info.tm_hour, clock._time_info.tm_min, clock._time_info.tm_sec);
}

void test_time(void) {
    WallClock local = hal_wallclock_now();
    _print_time("Local", local);

    WallClock utc = hal_wallclock_now_utc();
    _print_time("UTC", utc);

    WallClock today = hal_wallclock_today();
    _print_time("Today", today);
}
