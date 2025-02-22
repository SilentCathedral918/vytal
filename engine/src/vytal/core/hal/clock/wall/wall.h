#pragma once

#include "vytal/defines/core/clock.h"
#include "vytal/defines/shared.h"

VYTAL_API WallClock     clock_wall_now(void);
VYTAL_API WallClock     clock_wall_now_utc(void);
VYTAL_API WallClock     clock_wall_today(void);
VYTAL_API WallClock     clock_wall_today_utc(void);
VYTAL_API WallClock     clock_wall_from_julian(const Flt64 julian);
VYTAL_API Int64         clock_wall_to_julian(const WallClock *clock);
VYTAL_API WallClock     clock_wall_from_unix(Int64 unix);
VYTAL_API Int64         clock_wall_to_unix(const WallClock *clock);
VYTAL_API WallClock     clock_wall_parse_datetime(ConstStr datetime);
VYTAL_API WallClock     clock_wall_parse_datetime_utc(ConstStr datetime);
VYTAL_API WallClockDate clock_wall_getdate(const WallClock *clock);
VYTAL_API void          clock_wall_getdate_ymd(const WallClock *clock, Int32 *year, Int32 *month, Int32 *day);
VYTAL_API void          clock_wall_tostring(Str result, const WallClock *clock);
VYTAL_API Int32         clock_wall_get_year(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_month(const WallClock *clock);
VYTAL_API MonthOfYear   clock_wall_get_month_of_year(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_day(const WallClock *clock);
VYTAL_API DayOfWeek     clock_wall_get_day_of_week(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_day_of_year(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_hour_12(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_hour_24(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_minute(const WallClock *clock);
VYTAL_API Int32         clock_wall_get_second(const WallClock *clock);
VYTAL_API Bool          clock_wall_is_am(const WallClock *clock);
VYTAL_API Bool          clock_wall_is_pm(const WallClock *clock);
VYTAL_API Bool          clock_wall_is_leap_year(const WallClock *clock);
