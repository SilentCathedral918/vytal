#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Vec4  math_vec4_construct(Flt32 x, Flt32 y, Flt32 z, Flt32 w);
VT_API Vec4  math_vec4_zero(void);
VT_API Vec4  math_vec4_one(void);
VT_API Vec4  math_vec4_scale(Flt32 scale);
VT_API Vec4  math_vec4_up(void);
VT_API Vec4  math_vec4_down(void);
VT_API Vec4  math_vec4_left(void);
VT_API Vec4  math_vec4_right(void);
VT_API Vec4  math_vec4_forward(void);
VT_API Vec4  math_vec4_backward(void);
VT_API Vec4  math_vec4_copy(Vec4 *vec);
VT_API Vec4  math_vec4_add(Vec4 left, Vec4 right);
VT_API Vec4  math_vec4_sub(Vec4 left, Vec4 right);
VT_API Vec4  math_vec4_mul(Vec4 left, Vec4 right);
VT_API Vec4  math_vec4_mul_scalar(Vec4 vec, Flt32 scalar);
VT_API Vec4  math_vec4_mul_add(Vec4 vec1, Vec4 vec2, Vec4 vec3);
VT_API Vec4  math_vec4_div(Vec4 left, Vec4 right);
VT_API Vec4  math_vec4_div_scalar(Vec4 vec, Flt32 scalar);
VT_API Flt32 math_vec4_magnitude(Vec4 vec);
VT_API Flt32 math_vec4_magnitude_squared(Vec4 vec);
VT_API void  math_vec4_normalize(Vec4 *vec);
VT_API Vec4  math_vec4_normalized(Vec4 vec);
VT_API Flt32 math_vec4_dot(Vec4 left, Vec4 right);
VT_API Bool  math_vec4_equals(Vec4 left, Vec4 right);
VT_API Bool  math_vec4_equals_approx(Vec4 left, Vec4 right, Flt32 limit);
VT_API Flt32 math_vec4_distance(Vec4 left, Vec4 right);
VT_API Flt32 math_vec4_distance_squared(Vec4 left, Vec4 right);
VT_API Vec4  math_vec4_transform(Vec4 vec, Flt32 w, Mat4 mat);
VT_API Vec4  math_vec4_convert_from_vec3(Vec3 vec, Flt32 w);
VT_API Vec4  math_vec4_lerp(Vec4 start, Vec4 end, Flt32 time);
VT_API Vec4  math_vec4_slerp(Vec4 start, Vec4 end, Flt32 time);
VT_API Vec4  math_vec4_project(Vec4 vec, Vec4 normal);
VT_API Vec4  math_vec4_transform_matrix(Vec4 vec, Mat4 mat);
