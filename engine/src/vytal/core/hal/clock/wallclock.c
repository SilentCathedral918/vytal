#include "wallclock.h"

#include "vytal/core/misc/string/vtstr.h"

#include <stdio.h>

#if defined(VT_PLATFORM_WINDOWS)
#    define THREADSAFE_LOCALTIME(time_info, timestamp) localtime_s(time_info, timestamp)
#    define THREADSAFE_GMTIME(time_info, timestamp) gmtime_s(time_info, timestamp)
#    define THREADSAFE_ASCTIME(buf, sz, time_info) asctime_s(buf, sz, time_info)

#elif defined(VT_PLATFORM_LINUX)
#    define THREADSAFE_LOCALTIME(time_info, timestamp) localtime_r(time_info, timestamp)
#    define THREADSAFE_GMTIME(time_info, timestamp) gmtime_r(time_info, timestamp)
#    define THREADSAFE_ASCTIME(buf, sz, time_info) asctime_r(buf, sz, time_info)

#endif

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

WallClock hal_wallclock_today_utc(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    THREADSAFE_GMTIME(&(clock_._time_info), &(clock_._timestamp));

    // set time to midnight
    {
        clock_._time_info.tm_hour = 0;
        clock_._time_info.tm_min  = 0;
        clock_._time_info.tm_sec  = 0;
    }

    return clock_;
}

WallClockTimeInfo _hal_wallclock_timeinfo_from_julian(const Flt64 julian) {
#define DATE_OFFSET 68569
#define EPOCH_OFFSET 49
#define DAYS_IN_400_YEARS 146097
#define DAYS_IN_4_YEARS 1461
#define MONTHS_IN_YEAR 12
#define AVERAGE_DAYS_IN_MONTH 30.4375
#define DAYS_IN_MONTHS (AVERAGE_DAYS_IN_MONTH * 80)
#define BASE_YEAR 1900

    Int64 l_ = VT_CAST(Int64, julian) + DATE_OFFSET; // align with Gregorian date
    Int64 n_ = 4 * l_ / DAYS_IN_400_YEARS;
    l_       = l_ - (DAYS_IN_400_YEARS * n_ + 3) / 4;
    Int64 i_ = 4000 * (l_ + 1) / 1461001;
    l_       = l_ - DAYS_IN_4_YEARS * i_ / 4 + 31;
    Int64 j_ = 80 * l_ / DAYS_IN_MONTHS;
    Int64 k_ = l_ - DAYS_IN_MONTHS * j_ / 80;
    l_       = j_ / 11;
    j_       = j_ + 2 - MONTHS_IN_YEAR * l_;
    i_       = 100 * (n_ - EPOCH_OFFSET) + i_ + l_;

    WallClockTimeInfo time_info_ = VT_STRUCT(WallClockTimeInfo, 0);
    time_info_.tm_year           = i_ - BASE_YEAR; // convert to year since 1900
    time_info_.tm_mon            = j_ - 1;         // convert to 0-based month index
    time_info_.tm_mday           = k_;
    time_info_.tm_hour           = 0;
    time_info_.tm_min            = 0;
    time_info_.tm_sec            = 0;
    return time_info_;
}

WallClock hal_wallclock_from_julian(const Flt64 julian) {
    WallClock clock_;

    clock_._time_info = _hal_wallclock_timeinfo_from_julian(julian);
    clock_._timestamp = mktime(&(clock_._time_info));

    return clock_;
}

Int64 hal_wallclock_to_julian(const WallClock *clock) {
#define DAY_OFFSET 32075
#define DAYS_IN_4_YEARS 1461
#define DAYS_IN_YEAR 365
#define MONTHS_IN_YEAR 12

    Int64 year_  = clock->_time_info.tm_year + 1900; // adjust to actual year
    Int64 month_ = clock->_time_info.tm_mon + 1;     // convert to 1-based for correct month
    Int64 day_   = clock->_time_info.tm_mday;

    // adjust months (Jan and Feb are treated as 13th and 14th months of previous year)
    if (month_ <= 2) {
        month_ += 12;
        year_ -= 1;
    }

    Int64 a_ = (DAYS_IN_4_YEARS * (year_ + 4800 + ((month_ - 14) / MONTHS_IN_YEAR))) / 4;
    Int64 b_ = (367 * (month_ - 2 - MONTHS_IN_YEAR * ((month_ - 14) / MONTHS_IN_YEAR))) / MONTHS_IN_YEAR;
    Int64 c_ = (3 * ((year_ + 4900 + ((month_ - 14) / MONTHS_IN_YEAR)) / 100)) / 4;

    return ((a_ + b_ - c_) + day_ - DAY_OFFSET);
}

WallClock hal_wallclock_from_unix(const Int64 unix) {
    WallClock clock_;

    clock_._timestamp = unix;
    THREADSAFE_GMTIME(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

Int64 hal_wallclock_to_unix(const WallClock *clock) {
    WallClockTimeInfo time_info_ = clock->_time_info;
    return mktime(&time_info_);
}

WallClockTimestamp _hal_wallclock_parse(ConstStr datetime) {
    WallClockTimeInfo time_info_        = VT_STRUCT(WallClockTimeInfo, 0);
    Char              datetime_buf_[32] = {'\0'};

    misc_str_strcpy(datetime_buf_, datetime);

    // process the 'date' part of date-time
    {
        Int32 year_ = 0, month_ = 0, day_ = 0;
        Str   date_part_ = misc_str_strtok(datetime_buf_, " ");

        if (sscanf(date_part_, "%d-%d-%d", &year_, &month_, &day_) != 3)
            return mktime(&time_info_);

        time_info_.tm_year = year_ - 1900; // convert to year since 1900
        time_info_.tm_mon  = month_ - 1;   // convert to 0-based
        time_info_.tm_mday = day_;
    }

    // process the 'time' part of date-time
    {
        Int32 hour_ = 0, minute_ = 0, second_ = 0;
        Str   time_part_ = misc_str_strtok(NULL, " ");

        if (sscanf(time_part_, "%d:%d:%d", &hour_, &minute_, &second_) != 3)
            return mktime(&time_info_);

        time_info_.tm_hour = hour_;
        time_info_.tm_min  = minute_;
        time_info_.tm_sec  = second_;
    }

    return mktime(&time_info_);
}

WallClock hal_wallclock_parse(ConstStr datetime) {
    WallClock clock_ = VT_STRUCT(WallClock, 0);

    clock_._timestamp = _hal_wallclock_parse(datetime);
    THREADSAFE_LOCALTIME(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock hal_wallclock_parse_utc(ConstStr datetime) {
    WallClock clock_ = VT_STRUCT(WallClock, 0);

    clock_._timestamp = _hal_wallclock_parse(datetime);
    THREADSAFE_GMTIME(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClockDate hal_wallclock_getdate(const WallClock *clock) {
    WallClockDate date;

    date._year  = clock->_time_info.tm_year + 1900;
    date._month = clock->_time_info.tm_mon + 1;
    date._day   = clock->_time_info.tm_mday;

    return date;
}

void hal_wallclock_getdate_ymd(const WallClock *clock, Int32 *year, Int32 *month, Int32 *day) {
    if (year != NULL)
        *year = clock->_time_info.tm_year + 1900;

    if (month != NULL)
        *month = clock->_time_info.tm_mon + 1;

    if (day != NULL)
        *day = clock->_time_info.tm_mday;
}

void hal_wallclock_tostring(Str result, const WallClock *clock) {
    Char buf_[64] = {'\0'};
    THREADSAFE_ASCTIME(buf_, sizeof buf_, &(clock->_time_info));
    misc_str_strcpy(result, buf_);
}
