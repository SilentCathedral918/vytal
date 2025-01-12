#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Quat4 math_quat4_construct(Flt32 x, Flt32 y, Flt32 z, Flt32 w);
VT_API Quat4 math_quat4_identity(void);
VT_API Quat4 math_quat4_copy(Quat4 *quat);
VT_API Quat4 math_quat4_convert_from_angles(Flt32 x, Flt32 y, Flt32 z);
VT_API Quat4 math_quat4_convert_from_rotation(Vec3 rotation);
VT_API Quat4 math_quat4_convert_from_axis_angle(Vec3 axis, Flt32 angle, Bool normalize);
VT_API Mat4  math_quat4_convert_to_mat4(Quat4 quat);
VT_API Quat4 math_quat4_add(Quat4 left, Quat4 right);
VT_API Quat4 math_quat4_sub(Quat4 left, Quat4 right);
VT_API Quat4 math_quat4_mul(Quat4 left, Quat4 right);
VT_API Quat4 math_quat4_mul_scalar(Quat4 quat, Flt32 scalar);
VT_API Flt32 math_quat4_dot(Quat4 left, Quat4 right);
VT_API Bool  math_quat4_equals(Quat4 left, Quat4 right);
VT_API Bool  math_quat4_equals_approx(Quat4 left, Quat4 right, Flt32 limit);
VT_API Flt32 math_quat4_magnitude_squared(Quat4 quat);
VT_API Flt32 math_quat4_magnitude(Quat4 quat);
VT_API void  math_quat4_normalize(Quat4 *quat);
VT_API Quat4 math_quat4_normalized(Quat4 quat);
VT_API Quat4 math_quat4_conjugate(Quat4 quat);
VT_API Quat4 math_quat4_quotient(Quat4 left, Quat4 right);
VT_API void  math_quat4_inverse(Quat4 *quat);
VT_API Quat4 math_quat4_inversed(Quat4 quat);
VT_API Quat4 math_quat4_lerp(Quat4 start, Quat4 end, Flt32 time, Bool normalize);
VT_API Quat4 math_quat4_slerp(Quat4 start, Quat4 end, Flt32 time, Bool normalize);
