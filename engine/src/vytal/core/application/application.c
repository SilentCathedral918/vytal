#include "application.h"

#include "vytal/core/hal/clock/hiresclock.h"
#include "vytal/core/hal/clock/wallclock.h"

#include <stdio.h>

void _print_time(ConstStr title, WallClock clock) {
    printf("%s Time: %d-%02d-%02d %02d:%02d:%02d\n", title, clock._time_info.tm_year + 1900, clock._time_info.tm_mon + 1,
           clock._time_info.tm_mday, clock._time_info.tm_hour, clock._time_info.tm_min, clock._time_info.tm_sec);
}

void test_wall(void) {
    WallClock local = hal_wallclock_now();
    _print_time("Local", local);

    WallClock utc = hal_wallclock_now_utc();
    _print_time("UTC", utc);

    WallClock today = hal_wallclock_today();
    _print_time("Today", today);

    WallClock from_julian = hal_wallclock_from_julian(2451544.75);
    _print_time("Julian -> Gregorian", from_julian);

    Flt64 to_julian = hal_wallclock_to_julian(&from_julian);
    printf("Gregorian -> Julian: %lf\n", to_julian);

    WallClock from_unix = hal_wallclock_from_unix(1733461851);
    _print_time("Unix -> UTC", from_unix);

    Int64 unix = hal_wallclock_to_unix(&utc);
    printf("UTC -> Unix: %lld\n", unix);

    WallClock parsed = hal_wallclock_parse("2023-01-01 00:00:00");
    _print_time("Parsed", parsed);

    WallClock parsed_utc = hal_wallclock_parse_utc("2023-01-01 00:00:00");
    _print_time("Parsed UTC", parsed_utc);

    WallClock clock;
    clock._timestamp         = 1733461851; // Example timestamp (Unix time)
    clock._time_info.tm_year = 2023 - 1900;
    clock._time_info.tm_mon  = 11; // December (0-based)
    clock._time_info.tm_mday = 6;
    clock._time_info.tm_wday = 3; // Wednesday
    clock._time_info.tm_yday = 340;
    clock._time_info.tm_hour = 15; // 3 PM
    clock._time_info.tm_min  = 45;
    clock._time_info.tm_sec  = 30;

    char result[128];
    hal_wallclock_tostring(result, &clock);
    printf("String Representation: %s\n", result);

    printf("Year: %d\n", hal_wallclock_getyear(&clock));
    printf("Month: %d\n", hal_wallclock_getmonth(&clock));
    printf("Month of Year: %d\n", hal_wallclock_getmonthofyear(&clock));
    printf("Day: %d\n", hal_wallclock_getday(&clock));
    printf("Day of Week: %d\n", hal_wallclock_getdayofweek(&clock));
    printf("Day of Year: %d\n", hal_wallclock_getdayofyear(&clock));
    printf("Hour (12-hour format): %d\n", hal_wallclock_gethour12(&clock));
    printf("Hour (24-hour format): %d\n", hal_wallclock_gethour24(&clock));
    printf("Minute: %d\n", hal_wallclock_getminute(&clock));
    printf("Second: %d\n", hal_wallclock_getsecond(&clock));
    printf("Is AM: %s\n", hal_wallclock_isam(&clock) ? "Yes" : "No");
    printf("Is PM: %s\n", hal_wallclock_ispm(&clock) ? "Yes" : "No");
    printf("Is Leap Year: %s\n", hal_wallclock_isleapyear(&clock) ? "Yes" : "No");
}

void test_hires(void) {
    HiResClock clock;
    hal_hiresclock_init(&clock);

    VT_HIRES_SUSPEND(2); // sleep for 2 seconds

    Flt64 elapsed_seconds = hal_hiresclock_getelapsed_sec(&clock);
    Flt64 elapsed_millis  = hal_hiresclock_getelapsed_millisec(&clock);
    Flt64 elapsed_micros  = hal_hiresclock_getelapsed_microsec(&clock);
    Flt64 elapsed_nanos   = hal_hiresclock_getelapsed_nanosec(&clock);
    printf("Elapsed Time: %.6f seconds\n", elapsed_seconds);
    printf("Elapsed Time: %.6f milliseconds\n", elapsed_millis);
    printf("Elapsed Time: %.6f microseconds\n", elapsed_micros);
    printf("Elapsed Time: %.6f nanoseconds\n", elapsed_nanos);

    Flt64 frequency = hal_hiresclock_getfrequency(&clock);
    printf("Clock frequency: %.12f Hz\n", frequency);
    printf("Clock frequency: %.9f Hz\n", hal_hiresclock_getfrequency(&clock));
    printf("Clock frequency: %.9f kHz\n", hal_hiresclock_getfrequency_khz(&clock));
    printf("Clock frequency: %.9f MHz\n", hal_hiresclock_getfrequency_mhz(&clock));
    printf("Clock frequency: %.9f GHz\n", hal_hiresclock_getfrequency_ghz(&clock));
}
