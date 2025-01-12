#include "mat3.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/math/helper/math_helper.h"
#include "vytal/core/math/vector/vec2.h"
#include "vytal/core/math/vector/vec3.h"

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    include <emmintrin.h>

VT_INLINE Flt32 _math_mat3_hadd(__m128 vec) {
    __m128 shuf_ = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums_ = _mm_add_ps(vec, shuf_);
    shuf_        = _mm_shuffle_ps(sums_, sums_, _MM_SHUFFLE(1, 0, 3, 2));
    sums_        = _mm_add_ps(sums_, shuf_);
    return _mm_cvtss_f32(sums_);
}
VT_INLINE __m128 _math_mat3_dp(__m128 v1, __m128 v2, Int32 mask) {
    __m128 mul_ = _mm_mul_ps(v1, v2);
    if ((mask & 0x1) == 0)
        mul_ = _mm_and_ps(mul_, _mm_set1_ps(0.0f));
    if ((mask & 0x2) == 0)
        mul_ = _mm_and_ps(mul_, _mm_set1_ps(0.0f));
    if ((mask & 0x4) == 0)
        mul_ = _mm_and_ps(mul_, _mm_set1_ps(0.0f));
    if ((mask & 0x8) == 0)
        mul_ = _mm_and_ps(mul_, _mm_set1_ps(0.0f));

    __m128 shuf_ = _mm_shuffle_ps(mul_, mul_, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sum_  = _mm_add_ps(mul_, shuf_);
    shuf_        = _mm_shuffle_ps(sum_, sum_, _MM_SHUFFLE(1, 0, 3, 2));
    sum_         = _mm_add_ps(sum_, shuf_);
    return sum_;
}
VT_INLINE Int32 _math_mat3_test_all_zeros(__m128 vec) {
    __m128i zeros_ = _mm_setzero_si128();
    __m128i cmp_   = _mm_cmpeq_epi32(_mm_castps_si128(vec), zeros_);
    return (_mm_movemask_epi8(cmp_) == 0xffff);
}
VT_INLINE Int32 _math_mat3_test_all_ones(__m128 vec) {
    __m128i ones_ = _mm_set1_epi32(-1);
    __m128i cmp_  = _mm_cmpeq_epi32(_mm_castps_si128(vec), ones_);
    return (_mm_movemask_epi8(cmp_) == 0xffff);
}

Mat3 _math_mat3_add(Mat3 *left, Mat3 *right) {
    Mat3 res_;

    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[3]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[6]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[3]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[6]));

    _mm_storeu_ps(&(res_._elements[0]), _mm_add_ps(left0_, right0_));
    _mm_storeu_ps(&(res_._elements[3]), _mm_add_ps(left1_, right1_));
    _mm_storeu_ps(&(res_._elements[6]), _mm_add_ps(left2_, right2_));

    return res_;
}
Mat3 _math_mat3_sub(Mat3 *left, Mat3 *right) {
    Mat3 res_;

    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[3]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[6]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[3]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[6]));

    _mm_storeu_ps(&(res_._elements[0]), _mm_sub_ps(left0_, right0_));
    _mm_storeu_ps(&(res_._elements[3]), _mm_sub_ps(left1_, right1_));
    _mm_storeu_ps(&(res_._elements[6]), _mm_sub_ps(left2_, right2_));

    return res_;
}
Mat3 _math_mat3_mul(Mat3 *left, Mat3 *right) {
    Mat3 res_;

    for (int i = 0; i < 3; ++i) {
        __m128 row_ = _mm_set_ps(0.0f, left->_elements[i * 3 + 2], left->_elements[i * 3 + 1], left->_elements[i * 3 + 0]);

        __m128 col0_ = _mm_set_ps(0.0f, right->_elements[6], right->_elements[3], right->_elements[0]);
        __m128 col1_ = _mm_set_ps(0.0f, right->_elements[7], right->_elements[4], right->_elements[1]);
        __m128 col2_ = _mm_set_ps(0.0f, right->_elements[8], right->_elements[5], right->_elements[2]);

        res_._elements[i * 3 + 0] = _mm_cvtss_f32(_math_mat3_dp(row_, col0_, 0x71));
        res_._elements[i * 3 + 1] = _mm_cvtss_f32(_math_mat3_dp(row_, col1_, 0x71));
        res_._elements[i * 3 + 2] = _mm_cvtss_f32(_math_mat3_dp(row_, col2_, 0x71));
    }

    return res_;
}
Mat3 _math_mat3_mul_scalar(Mat3 *mat, Flt32 scalar) {
    Mat3   res_;
    __m128 scalar_ = _mm_set1_ps(scalar);
    __m128 mat0_   = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 mat1_   = _mm_loadu_ps(&(mat->_elements[3]));
    __m128 mat2_   = _mm_loadu_ps(&(mat->_elements[6]));

    _mm_storeu_ps(&(res_._elements[0]), _mm_mul_ps(mat0_, scalar_));
    _mm_storeu_ps(&(res_._elements[3]), _mm_mul_ps(mat1_, scalar_));
    _mm_storeu_ps(&(res_._elements[6]), _mm_mul_ps(mat2_, scalar_));
    return res_;
}
void _math_mat3_inverse(Mat3 *mat) {
    Flt32 det_ = mat->_elements[0] * (mat->_elements[4] * mat->_elements[8] - mat->_elements[5] * mat->_elements[7]) - mat->_elements[1] * (mat->_elements[3] * mat->_elements[8] - mat->_elements[5] * mat->_elements[6]) +
                 mat->_elements[2] * (mat->_elements[3] * mat->_elements[7] - mat->_elements[4] * mat->_elements[6]);

    if (math_abs(det_) < VT_MATH_EPSILON)
        return; // singular matrix cannot be inverted

    // cofactors (row-major)
    __m128 cof0_ = _mm_set_ps(0.0f,
                              mat->_elements[4] * mat->_elements[8] - mat->_elements[5] * mat->_elements[7],  // x0y0
                              mat->_elements[5] * mat->_elements[6] - mat->_elements[3] * mat->_elements[8],  // x0y1
                              mat->_elements[3] * mat->_elements[7] - mat->_elements[4] * mat->_elements[6]); // x0y2
    __m128 cof1_ = _mm_set_ps(0.0f,
                              mat->_elements[2] * mat->_elements[7] - mat->_elements[1] * mat->_elements[8],  // x1y0
                              mat->_elements[0] * mat->_elements[8] - mat->_elements[2] * mat->_elements[6],  // x1y1
                              mat->_elements[1] * mat->_elements[6] - mat->_elements[0] * mat->_elements[7]); // x1y2
    __m128 cof2_ = _mm_set_ps(0.0f,
                              mat->_elements[1] * mat->_elements[5] - mat->_elements[2] * mat->_elements[4],  // x2y0
                              mat->_elements[2] * mat->_elements[3] - mat->_elements[0] * mat->_elements[5],  // x2y1
                              mat->_elements[0] * mat->_elements[4] - mat->_elements[1] * mat->_elements[3]); // x2y2

    // adjugates
    __m128 adj0_ = _mm_set_ps(0.0f, _mm_cvtss_f32(_mm_shuffle_ps(cof0_, cof0_, _MM_SHUFFLE(0, 0, 0, 0))), _mm_cvtss_f32(_mm_shuffle_ps(cof1_, cof1_, _MM_SHUFFLE(0, 0, 0, 0))), _mm_cvtss_f32(_mm_shuffle_ps(cof2_, cof2_, _MM_SHUFFLE(0, 0, 0, 0))));
    __m128 adj1_ = _mm_set_ps(0.0f, _mm_cvtss_f32(_mm_shuffle_ps(cof0_, cof0_, _MM_SHUFFLE(1, 1, 1, 1))), _mm_cvtss_f32(_mm_shuffle_ps(cof1_, cof1_, _MM_SHUFFLE(1, 1, 1, 1))), _mm_cvtss_f32(_mm_shuffle_ps(cof2_, cof2_, _MM_SHUFFLE(1, 1, 1, 1))));
    __m128 adj2_ = _mm_set_ps(0.0f, _mm_cvtss_f32(_mm_shuffle_ps(cof0_, cof0_, _MM_SHUFFLE(2, 2, 2, 2))), _mm_cvtss_f32(_mm_shuffle_ps(cof1_, cof1_, _MM_SHUFFLE(2, 2, 2, 2))), _mm_cvtss_f32(_mm_shuffle_ps(cof2_, cof2_, _MM_SHUFFLE(2, 2, 2, 2))));

    // scale adjugates by 1 / determinant
    __m128 scale_ = _mm_set1_ps(1.0f / det_);
    adj0_         = _mm_mul_ps(adj0_, scale_);
    adj1_         = _mm_mul_ps(adj1_, scale_);
    adj2_         = _mm_mul_ps(adj2_, scale_);

    _mm_storeu_ps(&(mat->_elements[0]), adj0_);
    _mm_storeu_ps(&(mat->_elements[3]), adj1_);
    _mm_storeu_ps(&(mat->_elements[6]), adj2_);
}
Vec2 _math_mat3_mat3_mul_vec2(Mat3 *mat, Vec2 *vec) {
    Vec2 res_;

    __m128 row0_ = _mm_set_ps(0.0f, mat->_elements[2], mat->_elements[1], mat->_elements[0]);
    __m128 row1_ = _mm_set_ps(0.0f, mat->_elements[5], mat->_elements[4], mat->_elements[3]);
    __m128 ext_  = _mm_set_ps(1.0f, vec->_elements[1], vec->_elements[0], 0.0f);

    res_._elements[0] = _mm_cvtss_f32(_math_mat3_dp(row0_, ext_, 0x71));
    res_._elements[1] = _mm_cvtss_f32(_math_mat3_dp(row1_, ext_, 0x71));
    return res_;
}
Vec2 _math_mat3_vec2_mul_mat3(Vec2 *vec, Mat3 *mat) {
    Vec2 res_;

    __m128 col0_ = _mm_set_ps(0.0f, mat->_elements[6], mat->_elements[3], mat->_elements[0]);
    __m128 col1_ = _mm_set_ps(0.0f, mat->_elements[7], mat->_elements[4], mat->_elements[1]);
    __m128 ext_  = _mm_set_ps(1.0f, vec->_elements[1], vec->_elements[0], 0.0f);

    res_._elements[0] = _mm_cvtss_f32(_math_mat3_dp(ext_, col0_, 0x71));
    res_._elements[1] = _mm_cvtss_f32(_math_mat3_dp(ext_, col1_, 0x71));
    return res_;
}
Bool _math_mat3_equals(Mat3 *left, Mat3 *right) {
    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[3]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[6]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[3]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[6]));

    __m128 abs0_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left0_, right0_));
    __m128 abs1_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left1_, right1_));
    __m128 abs2_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left2_, right2_));

    __m128 eps_  = _mm_set1_ps(VT_MATH_EPSILON);
    __m128 cmp0_ = _mm_cmple_ps(abs0_, eps_);
    __m128 cmp1_ = _mm_cmple_ps(abs1_, eps_);
    __m128 cmp2_ = _mm_cmple_ps(abs2_, eps_);

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, cmp2_));
    return _math_mat3_test_all_ones(res_);
}
Bool _math_mat3_equals_approx(Mat3 *left, Mat3 *right, Flt32 limit) {
    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[3]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[6]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[3]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[6]));

    __m128 abs0_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left0_, right0_));
    __m128 abs1_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left1_, right1_));
    __m128 abs2_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), _mm_sub_ps(left2_, right2_));

    __m128 limit_ = _mm_set1_ps(limit);
    __m128 cmp0_  = _mm_cmple_ps(abs0_, limit_);
    __m128 cmp1_  = _mm_cmple_ps(abs1_, limit_);
    __m128 cmp2_  = _mm_cmple_ps(abs2_, limit_);

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, cmp2_));
    return _math_mat3_test_all_ones(res_);
}
Bool _math_mat3_is_zero(Mat3 *mat) {
    __m128 mat0_ = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 mat1_ = _mm_loadu_ps(&(mat->_elements[3]));
    __m128 mat2_ = _mm_loadu_ps(&(mat->_elements[6]));

    __m128 cmp0_ = _mm_cmpeq_ps(mat0_, _mm_setzero_ps());
    __m128 cmp1_ = _mm_cmpeq_ps(mat1_, _mm_setzero_ps());
    __m128 cmp2_ = _mm_cmpeq_ps(mat2_, _mm_setzero_ps());

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, cmp2_));
    return _math_mat3_test_all_ones(res_);
}
Bool _math_mat3_is_identity(Mat3 *mat) {
    __m128 mat0_ = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 mat1_ = _mm_loadu_ps(&(mat->_elements[3]));
    __m128 mat2_ = _mm_loadu_ps(&(mat->_elements[6]));

    __m128 cmp0_ = _mm_cmpeq_ps(mat0_, _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f));
    __m128 cmp1_ = _mm_cmpeq_ps(mat1_, _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f));
    __m128 cmp2_ = _mm_cmpeq_ps(mat2_, _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, cmp2_));
    return _math_mat3_test_all_ones(res_);
}

#else

Mat3 _math_mat3_add(Mat3 *left, Mat3 *right) {
    return VT_UNION(Mat3, left->_elements[0] + right->_elements[0], left->_elements[1] + right->_elements[1], left->_elements[2] + right->_elements[2], left->_elements[3] + right->_elements[3], left->_elements[4] + right->_elements[4],
                    left->_elements[5] + right->_elements[5], left->_elements[6] + right->_elements[6], left->_elements[7] + right->_elements[7], left->_elements[8] + right->_elements[8]);
}
Mat3 _math_mat3_sub(Mat3 *left, Mat3 *right) {
    return VT_UNION(Mat3, left->_elements[0] - right->_elements[0], left->_elements[1] - right->_elements[1], left->_elements[2] - right->_elements[2], left->_elements[3] - right->_elements[3], left->_elements[4] - right->_elements[4],
                    left->_elements[5] - right->_elements[5], left->_elements[6] - right->_elements[6], left->_elements[7] - right->_elements[7], left->_elements[8] - right->_elements[8]);
}
Mat3 _math_mat3_mul(Mat3 *left, Mat3 *right) {
    return VT_UNION(Mat3, (left->_elements[0] * right->_elements[0]) + (left->_elements[1] * right->_elements[3]) + (left->_elements[2] * right->_elements[6]),
                    (left->_elements[0] * right->_elements[1]) + (left->_elements[1] * right->_elements[4]) + (left->_elements[2] * right->_elements[7]),
                    (left->_elements[0] * right->_elements[2]) + (left->_elements[1] * right->_elements[5]) + (left->_elements[2] * right->_elements[8]),
                    (left->_elements[3] * right->_elements[0]) + (left->_elements[4] * right->_elements[3]) + (left->_elements[5] * right->_elements[6]),
                    (left->_elements[3] * right->_elements[1]) + (left->_elements[4] * right->_elements[4]) + (left->_elements[5] * right->_elements[7]),
                    (left->_elements[3] * right->_elements[2]) + (left->_elements[4] * right->_elements[5]) + (left->_elements[5] * right->_elements[8]),
                    (left->_elements[6] * right->_elements[0]) + (left->_elements[7] * right->_elements[3]) + (left->_elements[8] * right->_elements[6]),
                    (left->_elements[6] * right->_elements[1]) + (left->_elements[7] * right->_elements[4]) + (left->_elements[8] * right->_elements[7]),
                    (left->_elements[6] * right->_elements[2]) + (left->_elements[7] * right->_elements[5]) + (left->_elements[8] * right->_elements[8]));
}
Mat3 _math_mat3_mul_scalar(Mat3 *mat, Flt32 scalar) {
    VT_UNION(Mat3, mat->_elements[0] * scalar, mat->_elements[1] * scalar, mat->_elements[2] * scalar, mat->_elements[3] * scalar, mat->_elements[4] * scalar, mat->_elements[5] * scalar, mat->_elements[6] * scalar, mat->_elements[7] * scalar,
             mat->_elements[8] * scalar);
}
void _math_mat3_inverse(Mat3 *mat) {
    Flt32 det_        = math_mat3_determinant(mat);
    Mat3  transposed_ = math_mat3_transposed(mat);

    Mat3 mat_adj_ = VT_UNION(Mat3, transposed_._elements[4] * transposed_._elements[8] - transposed_._elements[5] * transposed_._elements[7], -transposed_._elements[3] * transposed_._elements[8] + transposed_._elements[5] * transposed_._elements[6],
                             transposed_._elements[3] * transposed_._elements[7] - transposed_._elements[4] * transposed_._elements[6], -transposed_._elements[1] * transposed_._elements[8] + transposed_._elements[2] * transposed_._elements[7],
                             transposed_._elements[0] * transposed_._elements[8] - transposed_._elements[2] * transposed_._elements[6], -transposed_._elements[0] * transposed_._elements[7] + transposed_._elements[1] * transposed_._elements[6],
                             transposed_._elements[1] * transposed_._elements[5] - transposed_._elements[2] * transposed_._elements[4], -transposed_._elements[0] * transposed_._elements[5] + transposed_._elements[2] * transposed_._elements[3],
                             transposed_._elements[0] * transposed_._elements[4] - transposed_._elements[1] * transposed_._elements[3]);

    *mat = math_mat3_mul_scalar(&mat_adj_, 1.0f / det_);
}
Vec2 _math_mat3_mat3_mul_vec2(Mat3 *mat, Vec2 *vec) { return VT_UNION(Vec2, (mat->_elements[0] * vec->_elements[0]) + (mat->_elements[1] * vec->_elements[1]), (mat->_elements[3] * vec->_elements[0]) + (mat->_elements[4] * vec->_elements[1])); }
Vec2 _math_mat3_vec2_mul_mat3(Vec2 *vec, Mat3 *mat) {
    return VT_UNION(Vec2, ((vec->_elements[0] * mat->_elements[0]) + (vec->_elements[1] * mat->_elements[3]) + mat->_elements[6]), ((vec->_elements[0] * mat->_elements[1]) + (vec->_elements[1] * mat->_elements[4]) + mat->_elements[6]));
}
Bool _math_mat3_equals(Mat3 *left, Mat3 *right) {
    return ((left->_elements[0] - right->_elements[0]) < VT_MATH_EPSILON) && ((left->_elements[1] - right->_elements[1]) < VT_MATH_EPSILON) && ((left->_elements[2] - right->_elements[2]) < VT_MATH_EPSILON) &&
           ((left->_elements[3] - right->_elements[3]) < VT_MATH_EPSILON) && ((left->_elements[4] - right->_elements[4]) < VT_MATH_EPSILON) && ((left->_elements[5] - right->_elements[5]) < VT_MATH_EPSILON) &&
           ((left->_elements[6] - right->_elements[6]) < VT_MATH_EPSILON) && ((left->_elements[7] - right->_elements[7]) < VT_MATH_EPSILON) && ((left->_elements[8] - right->_elements[8]) < VT_MATH_EPSILON);
}
Bool _math_mat3_equals_approx(Mat3 *left, Mat3 *right, Flt32 limit) {
    return ((left->_elements[0] - right->_elements[0]) <= limit) && ((left->_elements[1] - right->_elements[1]) <= limit) && ((left->_elements[2] - right->_elements[2]) <= limit) && ((left->_elements[3] - right->_elements[3]) <= limit) &&
           ((left->_elements[4] - right->_elements[4]) <= limit) && ((left->_elements[5] - right->_elements[5]) <= limit) && ((left->_elements[6] - right->_elements[6]) <= limit) && ((left->_elements[7] - right->_elements[7]) <= limit) &&
           ((left->_elements[8] - right->_elements[8]) <= limit);
}
Bool _math_mat3_is_zero(Mat3 *mat) {
    return (mat->_elements[0] == 0.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 0.0f) && (mat->_elements[5] == 0.0f) && (mat->_elements[6] == 0.0f) &&
           (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 0.0f);
}
Bool _math_mat3_is_identity(Mat3 *mat) {
    return (mat->_elements[0] == 1.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 1.0f) && (mat->_elements[5] == 0.0f) && (mat->_elements[6] == 0.0f) &&
           (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 1.0f);
}

#endif

Mat3 math_mat3_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x3y1, Flt32 x1y2, Flt32 x2y2, Flt32 x3y2, Flt32 x1y3, Flt32 x2y3, Flt32 x3y3) { return VT_UNION(Mat3, x1y1, x2y1, x3y1, x1y2, x2y2, x3y2, x1y3, x2y3, x3y3); }
Mat3 math_mat3_zero(void) { return VT_UNION(Mat3, 0.0f); }
Mat3 math_mat3_identity(void) { return VT_UNION(Mat3, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat3 math_mat3_copy(Mat3 *mat) { return !mat ? math_mat3_identity() : *mat; }
Mat3 math_mat3_from_vectors(Vec2 vec1, Vec2 vec2) {
    return VT_UNION(Mat3, vec1._elements[0] * vec2._elements[0], vec1._elements[0] * vec2._elements[1], 0.0f, vec1._elements[1] * vec2._elements[0], vec1._elements[1] * vec2._elements[1], 0.0f, 0.0f, 0.0f, 1.0f);
}
Mat3 math_mat3_add(Mat3 *left, Mat3 *right) { return (!left || !right) ? math_mat3_identity() : _math_mat3_add(left, right); }
Mat3 math_mat3_sub(Mat3 *left, Mat3 *right) { return (!left || !right) ? math_mat3_identity() : _math_mat3_sub(left, right); }
Mat3 math_mat3_mul(Mat3 *left, Mat3 *right) { return (!left || !right) ? math_mat3_identity() : _math_mat3_mul(left, right); }
Mat3 math_mat3_mul_scalar(Mat3 *mat, Flt32 scalar) { return !mat ? math_mat3_identity() : _math_mat3_mul_scalar(mat, scalar); }
Vec2 math_mat3_mat3_mul_vec2(Mat3 *mat, Vec2 *vec) { return (!mat || !vec) ? math_vec2_zero() : _math_mat3_mat3_mul_vec2(mat, vec); }
Vec2 math_mat3_vec2_mul_mat3(Vec2 *vec, Mat3 *mat) { return (!mat || !vec) ? math_vec2_zero() : _math_mat3_vec2_mul_mat3(vec, mat); }
void math_mat3_transpose(Mat3 *mat) {
    if (!mat)
        return;

    hal_mem_memswap(&(mat->_elements[1]), &(mat->_elements[3]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[2]), &(mat->_elements[6]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[5]), &(mat->_elements[7]), sizeof(Flt32));
}
Mat3 math_mat3_transposed(Mat3 *mat) {
    math_mat3_transpose(mat);
    return *mat;
}
Flt32 math_mat3_determinant(Mat3 *mat) {
    return !mat ? 0.0f
                : (mat->_elements[0] * ((mat->_elements[4] * mat->_elements[8]) - (mat->_elements[7] * mat->_elements[5]))) - (mat->_elements[1] * ((mat->_elements[3] * mat->_elements[8]) - (mat->_elements[6] * mat->_elements[5]))) +
                      (mat->_elements[2] * ((mat->_elements[3] * mat->_elements[7]) - (mat->_elements[6] * mat->_elements[4])));
}
void math_mat3_inverse(Mat3 *mat) {
    if (!mat || (math_mat3_determinant(mat) == 0))
        return;

    _math_mat3_inverse(mat);
}
Mat3 math_mat3_inversed(Mat3 *mat) {
    math_mat3_inverse(mat);
    return *mat;
}
Mat3 math_mat3_make_translation(Vec2 position) { return VT_UNION(Mat3, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, position._elements[0], position._elements[1], 1.0f); }
Mat3 math_mat3_make_rotation(Flt32 rad_angle) { return VT_UNION(Mat3, math_cos(rad_angle), math_sin(rad_angle), 0.0f, -math_sin(rad_angle), math_cos(rad_angle), 0.0f, 0.0f, 0.0f, 1.0f); }
Mat3 math_mat3_make_scale(Vec2 scale) { return VT_UNION(Mat3, scale._elements[0], 0.0f, 0.0f, 0.0f, scale._elements[1], 0.0f, 0.0f, 0.0f, 1.0f); }
Vec3 math_mat3_get_row(Mat3 *mat, Int32 row) { return (!mat || (row < 0) || (row > 2)) ? math_vec3_zero() : VT_UNION(Vec3, mat->_elements[(row * 3) + 0], mat->_elements[(row * 3) + 1], mat->_elements[(row * 3) + 2]); }
Vec3 math_mat3_get_column(Mat3 *mat, Int32 column) { return (!mat || (column < 0) || (column > 2)) ? math_vec3_zero() : VT_UNION(Vec3, mat->_elements[column + 0], mat->_elements[column + 3], mat->_elements[column + 6]); }
void math_mat3_set_row(Mat3 *mat, Int32 row, Vec3 *data) {
    if (!mat || !data || (row < 0) || (row > 2))
        return;

    hal_mem_memcpy(&(mat->_elements[row * 3]), data, sizeof(Vec3));
}
void math_mat3_set_column(Mat3 *mat, Int32 column, Vec3 *data) {
    if (!mat || !data || (column < 0) || (column > 2))
        return;

    mat->_elements[column + 0] = data->_elements[0];
    mat->_elements[column + 3] = data->_elements[1];
    mat->_elements[column + 6] = data->_elements[2];
}
Bool math_mat3_equals(Mat3 *left, Mat3 *right) { return (!left || !right) ? false : _math_mat3_equals(left, right); }
Bool math_mat3_equals_approx(Mat3 *left, Mat3 *right, Flt32 limit) { return (!left || !right) ? false : _math_mat3_equals_approx(left, right, limit); }
Bool math_mat3_is_zero(Mat3 *mat) { return (!mat) ? false : _math_mat3_is_zero(mat); }
Bool math_mat3_is_identity(Mat3 *mat) { return (!mat) ? false : _math_mat3_is_identity(mat); }
