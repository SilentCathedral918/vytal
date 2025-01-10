#include "math_helper.h"

#include <math.h>

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    include <emmintrin.h>
#endif

Flt32 math_sin(Flt32 value) { return sinf(value); }
Flt32 math_cos(Flt32 value) { return cosf(value); }
Flt32 math_tan(Flt32 value) { return tanf(value); }
Flt32 math_asin(Flt32 value) { return asinf(value); }
Flt32 math_acos(Flt32 value) { return acosf(value); }
Flt32 math_atan(Flt32 value) { return atanf(value); }
Flt32 math_atan2(Flt32 x, Flt32 y) { return atan2f(x, y); }
Flt32 math_sqrt(Flt32 value) { return sqrtf(value); }
Flt32 math_rsqrt(Flt32 value) {
#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
    __m128 value_ = _mm_set_ss(value);
    __m128 rsqrt_ = _mm_rsqrt_ss(value_);
    return _mm_cvtss_f32(rsqrt_);

#else
    const Int32 inv_ = 1597463007u - (*VT_CAST(Int32 *, &value) >> 1);
    return *VT_CAST(Flt32 *, &inv_) * (1.5f - (value * 0.5f) * powf(*VT_CAST(Flt32 *, &inv_), 2.0f));

#endif
}
Flt32 math_abs(Flt32 value) { return fabsf(value); }
Flt32 math_exp(Flt32 value) { return expf(value); }
Flt32 math_pow(Flt32 value, Flt32 power) { return powf(value, power); }
Flt32 math_log(Flt32 value) { return logf(value); }
Flt32 math_log_2(Flt32 value) { return log2f(value); }
Flt32 math_log_10(Flt32 value) { return log10f(value); }
Flt32 math_ceil(Flt32 value) { return ceilf(value); }
Flt32 math_floor(Flt32 value) { return floorf(value); }
Flt32 math_round(Flt32 value) { return roundf(value); }
Flt32 math_round_to(Flt32 value, UInt32 decimal_places) { return (decimal_places < 0) ? value : VT_CAST(Flt32, VT_CAST(Int32, value * powf(10.f, VT_CAST(Flt32, decimal_places)) + 0.5f) / powf(10.f, VT_CAST(Flt32, decimal_places))); }
Flt32 math_mod(Flt32 x, Flt32 y) { return fmodf(x, y); }
Int32 math_sign(Flt32 value) { return ((value > 0) - (value < 0)); }
Bool  math_is_power_of_two(UInt64 value) { return ((value > 0) && (!(value & (value - 1)))); }
Bool  math_epsilon_approx(Flt32 left, Flt32 right) { return (fabsf(left - right) < VT_MATH_EPSILON); }
