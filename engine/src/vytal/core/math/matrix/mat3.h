#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Mat3  math_mat3_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x3y1, Flt32 x1y2, Flt32 x2y2, Flt32 x3y2, Flt32 x1y3, Flt32 x2y3, Flt32 x3y3);
VT_API Mat3  math_mat3_zero(void);
VT_API Mat3  math_mat3_identity(void);
VT_API Mat3  math_mat3_copy(Mat3 *mat);
VT_API Mat3  math_mat3_from_vectors(Vec2 vec1, Vec2 vec2);
VT_API Mat3  math_mat3_add(Mat3 *left, Mat3 *right);
VT_API Mat3  math_mat3_sub(Mat3 *left, Mat3 *right);
VT_API Mat3  math_mat3_mul(Mat3 *left, Mat3 *right);
VT_API Mat3  math_mat3_mul_scalar(Mat3 *mat, Flt32 scalar);
VT_API Vec2  math_mat3_mat3_mul_vec2(Mat3 *mat, Vec2 *vec);
VT_API Vec2  math_mat3_vec2_mul_mat3(Vec2 *vec, Mat3 *mat);
VT_API void  math_mat3_transpose(Mat3 *mat);
VT_API Mat3  math_mat3_transposed(Mat3 *mat);
VT_API Flt32 math_mat3_determinant(Mat3 *mat);
VT_API void  math_mat3_inverse(Mat3 *mat);
VT_API Mat3  math_mat3_inversed(Mat3 *mat);
VT_API Mat3  math_mat3_make_translation(Vec2 position);
VT_API Mat3  math_mat3_make_rotation(Flt32 rad_angle);
VT_API Mat3  math_mat3_make_scale(Vec2 scale);
VT_API Vec3  math_mat3_get_row(Mat3 *mat, Int32 row);
VT_API Vec3  math_mat3_get_column(Mat3 *mat, Int32 column);
VT_API void  math_mat3_set_row(Mat3 *mat, Int32 row, Vec3 *data);
VT_API void  math_mat3_set_column(Mat3 *mat, Int32 column, Vec3 *data);
VT_API Bool  math_mat3_equals(Mat3 *left, Mat3 *right);
VT_API Bool  math_mat3_equals_approx(Mat3 *left, Mat3 *right, Flt32 limit);
VT_API Bool  math_mat3_is_zero(Mat3 *mat);
VT_API Bool  math_mat3_is_identity(Mat3 *mat);
