#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Vec3  math_vec3_construct(Flt32 x, Flt32 y, Flt32 z);
VT_API Vec3  math_vec3_zero(void);
VT_API Vec3  math_vec3_one(void);
VT_API Vec3  math_vec3_scale(Flt32 scale);
VT_API Vec3  math_vec3_up(void);
VT_API Vec3  math_vec3_down(void);
VT_API Vec3  math_vec3_left(void);
VT_API Vec3  math_vec3_right(void);
VT_API Vec3  math_vec3_forward(void);
VT_API Vec3  math_vec3_backward(void);
VT_API Vec3  math_vec3_copy(Vec3 *vec);
VT_API Vec3  math_vec3_add(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_sub(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_mul(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_mul_scalar(Vec3 vec, Flt32 scalar);
VT_API Vec3  math_vec3_mul_add(Vec3 vec1, Vec3 vec2, Vec3 vec3);
VT_API Vec3  math_vec3_div(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_div_scalar(Vec3 vec, Flt32 scalar);
VT_API Flt32 math_vec3_magnitude(Vec3 vec);
VT_API Flt32 math_vec3_magnitude_squared(Vec3 vec);
VT_API void  math_vec3_normalize(Vec3 *vec);
VT_API Vec3  math_vec3_normalized(Vec3 vec);
VT_API Flt32 math_vec3_dot(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_cross(Vec3 left, Vec3 right);
VT_API Bool  math_vec3_equals(Vec3 left, Vec3 right);
VT_API Bool  math_vec3_equals_approx(Vec3 left, Vec3 right, Flt32 limit);
VT_API Flt32 math_vec3_distance(Vec3 left, Vec3 right);
VT_API Flt32 math_vec3_distance_squared(Vec3 left, Vec3 right);
VT_API Vec3  math_vec3_transform(Vec3 vec, Flt32 w, Mat4 mat);
VT_API Vec3  math_vec3_convert_from_vec4(Vec4 vec);
VT_API Vec3  math_vec3_lerp(Vec3 start, Vec3 end, Flt32 time);
VT_API Vec3  math_vec3_slerp(Vec3 start, Vec3 end, Flt32 time);
VT_API Vec3  math_vec3_project(Vec3 vec, Vec3 normal);
VT_API Vec3  math_vec3_reflect(Vec3 vec, Vec3 normal);
VT_API Vec3  math_vec3_transform_matrix(Vec3 vec, Mat3 mat);
