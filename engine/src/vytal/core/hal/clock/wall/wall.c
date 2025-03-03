#include "wall.h"

#include <stdio.h>

#if defined(_WIN64) || defined(__MINGW64__)
#    define threadsafe_localtime(time_info, timestamp) localtime_s(time_info, timestamp)
#    define threadsafe_gmtime(time_info, timestamp) gmtime_s(time_info, timestamp)
#    define threadsafe_asctime(buf, sz, time_info) asctime_s(buf, sz, time_info)

#else

#    define threadsafe_localtime(time_info, timestamp) localtime_r(time_info, timestamp)
#    define threadsafe_gmtime(time_info, timestamp) gmtime_r(time_info, timestamp)
#    define threadsafe_asctime(buf, sz, time_info) asctime_r(buf, sz, time_info)

#endif

WallClockTimeInfo _clock_wall_timeinfo_from_julian(const Flt64 julian) {
#define DATE_OFFSET 68569
#define EPOCH_OFFSET 49
#define DAYS_IN_400_YEARS 146097
#define DAYS_IN_4_YEARS 1461
#define MONTHS_IN_YEAR 12
#define AVERAGE_DAYS_IN_MONTH 30.4375
#define DAYS_IN_MONTHS (AVERAGE_DAYS_IN_MONTH * 80)
#define BASE_YEAR 1900

    Int64 l_ = (Int64)julian + DATE_OFFSET;  // align with Gregorian date
    Int64 n_ = 4 * l_ / DAYS_IN_400_YEARS;
    l_       = l_ - (DAYS_IN_400_YEARS * n_ + 3) / 4;
    Int64 i_ = 4000 * (l_ + 1) / 1461001;
    l_       = l_ - DAYS_IN_4_YEARS * i_ / 4 + 31;
    Int64 j_ = 80 * l_ / DAYS_IN_MONTHS;
    Int64 k_ = l_ - DAYS_IN_MONTHS * j_ / 80;
    l_       = j_ / 11;
    j_       = j_ + 2 - MONTHS_IN_YEAR * l_;
    i_       = 100 * (n_ - EPOCH_OFFSET) + i_ + l_;

    WallClockTimeInfo time_info_ = {0};
    time_info_.tm_year           = i_ - BASE_YEAR;  // convert to year since 1900
    time_info_.tm_mon            = j_ - 1;          // convert to 0-based month index
    time_info_.tm_mday           = k_;
    time_info_.tm_hour           = 0;
    time_info_.tm_min            = 0;
    time_info_.tm_sec            = 0;

    return time_info_;
}

WallClockTimestamp _clock_wall_parse_datetime(ConstStr datetime) {
    WallClockTimeInfo time_info_        = {0};
    Char              datetime_buf_[32] = {'\0'};

    strncpy(datetime_buf_, datetime, sizeof(datetime_buf_) - 1);
    datetime_buf_[sizeof(datetime_buf_) - 1] = '\0';

    // check for datetime validity
    if (datetime == NULL || strlen(datetime) < 10) return -1;  // invalid date

    // process the 'date' part of date-time
    {
        Int32 year_ = 0, month_ = 0, day_ = 0;
        Str   date_part_ = strtok(datetime_buf_, " ");

        if (!date_part_ || (sscanf(date_part_, "%d-%d-%d", &year_, &month_, &day_) != 3))
            return mktime(&time_info_);

        time_info_.tm_year = year_ - 1900;  // convert to year since 1900
        time_info_.tm_mon  = month_ - 1;    // convert to 0-based
        time_info_.tm_mday = day_;
    }

    // process the 'time' part of date-time
    {
        Int32 hour_ = 0, minute_ = 0, second_ = 0;
        Str   time_part_ = strtok(NULL, " ");

        if (sscanf(time_part_, "%d:%d:%d", &hour_, &minute_, &second_) != 3)
            return mktime(&time_info_);

        time_info_.tm_hour = hour_;
        time_info_.tm_min  = minute_;
        time_info_.tm_sec  = second_;
    }

    return mktime(&time_info_);
}

WallClock clock_wall_now(void) {
    WallClock clock_;

    clock_._timestamp = time(NULL);
    if (clock_._timestamp == (time_t)(-1)) {
        memset(&clock_, 0, sizeof(WallClock));
        return clock_;
    }

    threadsafe_localtime(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock clock_wall_now_utc(void) {
    WallClock clock_;

    clock_._timestamp = time(NULL);
    threadsafe_gmtime(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock clock_wall_today(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    threadsafe_localtime(&(clock_._time_info), &(clock_._timestamp));

    // set time to 00:00 AM (midnight)
    {
        clock_._time_info.tm_hour = 0;
        clock_._time_info.tm_min  = 0;
        clock_._time_info.tm_sec  = 0;
    }

    return clock_;
}

WallClock clock_wall_today_utc(void) {
    WallClock clock_;
    clock_._timestamp = time(NULL);
    threadsafe_gmtime(&(clock_._time_info), &(clock_._timestamp));

    // set time to 00:00 AM (midnight)
    {
        clock_._time_info.tm_hour = 0;
        clock_._time_info.tm_min  = 0;
        clock_._time_info.tm_sec  = 0;
    }

    return clock_;
}

WallClock clock_wall_from_julian(const Flt64 julian) {
    WallClock clock_;

    clock_._time_info = _clock_wall_timeinfo_from_julian(julian);
    clock_._timestamp = mktime(&(clock_._time_info));

    return clock_;
}

Int64 clock_wall_to_julian(const WallClock *clock) {
    return (Int64)(clock->_timestamp / 86400.0 + 2440587.5);
}

WallClock clock_wall_from_unix(Int64 unix) {
    WallClock clock_;

    clock_._timestamp = unix;
    threadsafe_gmtime(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

Int64 clock_wall_to_unix(const WallClock *clock) {
    WallClockTimeInfo time_info_ = clock->_time_info;
    time_info_.tm_isdst          = -1;

    return mktime(&time_info_);
}

WallClock clock_wall_parse_datetime(ConstStr datetime) {
    WallClock clock_;

    clock_._timestamp = _clock_wall_parse_datetime(datetime);
    threadsafe_localtime(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClock clock_wall_parse_datetime_utc(ConstStr datetime) {
    WallClock clock_;

    clock_._timestamp = _clock_wall_parse_datetime(datetime);
    threadsafe_gmtime(&(clock_._time_info), &(clock_._timestamp));

    return clock_;
}

WallClockDate clock_wall_getdate(const WallClock *clock) {
    WallClockDate date = {
        ._year  = clock->_time_info.tm_year + 1900,
        ._month = clock->_time_info.tm_mon + 1,
        ._day   = clock->_time_info.tm_mday};

    return date;
}

void clock_wall_getdate_ymd(const WallClock *clock, Int32 *year, Int32 *month, Int32 *day) {
    if (year != NULL)
        *year = clock->_time_info.tm_year + 1900;

    if (month != NULL)
        *month = clock->_time_info.tm_mon + 1;

    if (day != NULL)
        *day = clock->_time_info.tm_mday;
}

void clock_wall_tostring(Str result, const WallClock *clock) {
    strftime(result, 64, "%Y-%m-%d %H:%M:%S", &(clock->_time_info));
}

Int32 clock_wall_get_year(const WallClock *clock) {
    return clock->_time_info.tm_year + 1900;
}

Int32 clock_wall_get_month(const WallClock *clock) {
    return clock->_time_info.tm_mon + 1;
}

MonthOfYear clock_wall_get_monthofyear(const WallClock *clock) {
    return (MonthOfYear)clock_wall_get_month(clock);
}

Int32 clock_wall_get_day(const WallClock *clock) {
    return clock->_time_info.tm_mday;
}

DayOfWeek clock_wall_get_day_of_week(const WallClock *clock) {
    return clock->_time_info.tm_wday;
}

Int32 clock_wall_get_day_of_year(const WallClock *clock) {
    return clock->_time_info.tm_yday + 1;
}

Int32 clock_wall_get_hour_12(const WallClock *clock) {
    return (clock->_time_info.tm_hour % 12 == 0) ? 12 : clock->_time_info.tm_hour % 12;
}

Int32 clock_wall_get_hour_24(const WallClock *clock) {
    return clock->_time_info.tm_hour;
}

Int32 clock_wall_get_minute(const WallClock *clock) {
    return clock->_time_info.tm_min;
}

Int32 clock_wall_get_second(const WallClock *clock) {
    return clock->_time_info.tm_sec;
}

Bool clock_wall_is_am(const WallClock *clock) {
    return (clock->_time_info.tm_hour >= 0) && (clock->_time_info.tm_hour < 12);
}

Bool clock_wall_is_pm(const WallClock *clock) {
    return !clock_wall_is_am(clock);
}

Bool clock_wall_is_leap_year(const WallClock *clock) {
    return ((clock->_time_info.tm_year % 4 == 0) && (clock->_time_info.tm_year % 100 != 0)) || (clock->_time_info.tm_year % 400 == 0);
}
