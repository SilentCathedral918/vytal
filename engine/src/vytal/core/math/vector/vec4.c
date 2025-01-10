#include "vec4.h"

#include "vytal/core/math/helper/math_helper.h"

Vec4 math_vec4_construct(Flt32 x, Flt32 y, Flt32 z, Flt32 w) { return VT_UNION(Vec4, x, y, z, w); }
Vec4 math_vec4_zero(void) { return VT_UNION(Vec4, 0.0f); }
Vec4 math_vec4_one(void) { return VT_UNION(Vec4, 1.0f); }
Vec4 math_vec4_scale(Flt32 scale) { return VT_UNION(Vec4, scale); }
Vec4 math_vec4_up(void) { return VT_UNION(Vec4, 0.0f, 1.0f, 0.0f, 0.0f); }
Vec4 math_vec4_down(void) { return VT_UNION(Vec4, 0.0f, -1.0f, 0.0f, 0.0f); }
Vec4 math_vec4_left(void) { return VT_UNION(Vec4, -1.0f, 0.0f, 0.0f, 0.0f); }
Vec4 math_vec4_right(void) { return VT_UNION(Vec4, 1.0f, 0.0f, 0.0f, 0.0f); }
Vec4 math_vec4_forward(void) { return VT_UNION(Vec4, 0.0f, 0.0f, 1.0f, 0.0f); }
Vec4 math_vec4_backward(void) { return VT_UNION(Vec4, 0.0f, 0.0f, -1.0f, 0.0f); }
Vec4 math_vec4_copy(Vec4 *vec) { return !vec ? math_vec4_zero() : *vec; }
Vec4 math_vec4_add(Vec4 left, Vec4 right) { return VT_UNION(Vec4, left._elements[0] + right._elements[0], left._elements[1] + right._elements[1], left._elements[2] + right._elements[2], left._elements[3] + right._elements[3]); }
Vec4 math_vec4_sub(Vec4 left, Vec4 right) { return VT_UNION(Vec4, left._elements[0] - right._elements[0], left._elements[1] - right._elements[1], left._elements[2] - right._elements[2], left._elements[3] - right._elements[3]); }
Vec4 math_vec4_mul(Vec4 left, Vec4 right) { return VT_UNION(Vec4, left._elements[0] * right._elements[0], left._elements[1] * right._elements[1], left._elements[2] * right._elements[2], left._elements[3] * right._elements[3]); }
Vec4 math_vec4_mul_scalar(Vec4 vec, Flt32 scalar) { return VT_UNION(Vec4, vec._elements[0] * scalar, vec._elements[1] * scalar, vec._elements[2] * scalar, vec._elements[3] * scalar); }
Vec4 math_vec4_mul_add(Vec4 vec1, Vec4 vec2, Vec4 vec3) {
    return VT_UNION(Vec4, vec1._elements[0] * vec2._elements[0] + vec3._elements[0], vec1._elements[1] * vec2._elements[1] + vec3._elements[1], vec1._elements[2] * vec2._elements[2] + vec3._elements[2],
                    vec1._elements[3] * vec2._elements[3] + vec3._elements[3]);
}
Vec4 math_vec4_div(Vec4 left, Vec4 right) {
    return VT_UNION(Vec4, (right._elements[0] == 0) ? 0.0f : left._elements[0] / right._elements[0], (right._elements[1] == 0) ? 0.0f : left._elements[1] / right._elements[1], (right._elements[2] == 0) ? 0.0f : left._elements[2] / right._elements[2],
                    (right._elements[3] == 0) ? 0.0f : left._elements[3] / right._elements[3]);
}
Vec4 math_vec4_div_scalar(Vec4 vec, Flt32 scalar) {
    return VT_UNION(Vec4, (scalar == 0) ? 0.0f : vec._elements[0] / scalar, (scalar == 0) ? 0.0f : vec._elements[1] / scalar, (scalar == 0) ? 0.0f : vec._elements[2] / scalar, (scalar == 0) ? 0.0f : vec._elements[3] / scalar);
}
Flt32 math_vec4_magnitude(Vec4 vec) { return math_sqrt((vec._elements[0] * vec._elements[0]) + (vec._elements[1] * vec._elements[1]) + (vec._elements[2] * vec._elements[2]) + (vec._elements[3] * vec._elements[3])); }
Flt32 math_vec4_magnitude_squared(Vec4 vec) { return ((vec._elements[0] * vec._elements[0]) + (vec._elements[1] * vec._elements[1]) + (vec._elements[2] * vec._elements[2]) + (vec._elements[3] * vec._elements[3])); }
void  math_vec4_normalize(Vec4 *vec) {
    if (!vec)
        return;

    Flt32 magnitude_  = math_vec4_magnitude(*vec);
    vec->_elements[0] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[0] / magnitude_;
    vec->_elements[1] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[1] / magnitude_;
    vec->_elements[2] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[2] / magnitude_;
    vec->_elements[3] = (magnitude_ == 0.0f) ? 0.0f : vec->_elements[3] / magnitude_;
}
Vec4 math_vec4_normalized(Vec4 vec) {
    math_vec4_normalize(&vec);
    return vec;
}
Flt32 math_vec4_dot(Vec4 left, Vec4 right) { return ((left._elements[0] * right._elements[0]) + (left._elements[1] * right._elements[1]) + (left._elements[2] * right._elements[2]) + (left._elements[3] * right._elements[3])); }
Bool  math_vec4_equals(Vec4 left, Vec4 right) {
    return (math_abs(left._elements[0] - right._elements[0]) < VT_MATH_EPSILON) && (math_abs(left._elements[1] - right._elements[1]) < VT_MATH_EPSILON) && (math_abs(left._elements[2] - right._elements[2]) < VT_MATH_EPSILON) &&
           (math_abs(left._elements[3] - right._elements[3]) < VT_MATH_EPSILON);
}
Bool math_vec4_equals_approx(Vec4 left, Vec4 right, Flt32 limit) {
    return (math_abs(left._elements[0] - right._elements[0]) <= limit) && (math_abs(left._elements[1] - right._elements[1]) <= limit) && (math_abs(left._elements[2] - right._elements[2]) <= limit) &&
           (math_abs(left._elements[3] - right._elements[3]) <= limit);
}
Flt32 math_vec4_distance(Vec4 left, Vec4 right) { return math_vec4_magnitude(math_vec4_sub(left, right)); }
Flt32 math_vec4_distance_squared(Vec4 left, Vec4 right) { return math_vec4_magnitude_squared(math_vec4_sub(left, right)); }
Vec4  math_vec4_transform(Vec4 vec, Flt32 w, Mat4 mat) {
    return VT_UNION(Vec4, (vec._elements[0] * mat._elements[0 + 0]) + (vec._elements[1] * mat._elements[4 + 0]) + (vec._elements[2] * mat._elements[8 + 0]) + (w * mat._elements[12 + 0]),
                     (vec._elements[0] * mat._elements[0 + 1]) + (vec._elements[1] * mat._elements[4 + 1]) + (vec._elements[2] * mat._elements[8 + 1]) + (w * mat._elements[12 + 1]),
                     (vec._elements[0] * mat._elements[0 + 2]) + (vec._elements[1] * mat._elements[4 + 2]) + (vec._elements[2] * mat._elements[8 + 2]) + (w * mat._elements[12 + 2]),
                     (vec._elements[0] * mat._elements[0 + 3]) + (vec._elements[1] * mat._elements[4 + 3]) + (vec._elements[2] * mat._elements[8 + 3]) + (w * mat._elements[12 + 3]));
}
Vec4 math_vec4_convert_from_vec3(Vec3 vec, Flt32 w) { return VT_UNION(Vec4, vec._elements[0], vec._elements[1], vec._elements[2], w); }
Vec4 math_vec4_lerp(Vec4 start, Vec4 end, Flt32 time) { return math_vec4_add(math_vec4_mul_scalar(start, 1.0f - time), math_vec4_mul_scalar(end, time)); }
Vec4 math_vec4_slerp(Vec4 start, Vec4 end, Flt32 time) {
    Flt32 dot_ = VT_MATH_CLAMP(math_vec4_dot(start, end), -1.0f, 1.0f);
    if (math_abs(dot_ - 1.0f) < VT_MATH_EPSILON)
        return start;

    Flt32 theta_ = math_acos(dot_) * time;
    Vec4  vec_   = math_vec4_sub(end, math_vec4_mul_scalar(start, dot_));
    return math_vec4_add(math_vec4_mul_scalar(start, math_cos(theta_)), math_vec4_mul_scalar(vec_, math_sin(theta_)));
}
Vec4 math_vec4_project(Vec4 vec, Vec4 normal) { return math_vec4_mul_scalar(normal, math_vec4_dot(vec, normal) / math_vec4_dot(normal, normal)); }
Vec4 math_vec4_transform_matrix(Vec4 vec, Mat4 mat) {
    return VT_UNION(Vec4, mat._x1y1 * vec._elements[0] + mat._x2y1 * vec._elements[1] + mat._x3y1 * vec._elements[2] + mat._x4y1 * vec._elements[3],
                    mat._x1y2 * vec._elements[0] + mat._x2y2 * vec._elements[1] + mat._x3y2 * vec._elements[2] + mat._x4y2 * vec._elements[3],
                    mat._x1y3 * vec._elements[0] + mat._x2y3 * vec._elements[1] + mat._x3y3 * vec._elements[2] + mat._x4y3 * vec._elements[3],
                    mat._x1y4 * vec._elements[0] + mat._x2y4 * vec._elements[1] + mat._x3y4 * vec._elements[2] + mat._x4y4 * vec._elements[3]);
}
