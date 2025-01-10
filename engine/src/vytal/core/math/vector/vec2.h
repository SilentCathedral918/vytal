#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Vec2  math_vec2_construct(Flt32 x, Flt32 y);
VT_API Vec2  math_vec2_zero(void);
VT_API Vec2  math_vec2_one(void);
VT_API Vec2  math_vec2_scale(Flt32 scale);
VT_API Vec2  math_vec2_up(void);
VT_API Vec2  math_vec2_down(void);
VT_API Vec2  math_vec2_left(void);
VT_API Vec2  math_vec2_right(void);
VT_API Vec2  math_vec2_copy(Vec2 *vec);
VT_API Vec2  math_vec2_add(Vec2 left, Vec2 right);
VT_API Vec2  math_vec2_sub(Vec2 left, Vec2 right);
VT_API Vec2  math_vec2_mul(Vec2 left, Vec2 right);
VT_API Vec2  math_vec2_mul_scalar(Vec2 vec, Flt32 scalar);
VT_API Vec2  math_vec2_mul_add(Vec2 a, Vec2 b, Vec2 c);
VT_API Vec2  math_vec2_div(Vec2 left, Vec2 right);
VT_API Vec2  math_vec2_div_scalar(Vec2 vec, Flt32 scalar);
VT_API Flt32 math_vec2_magnitude(Vec2 vec);
VT_API Flt32 math_vec2_magnitude_squared(Vec2 vec);
VT_API void  math_vec2_normalize(Vec2 *vec);
VT_API Vec2  math_vec2_normalized(Vec2 vec);
VT_API Bool  math_vec2_equals(Vec2 left, Vec2 right);
VT_API Bool  math_vec2_equals_approx(Vec2 left, Vec2 right, Flt32 limit);
VT_API Flt32 math_vec2_distance(Vec2 left, Vec2 right);
VT_API Flt32 math_vec2_distance_squared(Vec2 left, Vec2 right);
VT_API Flt32 math_vec2_dot(Vec2 left, Vec2 right);
VT_API Flt32 math_vec2_cross(Vec2 left, Vec2 right);
VT_API Vec2  math_vec2_lerp(Vec2 start, Vec2 end, Flt32 time);
VT_API Vec2  math_vec2_slerp(Vec2 start, Vec2 end, Flt32 time);
VT_API Vec2  math_vec2_project(Vec2 vec, Vec2 normal);
VT_API Vec2  math_vec2_reflect(Vec2 vec, Vec2 normal);
VT_API Vec2  math_vec2_transform_matrix(Vec2 vec, Mat2 mat);
