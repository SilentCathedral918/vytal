#include "vec2.h"

#include "vytal/core/math/helper/math_helper.h"

Vec2  math_vec2_construct(Flt32 x, Flt32 y) { return VT_UNION(Vec2, x, y); }
Vec2  math_vec2_zero(void) { return VT_UNION(Vec2, 0.0f); }
Vec2  math_vec2_one(void) { return VT_UNION(Vec2, 1.0f); }
Vec2  math_vec2_scale(Flt32 scale) { return VT_UNION(Vec2, scale); }
Vec2  math_vec2_up(void) { return VT_UNION(Vec2, 0.0f, 1.0f); }
Vec2  math_vec2_down(void) { return VT_UNION(Vec2, 0.0f, -1.0f); }
Vec2  math_vec2_left(void) { return VT_UNION(Vec2, -1.0f, 0.0f); }
Vec2  math_vec2_right(void) { return VT_UNION(Vec2, 1.0f, 0.0f); }
Vec2  math_vec2_copy(Vec2 *vec) { return VT_UNION(Vec2, vec->_elements[0], vec->_elements[1]); }
Vec2  math_vec2_add(Vec2 left, Vec2 right) { return VT_UNION(Vec2, left._elements[0] + right._elements[0], left._elements[1] + right._elements[1]); }
Vec2  math_vec2_sub(Vec2 left, Vec2 right) { return VT_UNION(Vec2, left._elements[0] - right._elements[0], left._elements[1] - right._elements[1]); }
Vec2  math_vec2_mul(Vec2 left, Vec2 right) { return VT_UNION(Vec2, left._elements[0] * right._elements[0], left._elements[1] * right._elements[1]); }
Vec2  math_vec2_mul_scalar(Vec2 vec, Flt32 scalar) { return VT_UNION(Vec2, vec._elements[0] * scalar, vec._elements[1] * scalar); }
Vec2  math_vec2_mul_add(Vec2 a, Vec2 b, Vec2 c) { return VT_UNION(Vec2, a._elements[0] * b._elements[0] + c._elements[0], a._elements[1] * b._elements[1] + c._elements[1]); }
Vec2  math_vec2_div(Vec2 left, Vec2 right) { return VT_UNION(Vec2, (right._elements[0] == 0) ? 0 : left._elements[0] / right._elements[0], (right._elements[1] == 1) ? 0 : left._elements[1] / right._elements[1]); }
 Vec2 math_vec2_div_scalar(Vec2 vec, Flt32 scalar) { return VT_UNION(Vec2, (scalar == 0) ? 0 : vec._elements[0] / scalar, (scalar == 0) ? 0 : vec._elements[1] / scalar); }
Flt32 math_vec2_magnitude(Vec2 vec) { return math_sqrt(vec._elements[0] * vec._elements[0] + vec._elements[1] * vec._elements[1]); }
Flt32 math_vec2_magnitude_squared(Vec2 vec) { return (vec._elements[0] * vec._elements[0] + vec._elements[1] * vec._elements[1]); }
void  math_vec2_normalize(Vec2 *vec) {
    if (!vec)
        return;

    Flt32 length_ = math_vec2_magnitude(*vec);

    vec->_elements[0] = (length_ == 0) ? 0.0f : vec->_elements[0] / length_;
    vec->_elements[1] = (length_ == 0) ? 0.0f : vec->_elements[1] / length_;
}
Vec2 math_vec2_normalized(Vec2 vec) {
    math_vec2_normalize(&vec);
    return vec;
}
Bool  math_vec2_equals(Vec2 left, Vec2 right) { return ((math_abs(left._elements[0] - right._elements[0]) < VT_MATH_EPSILON) && (math_abs(left._elements[1] - right._elements[1]) < VT_MATH_EPSILON)); }
Bool  math_vec2_equals_approx(Vec2 left, Vec2 right, Flt32 limit) { return ((math_abs(left._elements[0] - right._elements[0]) <= limit) && (math_abs(left._elements[1] - right._elements[1]) <= limit)); }
Flt32 math_vec2_distance(Vec2 left, Vec2 right) { return math_vec2_magnitude(math_vec2_sub(left, right)); }
Flt32 math_vec2_distance_squared(Vec2 left, Vec2 right) { return math_vec2_magnitude_squared(math_vec2_sub(left, right)); }
Flt32 math_vec2_dot(Vec2 left, Vec2 right) { return ((left._elements[0] * right._elements[0]) + (left._elements[1] * right._elements[1])); }
Flt32 math_vec2_cross(Vec2 left, Vec2 right) { return ((left._elements[0] * right._elements[0]) - (left._elements[1] * right._elements[1])); }
Vec2  math_vec2_lerp(Vec2 start, Vec2 end, Flt32 time) { return math_vec2_add(math_vec2_mul_scalar(start, 1.0f - time), math_vec2_mul_scalar(end, time)); }
Vec2  math_vec2_slerp(Vec2 start, Vec2 end, Flt32 time) {
    Flt32 dot_ = VT_MATH_CLAMP(math_vec2_dot(start, end), -1.0f, 1.0f);
    if (math_abs(dot_ - 1.0f) < VT_MATH_EPSILON)
        return start;

    Flt32 theta_ = math_acos(dot_) * time;
    Vec2  vec_   = math_vec2_sub(end, math_vec2_mul_scalar(start, dot_));
    return math_vec2_add(math_vec2_mul_scalar(start, math_cos(theta_)), math_vec2_mul_scalar(vec_, math_sin(theta_)));
}
Vec2 math_vec2_project(Vec2 vec, Vec2 normal) { return math_vec2_mul_scalar(normal, math_vec2_dot(vec, normal) / math_vec2_dot(normal, normal)); }
Vec2 math_vec2_reflect(Vec2 vec, Vec2 normal) { return math_vec2_sub(vec, math_vec2_mul_scalar(normal, math_vec2_dot(vec, normal) * 2.0f)); }
Vec2 math_vec2_transform_matrix(Vec2 vec, Mat2 mat) { return VT_UNION(Vec2, mat._x1y1 * vec._elements[0] + mat._x2y1 * vec._elements[1], mat._x1y2 * vec._elements[0] + mat._x2y2 * vec._elements[1]); }
