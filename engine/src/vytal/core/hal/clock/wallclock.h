#pragma once

#include "vytal/defines/core/clock.h"
#include "vytal/defines/shared.h"

VT_API WallClock hal_wallclock_now(void);
VT_API WallClock hal_wallclock_now_utc(void);
VT_API WallClock hal_wallclock_today(void);
