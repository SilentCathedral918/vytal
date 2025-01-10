#include "vec3.h"

#include "vytal/core/math/helper/math_helper.h"

Vec3 math_vec3_construct(Flt32 x, Flt32 y, Flt32 z) { return VT_UNION(Vec3, x, y, z); }
Vec3 math_vec3_zero(void) { return VT_UNION(Vec3, 0.0f); }
Vec3 math_vec3_one(void) { return VT_UNION(Vec3, 1.0f); }
Vec3 math_vec3_scale(Flt32 scale) { return VT_UNION(Vec3, scale); }
Vec3 math_vec3_up(void) { return VT_UNION(Vec3, 0.0f, 1.0f, 0.0f); }
Vec3 math_vec3_down(void) { return VT_UNION(Vec3, 0.0f, -1.0f, 0.0f); }
Vec3 math_vec3_left(void) { return VT_UNION(Vec3, -1.0f, 0.0f, 0.0f); }
Vec3 math_vec3_right(void) { return VT_UNION(Vec3, 1.0f, 0.0f, 0.0f); }
Vec3 math_vec3_forward(void) { return VT_UNION(Vec3, 0.0f, 0.0f, 1.0f); }
Vec3 math_vec3_backward(void) { return VT_UNION(Vec3, 0.0f, 0.0f, -1.0f); }
Vec3 math_vec3_copy(Vec3 *vec) { return !vec ? math_vec3_zero() : *vec; }
Vec3 math_vec3_add(Vec3 left, Vec3 right) { return VT_UNION(Vec3, left._elements[0] + right._elements[0], left._elements[1] + right._elements[1], left._elements[2] + right._elements[2]); }
Vec3 math_vec3_sub(Vec3 left, Vec3 right) { return VT_UNION(Vec3, left._elements[0] - right._elements[0], left._elements[1] - right._elements[1], left._elements[2] - right._elements[2]); }
Vec3 math_vec3_mul(Vec3 left, Vec3 right) { return VT_UNION(Vec3, left._elements[0] * right._elements[0], left._elements[1] * right._elements[1], left._elements[2] * right._elements[2]); }
Vec3 math_vec3_mul_scalar(Vec3 vec, Flt32 scalar) { return VT_UNION(Vec3, vec._elements[0] * scalar, vec._elements[1] * scalar, vec._elements[2] * scalar); }
Vec3 math_vec3_mul_add(Vec3 vec1, Vec3 vec2, Vec3 vec3) {
    return VT_UNION(Vec3, vec1._elements[0] * vec2._elements[0] + vec3._elements[0], vec1._elements[1] * vec2._elements[1] + vec3._elements[1], vec1._elements[2] * vec2._elements[2] + vec3._elements[2]);
}
Vec3 math_vec3_div(Vec3 left, Vec3 right) {
    return VT_UNION(Vec3, (right._elements[0] == 0) ? 0 : left._elements[0] / right._elements[0], (right._elements[1] == 0) ? 0 : left._elements[1] / right._elements[1], (right._elements[2] == 0) ? 0 : left._elements[2] / right._elements[2]);
}
Vec3  math_vec3_div_scalar(Vec3 vec, Flt32 scalar) { return VT_UNION(Vec3, (scalar == 0) ? 0 : vec._elements[0] / scalar, (scalar == 0) ? 0 : vec._elements[1] / scalar, (scalar == 0) ? 0 : vec._elements[2] / scalar); }
Flt32 math_vec3_magnitude(Vec3 vec) { return math_sqrt((vec._elements[0] * vec._elements[0]) + (vec._elements[1] * vec._elements[1]) + (vec._elements[2] * vec._elements[2])); }
Flt32 math_vec3_magnitude_squared(Vec3 vec) { return ((vec._elements[0] * vec._elements[0]) + (vec._elements[1] * vec._elements[1]) + (vec._elements[2] * vec._elements[2])); }
void  math_vec3_normalize(Vec3 *vec) {
    if (!vec)
        return;

    Flt32 magnitude_  = math_vec3_magnitude(*vec);
    vec->_elements[0] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[0] / magnitude_;
    vec->_elements[1] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[1] / magnitude_;
    vec->_elements[2] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[2] / magnitude_;
}
Vec3 math_vec3_normalized(Vec3 vec) {
    math_vec3_normalize(&vec);
    return vec;
}
Flt32 math_vec3_dot(Vec3 left, Vec3 right) { return ((left._elements[0] * right._elements[0]) + (left._elements[1] * right._elements[1]) + (left._elements[2] * right._elements[2])); }
Vec3  math_vec3_cross(Vec3 left, Vec3 right) {
    return VT_UNION(Vec3, (left._elements[2] * right._elements[2]) - (left._elements[2] * right._elements[1]), (left._elements[2] * right._elements[0]) - (left._elements[0] * right._elements[2]),
                     (left._elements[0] * right._elements[1]) - (left._elements[1] * right._elements[0]));
}
Bool math_vec3_equals(Vec3 left, Vec3 right) {
    return (math_abs(left._elements[0] - right._elements[0]) < VT_MATH_EPSILON) && (math_abs(left._elements[1] - right._elements[1]) < VT_MATH_EPSILON) && (math_abs(left._elements[2] - right._elements[2]) < VT_MATH_EPSILON);
}
Bool math_vec3_equals_approx(Vec3 left, Vec3 right, Flt32 limit) {
    return (math_abs(left._elements[0] - right._elements[0]) <= limit) && (math_abs(left._elements[1] - right._elements[1]) <= limit) && (math_abs(left._elements[2] - right._elements[2]) <= limit);
}
Flt32 math_vec3_distance(Vec3 left, Vec3 right) { return math_vec3_magnitude(math_vec3_sub(left, right)); }
Flt32 math_vec3_distance_squared(Vec3 left, Vec3 right) { return math_vec3_magnitude_squared(math_vec3_sub(left, right)); }
Vec3  math_vec3_transform(Vec3 vec, Flt32 w, Mat4 mat) {
    return VT_UNION(Vec3, (vec._elements[0] * mat._elements[0 + 0]) + (vec._elements[1] * mat._elements[4 + 0]) + (vec._elements[2] * mat._elements[8 + 0]) + (w * mat._elements[12 + 0]),
                     (vec._elements[0] * mat._elements[0 + 1]) + (vec._elements[1] * mat._elements[4 + 1]) + (vec._elements[2] * mat._elements[8 + 1]) + (w * mat._elements[12 + 1]),
                     (vec._elements[0] * mat._elements[0 + 2]) + (vec._elements[1] * mat._elements[4 + 2]) + (vec._elements[2] * mat._elements[8 + 2]) + (w * mat._elements[12 + 2]));
}
Vec3 math_vec3_convert_from_vec4(Vec4 vec) { return VT_UNION(Vec3, vec._elements[0], vec._elements[1], vec._elements[2]); }
Vec3 math_vec3_lerp(Vec3 start, Vec3 end, Flt32 time) { return math_vec3_add(math_vec3_mul_scalar(start, 1.0f - time), math_vec3_mul_scalar(end, time)); }
Vec3 math_vec3_slerp(Vec3 start, Vec3 end, Flt32 time) {
    Flt32 dot_ = VT_MATH_CLAMP(math_vec3_dot(start, end), -1.0f, 1.0f);
    if (math_abs(dot_ - 1.0f) < VT_MATH_EPSILON)
        return start;

    Flt32 theta_ = math_acos(dot_) * time;
    Vec3  vec_   = math_vec3_sub(end, math_vec3_mul_scalar(start, dot_));
    return math_vec3_add(math_vec3_mul_scalar(start, math_cos(theta_)), math_vec3_mul_scalar(vec_, math_sin(theta_)));
}
Vec3 math_vec3_project(Vec3 vec, Vec3 normal) { return math_vec3_mul_scalar(normal, math_vec3_dot(vec, normal) / math_vec3_dot(normal, normal)); }
Vec3 math_vec3_reflect(Vec3 vec, Vec3 normal) { return math_vec3_sub(vec, math_vec3_mul_scalar(normal, math_vec3_dot(vec, normal) * 2.0f)); }
Vec3 math_vec3_transform_matrix(Vec3 vec, Mat3 mat) {
    return VT_UNION(Vec3, mat._x1y1 * vec._elements[0] + mat._x2y1 * vec._elements[1] + mat._x3y1 * vec._elements[2], mat._x1y2 * vec._elements[0] + mat._x2y2 * vec._elements[1] + mat._x3y2 * vec._elements[2],
                    mat._x1y3 * vec._elements[0] + mat._x2y3 * vec._elements[1] + mat._x3y3 * vec._elements[2]);
}
