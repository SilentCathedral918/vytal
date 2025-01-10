#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Mat4  math_mat4_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x3y1, Flt32 x4y1, Flt32 x1y2, Flt32 x2y2, Flt32 x3y2, Flt32 x4y2, Flt32 x1y3, Flt32 x2y3, Flt32 x3y3, Flt32 x4y3, Flt32 x1y4, Flt32 x2y4, Flt32 x3y4, Flt32 x4y4);
VT_API Mat4  math_mat4_zero(void);
VT_API Mat4  math_mat4_identity(void);
VT_API Mat4  math_mat4_copy(Mat4 *mat);
VT_API Mat4  math_mat4_from_vectors(Vec3 vec1, Vec3 vec2);
VT_API Mat4  math_mat4_add(Mat4 *left, Mat4 *right);
VT_API Mat4  math_mat4_sub(Mat4 *left, Mat4 *right);
VT_API Mat4  math_mat4_mul(Mat4 *left, Mat4 *right);
VT_API Mat4  math_mat4_mul_scalar(Mat4 *mat, Flt32 scalar);
VT_API Mat4  math_mat4_orthographic(Flt32 top, Flt32 bottom, Flt32 left, Flt32 right, Flt32 near, Flt32 far);
VT_API Mat4  math_mat4_perspective(Flt32 rad_fov, Flt32 ratio, Flt32 near, Flt32 far);
VT_API Mat4  math_mat4_look_at(Vec3 position, Vec3 target, Vec3 up);
VT_API void  math_mat4_transpose(Mat4 *mat);
VT_API Mat4  math_mat4_transposed(Mat4 *mat);
VT_API Flt32 math_mat4_determinant(Mat4 *mat);
VT_API void  math_mat4_inverse(Mat4 *mat);
VT_API Mat4  math_mat4_inversed(Mat4 *mat);
VT_API Mat4  math_mat4_make_translation(Vec3 position);
VT_API Mat4  math_mat4_make_rotation_x(Flt32 rad_angle);
VT_API Mat4  math_mat4_make_rotation_y(Flt32 rad_angle);
VT_API Mat4  math_mat4_make_rotation_z(Flt32 rad_angle);
VT_API Mat4  math_mat4_make_rotation_xyz(Flt32 rad_x, Flt32 rad_y, Flt32 rad_z);
VT_API Mat4  math_mat4_make_scale(Vec3 scale);
VT_API Mat4  math_mat4_transform(Vec3 translation, Vec3 rotation, Vec3 scale);
VT_API Vec3  math_mat4_make_forward_vector(Mat4 *mat);
VT_API Vec3  math_mat4_make_backward_vector(Mat4 *mat);
VT_API Vec3  math_mat4_make_up_vector(Mat4 *mat);
VT_API Vec3  math_mat4_make_down_vector(Mat4 *mat);
VT_API Vec3  math_mat4_make_left_vector(Mat4 *mat);
VT_API Vec3  math_mat4_make_right_vector(Mat4 *mat);
VT_API Vec3  math_mat4_mat4_mul_vec3(Mat4 *mat, Vec3 *vec);
VT_API Vec3  math_mat4_vec3_mul_mat4(Vec3 *vec, Mat4 *mat);
VT_API Vec4  math_mat4_mat4_mul_vec4(Mat4 *mat, Vec4 *vec);
VT_API Vec4  math_mat4_vec4_mul_mat4(Vec4 *vec, Mat4 *mat);
VT_API Vec4  math_mat4_get_row(Mat4 *mat, Int32 row);
VT_API Vec4  math_mat4_get_column(Mat4 *mat, Int32 column);
VT_API void  math_mat4_set_row(Mat4 *mat, Int32 row, Vec4 *data);
VT_API void  math_mat4_set_column(Mat4 *mat, Int32 column, Vec4 *data);
VT_API Mat3  math_mat4_get_topleft_mat3(Mat4 *mat);
VT_API void  math_mat4_set_topleft_mat3(Mat4 *mat, Mat3 *data);
VT_API Bool  math_mat4_equals(Mat4 *left, Mat4 *right);
VT_API Bool  math_mat4_equals_approx(Mat4 *left, Mat4 *right, Flt32 limit);
VT_API Bool  math_mat4_is_zero(Mat4 *mat);
VT_API Bool  math_mat4_is_identity(Mat4 *mat);
