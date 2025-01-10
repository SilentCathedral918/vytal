#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Flt32 math_sin(Flt32 value);
VT_API Flt32 math_cos(Flt32 value);
VT_API Flt32 math_tan(Flt32 value);
VT_API Flt32 math_asin(Flt32 value);
VT_API Flt32 math_acos(Flt32 value);
VT_API Flt32 math_atan(Flt32 value);
VT_API Flt32 math_atan2(Flt32 x, Flt32 y);
VT_API Flt32 math_sqrt(Flt32 value);
VT_API Flt32 math_rsqrt(Flt32 value);
VT_API Flt32 math_abs(Flt32 value);
VT_API Flt32 math_exp(Flt32 value);
VT_API Flt32 math_pow(Flt32 value, Flt32 power);
VT_API Flt32 math_log(Flt32 value);
VT_API Flt32 math_log_2(Flt32 value);
VT_API Flt32 math_log_10(Flt32 value);
VT_API Flt32 math_ceil(Flt32 value);
VT_API Flt32 math_floor(Flt32 value);
VT_API Flt32 math_round(Flt32 value);
VT_API Flt32 math_round_to(Flt32 value, UInt32 decimal_places);
VT_API Flt32 math_mod(Flt32 x, Flt32 y);
VT_API Int32 math_sign(Flt32 value);
VT_API Bool  math_is_power_of_two(UInt64 value);
VT_API Bool  math_epsilon_approx(Flt32 left, Flt32 right);
