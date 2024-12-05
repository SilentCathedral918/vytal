#include "wallclock.h"

#define THREADSAFE_LOCALTIME(t, result) localtime_s(t, result)
#define THREADSAFE_GMTIME(t, result) gmtime_s(t, result)

WallClock hal_wallclock_now(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    THREADSAFE_LOCALTIME(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock hal_wallclock_now_utc(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    THREADSAFE_GMTIME(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock hal_wallclock_today(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    THREADSAFE_LOCALTIME(&(clock_._time_info), &(clock_._timestamp));

    // set time to midnight
    {
        clock_._time_info.tm_hour = 0;
        clock_._time_info.tm_min  = 0;
        clock_._time_info.tm_sec  = 0;
    }

    return clock_;
}
