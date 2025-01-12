#include "quat4.h"

#include "vytal/core/math/helper/math_helper.h"

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    include <emmintrin.h>

Quat4 _math_quat4_mul(Quat4 left, Quat4 right) {
    Quat4 res_;

    __m128 left_  = _mm_load_ps(left._elements);
    __m128 right_ = _mm_load_ps(right._elements);

    __m128 w1_ = _mm_shuffle_ps(left_, left_, _MM_SHUFFLE(3, 3, 3, 3));
    __m128 x1_ = _mm_shuffle_ps(left_, left_, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 y1_ = _mm_shuffle_ps(left_, left_, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 z1_ = _mm_shuffle_ps(left_, left_, _MM_SHUFFLE(2, 2, 2, 2));

    __m128 w2_ = _mm_shuffle_ps(right_, right_, _MM_SHUFFLE(3, 3, 3, 3));
    __m128 x2_ = _mm_shuffle_ps(right_, right_, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 y2_ = _mm_shuffle_ps(right_, right_, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 z2_ = _mm_shuffle_ps(right_, right_, _MM_SHUFFLE(2, 2, 2, 2));

    __m128 resx_ = _mm_sub_ps(_mm_sub_ps(_mm_mul_ps(w1_, w2_), _mm_mul_ps(x1_, x2_)), _mm_add_ps(_mm_mul_ps(y1_, y2_), _mm_mul_ps(z1_, z2_)));
    __m128 resy_ = _mm_add_ps(_mm_add_ps(_mm_mul_ps(w1_, x2_), _mm_mul_ps(x1_, w2_)), _mm_sub_ps(_mm_mul_ps(y1_, z2_), _mm_mul_ps(z1_, y2_)));
    __m128 resz_ = _mm_add_ps(_mm_sub_ps(_mm_mul_ps(w1_, y2_), _mm_mul_ps(x1_, z2_)), _mm_add_ps(_mm_mul_ps(y1_, w2_), _mm_mul_ps(z1_, x2_)));
    __m128 resw_ = _mm_add_ps(_mm_add_ps(_mm_mul_ps(w1_, z2_), _mm_mul_ps(x1_, y2_)), _mm_sub_ps(_mm_mul_ps(y1_, x2_), _mm_mul_ps(z1_, w2_)));

    _mm_store_ps(res_._elements, _mm_shuffle_ps(resx_, resy_, _MM_SHUFFLE(0, 1, 2, 3)));
    _mm_store_ps(res_._elements + 2, _mm_shuffle_ps(resz_, resw_, _MM_SHUFFLE(0, 1, 2, 3)));
    return res_;
}

#else

Quat4 _math_quat4_mul(Quat4 left, Quat4 right) {
    return VT_UNION(Quat4, (left._elements[3] * right._elements[3]) - (left._elements[0] * right._elements[0]) - (left._elements[1] * right._elements[1]) - (left._elements[2] * right._elements[2]),
                    (left._elements[3] * right._elements[0]) + (left._elements[0] * right._elements[3]) + (left._elements[1] * right._elements[2]) - (left._elements[2] * right._elements[1]),
                    (left._elements[3] * right._elements[1]) - (left._elements[0] * right._elements[2]) + (left._elements[1] * right._elements[3]) + (left._elements[2] * right._elements[0]),
                    (left._elements[3] * right._elements[2]) + (left._elements[0] * right._elements[1]) - (left._elements[1] * right._elements[0]) + (left._elements[2] * right._elements[3]));
}

#endif

Quat4 math_quat4_construct(Flt32 x, Flt32 y, Flt32 z, Flt32 w) { return VT_UNION(Quat4, x, y, z, w); }
Quat4 math_quat4_identity(void) { return VT_UNION(Quat4, 0.0f, 0.0f, 0.0f, 1.0f); }
Quat4 math_quat4_copy(Quat4 *quat) { return !quat ? math_quat4_identity() : *quat; }
Quat4 math_quat4_convert_from_angles(Flt32 x, Flt32 y, Flt32 z) {
    const Quat4 roll_  = math_quat4_construct(math_sin(x * 0.5f), 0.0f, 0.0f, math_cos(x * 0.5f));
    const Quat4 pitch_ = math_quat4_construct(0.0f, math_sin(y * 0.5f), 0.0f, math_cos(y * 0.5f));
    const Quat4 yaw_   = math_quat4_construct(0.0f, 0.0f, math_sin(z * 0.5f), math_cos(z * 0.5f));
    return math_quat4_normalized(math_quat4_mul(math_quat4_mul(yaw_, pitch_), roll_));
}
Quat4 math_quat4_convert_from_rotation(Vec3 rotation) { return math_quat4_convert_from_angles(rotation._elements[0], rotation._elements[1], rotation._elements[2]); }
Quat4 math_quat4_convert_from_axis_angle(Vec3 axis, Flt32 angle, Bool normalize) {
    const Flt32 angle_sin_ = math_sin(angle * 0.5f);
    const Flt32 angle_cos_ = math_cos(angle * 0.5f);
    return (!normalize) ? math_quat4_construct(axis._elements[0] * angle_sin_, axis._elements[1] * angle_sin_, axis._elements[2] * angle_sin_, angle_cos_)
                        : math_quat4_normalized(math_quat4_construct(axis._elements[0] * angle_sin_, axis._elements[1] * angle_sin_, axis._elements[2] * angle_sin_, angle_cos_));
}
Mat4 math_quat4_convert_to_mat4(Quat4 quat) {
    const Quat4 norm_ = math_quat4_normalized(quat);
    const Flt32 xx_   = norm_._elements[0] * norm_._elements[0];
    const Flt32 yy_   = norm_._elements[1] * norm_._elements[1];
    const Flt32 zz_   = norm_._elements[2] * norm_._elements[2];
    const Flt32 xy_   = 2.0f * norm_._elements[0] * norm_._elements[1];
    const Flt32 xz_   = 2.0f * norm_._elements[0] * norm_._elements[2];
    const Flt32 yz_   = 2.0f * norm_._elements[1] * norm_._elements[2];
    const Flt32 wx_   = 2.0f * norm_._elements[3] * norm_._elements[0];
    const Flt32 wy_   = 2.0f * norm_._elements[3] * norm_._elements[1];
    const Flt32 wz_   = 2.0f * norm_._elements[3] * norm_._elements[2];

    return VT_UNION(Mat4, 1.0f - 2.0f * (yy_ + zz_), xy_ + wz_, xz_ - wy_, 0.0f, xy_ - wz_, 1.0f - 2.0f * (xx_ + zz_), yz_ + wx_, 0.0f, xz_ + wy_, yz_ - wx_, 1.0f - 2.0f * (xx_ + yy_), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}
Quat4 math_quat4_add(Quat4 left, Quat4 right) { return VT_UNION(Quat4, left._elements[0] + right._elements[0], left._elements[1] + right._elements[1], left._elements[2] + right._elements[2], left._elements[3] + right._elements[3]); }
Quat4 math_quat4_sub(Quat4 left, Quat4 right) { return VT_UNION(Quat4, left._elements[0] - right._elements[0], left._elements[1] - right._elements[1], left._elements[2] - right._elements[2], left._elements[3] - right._elements[3]); }
Quat4 math_quat4_mul(Quat4 left, Quat4 right) { return _math_quat4_mul(left, right); }
Quat4 math_quat4_mul_scalar(Quat4 quat, Flt32 scalar) { return VT_UNION(Quat4, quat._elements[0] * scalar, quat._elements[1] * scalar, quat._elements[2] * scalar, quat._elements[3] * scalar); }
Flt32 math_quat4_dot(Quat4 left, Quat4 right) { return ((left._elements[0] * right._elements[0]) + (left._elements[1] * right._elements[1]) + (left._elements[2] * right._elements[2]) + (left._elements[3] * right._elements[3])); }
Bool  math_quat4_equals(Quat4 left, Quat4 right) {
    return ((left._elements[0] - right._elements[0]) < VT_MATH_EPSILON) && ((left._elements[1] - right._elements[1]) < VT_MATH_EPSILON) && ((left._elements[2] - right._elements[2]) < VT_MATH_EPSILON) &&
           ((left._elements[3] - right._elements[3]) < VT_MATH_EPSILON);
}
Bool math_quat4_equals_approx(Quat4 left, Quat4 right, Flt32 limit) {
    return ((left._elements[0] - right._elements[0]) <= limit) && ((left._elements[1] - right._elements[1]) <= limit) && ((left._elements[2] - right._elements[2]) <= limit) && ((left._elements[3] - right._elements[3]) <= limit);
}
Flt32 math_quat4_magnitude_squared(Quat4 quat) { return ((quat._elements[0] * quat._elements[0]) + (quat._elements[1] * quat._elements[1]) + (quat._elements[2] * quat._elements[2]) + (quat._elements[3] * quat._elements[3])); }
Flt32 math_quat4_magnitude(Quat4 quat) { return math_sqrt((quat._elements[0] * quat._elements[0]) + (quat._elements[1] * quat._elements[1]) + (quat._elements[2] * quat._elements[2]) + (quat._elements[3] * quat._elements[3])); }
void  math_quat4_normalize(Quat4 *quat) {
    if (!quat)
        return;

    Flt32 magnitude_ = math_quat4_magnitude(*quat);
    if (magnitude_ < VT_MATH_EPSILON)
        return;

    magnitude_ = 1.0f / magnitude_;
    quat->_elements[0] *= magnitude_;
    quat->_elements[1] *= magnitude_;
    quat->_elements[2] *= magnitude_;
    quat->_elements[3] *= magnitude_;
}
Quat4 math_quat4_normalized(Quat4 quat) {
    math_quat4_normalize(&quat);
    return quat;
}
Quat4 math_quat4_conjugate(Quat4 quat) { return VT_UNION(Quat4, quat._elements[0] * -1.0f, quat._elements[1] * -1.0f, quat._elements[2] * -1.0f, quat._elements[3]); }
Quat4 math_quat4_quotient(Quat4 left, Quat4 right) { return math_quat4_mul(math_quat4_inversed(left), math_quat4_inversed(right)); }
void  math_quat4_inverse(Quat4 *quat) {
    if (!quat)
        return;

    *quat = math_quat4_normalized(math_quat4_conjugate(*quat));
}
Quat4 math_quat4_inversed(Quat4 quat) {
    math_quat4_inverse(&quat);
    return quat;
}
Quat4 math_quat4_lerp(Quat4 start, Quat4 end, Flt32 time, Bool normalize) {
    Quat4 res_ = math_quat4_add(math_quat4_mul_scalar(start, 1.0f - time), math_quat4_mul_scalar(end, time));
    return !normalize ? res_ : math_quat4_normalized(res_);
}
Quat4 math_quat4_slerp(Quat4 start, Quat4 end, Flt32 time, Bool normalize) {
    const Flt32 theta_ = math_abs(math_acos(math_quat4_dot(start, end)));
    const Flt32 dot_   = math_quat4_dot(start, end);
    const Flt32 wp_    = math_sin((1 - time) * theta_) / math_sin(theta_);
    const Flt32 wq_    = math_sin(time * theta_) / math_sin(theta_);

    if (dot_ < 0.0f)
        start = math_quat4_mul_scalar(start, -1.0f);
    return !normalize ? math_quat4_add(math_quat4_mul_scalar(start, wp_), math_quat4_mul_scalar(end, wq_)) : math_quat4_normalized(math_quat4_add(math_quat4_mul_scalar(start, wp_), math_quat4_mul_scalar(end, wq_)));
}
