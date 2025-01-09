#pragma once

#include "vytal/defines/core/clock.h"
#include "vytal/defines/shared.h"

VT_API WallClock     hal_wallclock_now(void);
VT_API WallClock     hal_wallclock_now_utc(void);
VT_API WallClock     hal_wallclock_today(void);
VT_API WallClock     hal_wallclock_today_utc(void);
VT_API WallClock     hal_wallclock_from_julian(const Flt64 julian);
VT_API Int64         hal_wallclock_to_julian(const WallClock *clock);
VT_API WallClock     hal_wallclock_from_unix(Int64 unix);
VT_API Int64         hal_wallclock_to_unix(const WallClock *clock);
VT_API WallClock     hal_wallclock_parse(ConstStr datetime);
VT_API WallClock     hal_wallclock_parse_utc(ConstStr datetime);
VT_API WallClockDate hal_wallclock_getdate(const WallClock *clock);
VT_API void          hal_wallclock_getdate_ymd(const WallClock *clock, Int32 *year, Int32 *month, Int32 *day);
VT_API void          hal_wallclock_tostring(Str result, const WallClock *clock);

VT_API VT_INLINE Int32       hal_wallclock_getyear(const WallClock *clock) { return clock->_time_info.tm_year + 1900; }
VT_API VT_INLINE Int32       hal_wallclock_getmonth(const WallClock *clock) { return clock->_time_info.tm_mon + 1; }
VT_API VT_INLINE MonthOfYear hal_wallclock_getmonthofyear(const WallClock *clock) { return VT_CAST(MonthOfYear, hal_wallclock_getmonth(clock)); }
VT_API VT_INLINE Int32       hal_wallclock_getday(const WallClock *clock) { return clock->_time_info.tm_mday; }
VT_API VT_INLINE DayOfWeek   hal_wallclock_getdayofweek(const WallClock *clock) { return clock->_time_info.tm_wday; }
VT_API VT_INLINE Int32       hal_wallclock_getdayofyear(const WallClock *clock) { return clock->_time_info.tm_yday + 1; }
VT_API VT_INLINE Int32       hal_wallclock_gethour12(const WallClock *clock) { return (clock->_time_info.tm_hour > 12) ? (clock->_time_info.tm_hour) % 12 : (clock->_time_info.tm_hour == 0) ? 12 : (clock->_time_info.tm_hour); }
VT_API VT_INLINE Int32       hal_wallclock_gethour24(const WallClock *clock) { return clock->_time_info.tm_hour; }
VT_API VT_INLINE Int32       hal_wallclock_getminute(const WallClock *clock) { return clock->_time_info.tm_min; }
VT_API VT_INLINE Int32       hal_wallclock_getsecond(const WallClock *clock) { return clock->_time_info.tm_sec; }
VT_API VT_INLINE Bool        hal_wallclock_isam(const WallClock *clock) { return (clock->_time_info.tm_hour >= 0) && (clock->_time_info.tm_hour < 12); }
VT_API VT_INLINE Bool        hal_wallclock_ispm(const WallClock *clock) { return !hal_wallclock_isam(clock); }
VT_API VT_INLINE Bool        hal_wallclock_isleapyear(const WallClock *clock) { return ((clock->_time_info.tm_year % 4 == 0) && (clock->_time_info.tm_year % 100 != 0)) || (clock->_time_info.tm_year % 400 == 0); }
