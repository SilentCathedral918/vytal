#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Bool   math_rng_initialize(UInt64 seed);
VT_API Bool   math_rng_new_seed(UInt64 new_seed);
VT_API Bool   math_rng_reset(void);
VT_API UInt64 math_rng_get_initial_seed(void);
VT_API UInt64 math_rng_get_current_seed(void);
VT_API Bool   math_rng_random_bool(void);
VT_API Char   math_rng_random_char(Bool uppercase);
VT_API Int32  math_rng_random_int(void);
VT_API Int32  math_rng_random_int_ranged(Int32 min, Int32 max);
VT_API UInt32 math_rng_random_uint(void);
VT_API UInt32 math_rng_random_uint_ranged(UInt32 min, UInt32 max);
VT_API Flt32  math_rng_random_flt32(void);
VT_API Flt32  math_rng_random_flt32_ranged(Flt32 min, Flt32 max);
VT_API Flt64  math_rng_random_flt64(void);
VT_API Flt64  math_rng_random_flt64_ranged(Flt64 min, Flt64 max);
VT_API Flt32  math_rng_random_fraction(Int32 decimal_places);
VT_API Vec2   math_rng_random_vec2(void);
VT_API Vec2   math_rng_random_vec2_ranged(Flt32 min, Flt32 max);
VT_API Vec3   math_rng_random_vec3(void);
VT_API Vec3   math_rng_random_vec3_ranged(Flt32 min, Flt32 max);
VT_API Vec4   math_rng_random_vec4(Bool random_w);
VT_API Vec4   math_rng_random_vec4_ranged(Flt32 min, Flt32 max, Bool random_w);
VT_API Mat4   math_rng_random_mat4(Bool random_w);
VT_API Mat4   math_rng_random_mat4_ranged(Flt32 min, Flt32 max, Bool random_w);
VT_API Quat4  math_rng_random_quat4(Bool random_w);
VT_API Quat4  math_rng_random_quat4_ranged(Flt32 min, Flt32 max, Bool random_w);
