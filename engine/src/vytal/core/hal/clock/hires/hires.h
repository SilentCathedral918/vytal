#pragma once

#include "vytal/defines/core/clock.h"
#include "vytal/defines/shared.h"

VYTAL_API void  clock_hires_init(HiResClock *clock);
VYTAL_API Flt64 clock_hires_elapsed_seconds(HiResClock *clock);
VYTAL_API Flt64 clock_hires_elapsed_milliseconds(HiResClock *clock);
VYTAL_API Flt64 clock_hires_elapsed_microseconds(HiResClock *clock);
VYTAL_API Flt64 clock_hires_elapsed_nanoseconds(HiResClock *clock);
VYTAL_API Flt64 clock_hires_frequency(HiResClock *clock);
VYTAL_API Flt64 clock_hires_frequency_khz(HiResClock *clock);
VYTAL_API Flt64 clock_hires_frequency_mhz(HiResClock *clock);
VYTAL_API Flt64 clock_hires_frequency_ghz(HiResClock *clock);
