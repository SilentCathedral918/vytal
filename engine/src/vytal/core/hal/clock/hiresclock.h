#pragma once

#include "vytal/defines/core/clock.h"
#include "vytal/defines/shared.h"

VT_API void  hal_hiresclock_init(HiResClock *clock);
VT_API Flt64 hal_hiresclock_getelapsed_sec(HiResClock *clock);
VT_API Flt64 hal_hiresclock_getfrequency(HiResClock *clock);

VT_API VT_INLINE Flt64 hal_hiresclock_getelapsed_millisec(HiResClock *clock) { return hal_hiresclock_getelapsed_sec(clock) * 1000.0; }
VT_API VT_INLINE Flt64 hal_hiresclock_getelapsed_microsec(HiResClock *clock) { return hal_hiresclock_getelapsed_sec(clock) * 1000000.0; }
VT_API VT_INLINE Flt64 hal_hiresclock_getelapsed_nanosec(HiResClock *clock) { return hal_hiresclock_getelapsed_sec(clock) * 1000000000.0; }
VT_API VT_INLINE Flt64 hal_hiresclock_getfrequency_khz(HiResClock *clock) { return (hal_hiresclock_getfrequency(clock) / 1000.0); }
VT_API VT_INLINE Flt64 hal_hiresclock_getfrequency_mhz(HiResClock *clock) { return (hal_hiresclock_getfrequency(clock) / 1000000.0); }
VT_API VT_INLINE Flt64 hal_hiresclock_getfrequency_ghz(HiResClock *clock) { return (hal_hiresclock_getfrequency(clock) / 1000000000.0); }