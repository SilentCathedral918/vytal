#include "mat4.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/math/helper/math_helper.h"
#include "vytal/core/math/matrix/mat3.h"
#include "vytal/core/math/vector/vec3.h"
#include "vytal/core/math/vector/vec4.h"

#if defined(__SSE2__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2))
#    include <emmintrin.h>

VT_INLINE Flt32 _math_mat4_hadd(__m128 vec) {
    __m128 shuf_ = _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums_ = _mm_add_ps(vec, shuf_);
    shuf_        = _mm_shuffle_ps(sums_, sums_, _MM_SHUFFLE(1, 0, 3, 2));
    sums_        = _mm_add_ps(sums_, shuf_);
    return _mm_cvtss_f32(sums_);
}
VT_INLINE __m128 _math_mat4_dp(__m128 v1, __m128 v2, Int32 mask) {
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
VT_INLINE Int32 _math_mat4_test_all_zeros(__m128 vec) {
    __m128i zeros_ = _mm_setzero_si128();
    __m128i cmp_   = _mm_cmpeq_epi32(_mm_castps_si128(vec), zeros_);
    return (_mm_movemask_epi8(cmp_) == 0xffff);
}
VT_INLINE Int32 _math_mat4_test_all_ones(__m128 vec) {
    __m128i ones_ = _mm_set1_epi32(-1);
    __m128i cmp_  = _mm_cmpeq_epi32(_mm_castps_si128(vec), ones_);
    return (_mm_movemask_epi8(cmp_) == 0xffff);
}

Mat4 _math_mat4_add(Mat4 *left, Mat4 *right) {
    Mat4 res_;

    __m128 left1_ = _mm_load_ps(&(left->_elements[0]));
    __m128 left2_ = _mm_load_ps(&(left->_elements[4]));
    __m128 left3_ = _mm_load_ps(&(left->_elements[8]));
    __m128 left4_ = _mm_load_ps(&(left->_elements[12]));

    __m128 right1_ = _mm_load_ps(&(right->_elements[0]));
    __m128 right2_ = _mm_load_ps(&(right->_elements[4]));
    __m128 right3_ = _mm_load_ps(&(right->_elements[8]));
    __m128 right4_ = _mm_load_ps(&(right->_elements[12]));

    _mm_store_ps(&(res_._elements[0]), _mm_add_ps(left1_, right1_));
    _mm_store_ps(&(res_._elements[4]), _mm_add_ps(left2_, right2_));
    _mm_store_ps(&(res_._elements[8]), _mm_add_ps(left3_, right3_));
    _mm_store_ps(&(res_._elements[12]), _mm_add_ps(left4_, right4_));

    return res_;
}
Mat4 _math_mat4_sub(Mat4 *left, Mat4 *right) {
    Mat4 res_;

    __m128 left1_ = _mm_load_ps(&(left->_elements[0]));
    __m128 left2_ = _mm_load_ps(&(left->_elements[4]));
    __m128 left3_ = _mm_load_ps(&(left->_elements[8]));
    __m128 left4_ = _mm_load_ps(&(left->_elements[12]));

    __m128 right1_ = _mm_load_ps(&(right->_elements[0]));
    __m128 right2_ = _mm_load_ps(&(right->_elements[4]));
    __m128 right3_ = _mm_load_ps(&(right->_elements[8]));
    __m128 right4_ = _mm_load_ps(&(right->_elements[12]));

    _mm_store_ps(&(res_._elements[0]), _mm_sub_ps(left1_, right1_));
    _mm_store_ps(&(res_._elements[4]), _mm_sub_ps(left2_, right2_));
    _mm_store_ps(&(res_._elements[8]), _mm_sub_ps(left3_, right3_));
    _mm_store_ps(&(res_._elements[12]), _mm_sub_ps(left4_, right4_));

    return res_;
}
Mat4 _math_mat4_mul(Mat4 *left, Mat4 *right) {
    Mat4 res_;

    __m128 right1_ = _mm_set_ps(right->_elements[12], right->_elements[8], right->_elements[4], right->_elements[0]);
    __m128 right2_ = _mm_set_ps(right->_elements[13], right->_elements[9], right->_elements[5], right->_elements[1]);
    __m128 right3_ = _mm_set_ps(right->_elements[14], right->_elements[10], right->_elements[6], right->_elements[2]);
    __m128 right4_ = _mm_set_ps(right->_elements[15], right->_elements[11], right->_elements[7], right->_elements[3]);

    for (ByteSize i = 0; i < 4; ++i) {
        __m128 left_              = _mm_load_ps(&(left->_elements[i * 4]));
        res_._elements[i * 4 + 0] = _math_mat4_hadd(_mm_mul_ps(left_, right1_));
        res_._elements[i * 4 + 1] = _math_mat4_hadd(_mm_mul_ps(left_, right2_));
        res_._elements[i * 4 + 2] = _math_mat4_hadd(_mm_mul_ps(left_, right3_));
        res_._elements[i * 4 + 3] = _math_mat4_hadd(_mm_mul_ps(left_, right4_));
    }

    return res_;
}
Mat4 _math_mat4_mul_scalar(Mat4 *mat, Flt32 scalar) {
    Mat4 res_;

    __m128 scalar_ = _mm_set1_ps(scalar);
    __m128 row1_   = _mm_load_ps(&(mat->_elements[0]));
    __m128 row2_   = _mm_load_ps(&(mat->_elements[4]));
    __m128 row3_   = _mm_load_ps(&(mat->_elements[8]));
    __m128 row4_   = _mm_load_ps(&(mat->_elements[12]));

    _mm_store_ps(&(res_._elements[0]), _mm_mul_ps(row1_, scalar_));
    _mm_store_ps(&(res_._elements[4]), _mm_mul_ps(row2_, scalar_));
    _mm_store_ps(&(res_._elements[8]), _mm_mul_ps(row3_, scalar_));
    _mm_store_ps(&(res_._elements[12]), _mm_mul_ps(row4_, scalar_));

    return res_;
}
void _math_mat4_inverse(Mat4 *mat) {
    if (!mat || math_mat4_determinant(mat) == 0)
        return;

    __m128 row1_ = _mm_load_ps(&mat->_elements[0]);
    __m128 row2_ = _mm_load_ps(&mat->_elements[4]);
    __m128 row3_ = _mm_load_ps(&mat->_elements[8]);
    __m128 row4_ = _mm_load_ps(&mat->_elements[12]);

    __m128 mul12_ = _mm_mul_ps(row1_, row2_);
    __m128 mul34_ = _mm_mul_ps(row3_, row4_);
    __m128 sum_   = _mm_add_ps(mul12_, mul34_);

    Flt32 det_     = math_mat4_determinant(mat);
    Flt32 inv_det_ = 1.0f / det_;

    __m128 vec_inv_det_ = _mm_set1_ps(inv_det_);
    _mm_store_ps(&(mat->_elements[0]), _mm_mul_ps(sum_, vec_inv_det_));
}
Flt32 _math_mat4_determinant(Mat4 *mat) {
    Flt32 det_ = 0.0f;

    __m128 row0_      = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 row1_      = _mm_loadu_ps(&(mat->_elements[4]));
    __m128 row2_      = _mm_loadu_ps(&(mat->_elements[8]));
    __m128 row3_      = _mm_loadu_ps(&(mat->_elements[12]));
    __m128 minor0_    = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(row1_, row1_, _MM_SHUFFLE(1, 0, 3, 2)), _mm_shuffle_ps(row2_, row2_, _MM_SHUFFLE(2, 3, 0, 1))),
                                   _mm_mul_ps(_mm_shuffle_ps(row1_, row1_, _MM_SHUFFLE(2, 3, 0, 1)), _mm_shuffle_ps(row2_, row2_, _MM_SHUFFLE(1, 0, 3, 2))));
    __m128 cofactor0_ = _mm_mul_ps(minor0_, row3_);
    __m128 cof_sums_  = _mm_add_ps(cofactor0_, _mm_movehl_ps(cofactor0_, cofactor0_));
    cof_sums_         = _mm_add_ss(cof_sums_, _mm_shuffle_ps(cof_sums_, cof_sums_, _MM_SHUFFLE(1, 0, 0, 0)));

    _mm_store_ss(&det_, _mm_mul_ps(cof_sums_, row0_));
    return det_;
}
Vec3 _math_mat4_mat4_mul_vec3(Mat4 *mat, Vec3 *vec) {
    __m128 vec4_ = _mm_set_ps(1.0f, vec->_elements[2], vec->_elements[1], vec->_elements[0]);
    __m128 row0_ = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 row1_ = _mm_loadu_ps(&(mat->_elements[4]));
    __m128 row2_ = _mm_loadu_ps(&(mat->_elements[8]));

    return VT_UNION(Vec3, _mm_cvtss_f32(_math_mat4_dp(row0_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(row1_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(row2_, vec4_, 0xf1)));
}
Vec3 _math_mat4_vec3_mul_mat4(Vec3 *vec, Mat4 *mat) {
    __m128 vec4_ = _mm_set_ps(1.0f, vec->_elements[2], vec->_elements[1], vec->_elements[0]);
    __m128 col0_ = _mm_set_ps(mat->_elements[12], mat->_elements[8], mat->_elements[4], mat->_elements[0]);
    __m128 col1_ = _mm_set_ps(mat->_elements[13], mat->_elements[9], mat->_elements[5], mat->_elements[1]);
    __m128 col2_ = _mm_set_ps(mat->_elements[14], mat->_elements[10], mat->_elements[6], mat->_elements[2]);

    return VT_UNION(Vec3, _mm_cvtss_f32(_math_mat4_dp(col0_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(col1_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(col2_, vec4_, 0xf1)));
}
Vec4 _math_mat4_mat4_mul_vec4(Mat4 *mat, Vec4 *vec) {
    __m128 vec4_ = _mm_loadu_ps(vec->_elements);
    __m128 row0_ = _mm_loadu_ps(&mat->_elements[0]);
    __m128 row1_ = _mm_loadu_ps(&mat->_elements[4]);
    __m128 row2_ = _mm_loadu_ps(&mat->_elements[8]);
    __m128 row3_ = _mm_loadu_ps(&mat->_elements[12]);

    return VT_UNION(Vec4, _mm_cvtss_f32(_math_mat4_dp(row0_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(row1_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(row2_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(row3_, vec4_, 0xf1)));
}
Vec4 _math_mat4_vec4_mul_mat4(Vec4 *vec, Mat4 *mat) {
    __m128 vec4_ = _mm_loadu_ps(vec->_elements);
    __m128 col0_ = _mm_set_ps(mat->_elements[12], mat->_elements[8], mat->_elements[4], mat->_elements[0]);
    __m128 col1_ = _mm_set_ps(mat->_elements[13], mat->_elements[9], mat->_elements[5], mat->_elements[1]);
    __m128 col2_ = _mm_set_ps(mat->_elements[14], mat->_elements[10], mat->_elements[6], mat->_elements[2]);
    __m128 col3_ = _mm_set_ps(mat->_elements[15], mat->_elements[11], mat->_elements[7], mat->_elements[3]);

    return VT_UNION(Vec4, _mm_cvtss_f32(_math_mat4_dp(col0_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(col1_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(col2_, vec4_, 0xf1)), _mm_cvtss_f32(_math_mat4_dp(col3_, vec4_, 0xf1)));
}
Bool _math_mat4_equals(Mat4 *left, Mat4 *right) {
    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[4]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[8]));
    __m128 left3_  = _mm_loadu_ps(&(left->_elements[12]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[4]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[8]));
    __m128 right3_ = _mm_loadu_ps(&(right->_elements[12]));

    __m128 diff0_ = _mm_sub_ps(left0_, right0_);
    __m128 diff1_ = _mm_sub_ps(left1_, right1_);
    __m128 diff2_ = _mm_sub_ps(left2_, right2_);
    __m128 diff3_ = _mm_sub_ps(left3_, right3_);

    __m128 abs0_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff0_);
    __m128 abs1_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff1_);
    __m128 abs2_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff2_);
    __m128 abs3_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff3_);

    __m128 eps_  = _mm_set1_ps(VT_MATH_EPSILON);
    __m128 cmp0_ = _mm_cmple_ps(abs0_, eps_);
    __m128 cmp1_ = _mm_cmple_ps(abs1_, eps_);
    __m128 cmp2_ = _mm_cmple_ps(abs2_, eps_);
    __m128 cmp3_ = _mm_cmple_ps(abs3_, eps_);

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, _mm_and_ps(cmp2_, cmp3_)));
    return _math_mat4_test_all_ones(res_);
}
Bool _math_mat4_equals_approx(Mat4 *left, Mat4 *right, Flt32 limit) {
    __m128 left0_  = _mm_loadu_ps(&(left->_elements[0]));
    __m128 left1_  = _mm_loadu_ps(&(left->_elements[4]));
    __m128 left2_  = _mm_loadu_ps(&(left->_elements[8]));
    __m128 left3_  = _mm_loadu_ps(&(left->_elements[12]));
    __m128 right0_ = _mm_loadu_ps(&(right->_elements[0]));
    __m128 right1_ = _mm_loadu_ps(&(right->_elements[4]));
    __m128 right2_ = _mm_loadu_ps(&(right->_elements[8]));
    __m128 right3_ = _mm_loadu_ps(&(right->_elements[12]));

    __m128 diff0_ = _mm_sub_ps(left0_, right0_);
    __m128 diff1_ = _mm_sub_ps(left1_, right1_);
    __m128 diff2_ = _mm_sub_ps(left2_, right2_);
    __m128 diff3_ = _mm_sub_ps(left3_, right3_);

    __m128 abs0_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff0_);
    __m128 abs1_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff1_);
    __m128 abs2_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff2_);
    __m128 abs3_ = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff3_);

    __m128 limit_ = _mm_set1_ps(limit);
    __m128 cmp0_  = _mm_cmple_ps(abs0_, limit_);
    __m128 cmp1_  = _mm_cmple_ps(abs1_, limit_);
    __m128 cmp2_  = _mm_cmple_ps(abs2_, limit_);
    __m128 cmp3_  = _mm_cmple_ps(abs3_, limit_);

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, _mm_and_ps(cmp2_, cmp3_)));
    return _math_mat4_test_all_ones(res_);
}
Bool _math_mat4_is_zero(Mat4 *mat) {
    __m128 mat0_ = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 mat1_ = _mm_loadu_ps(&(mat->_elements[4]));
    __m128 mat2_ = _mm_loadu_ps(&(mat->_elements[8]));
    __m128 mat3_ = _mm_loadu_ps(&(mat->_elements[12]));

    __m128 cmp0_ = _mm_cmpeq_ps(mat0_, _mm_setzero_ps());
    __m128 cmp1_ = _mm_cmpeq_ps(mat1_, _mm_setzero_ps());
    __m128 cmp2_ = _mm_cmpeq_ps(mat2_, _mm_setzero_ps());
    __m128 cmp3_ = _mm_cmpeq_ps(mat3_, _mm_setzero_ps());

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, _mm_and_ps(cmp2_, cmp3_)));
    return _math_mat4_test_all_ones(res_);
}
Bool _math_mat4_is_identity(Mat4 *mat) {
    __m128 mat0_ = _mm_loadu_ps(&(mat->_elements[0]));
    __m128 mat1_ = _mm_loadu_ps(&(mat->_elements[4]));
    __m128 mat2_ = _mm_loadu_ps(&(mat->_elements[8]));
    __m128 mat3_ = _mm_loadu_ps(&(mat->_elements[12]));

    __m128 id0_ = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    __m128 id1_ = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    __m128 id2_ = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    __m128 id3_ = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

    __m128 cmp0_ = _mm_cmpeq_ps(mat0_, id0_);
    __m128 cmp1_ = _mm_cmpeq_ps(mat1_, id1_);
    __m128 cmp2_ = _mm_cmpeq_ps(mat2_, id2_);
    __m128 cmp3_ = _mm_cmpeq_ps(mat3_, id3_);

    __m128 res_ = _mm_and_ps(cmp0_, _mm_and_ps(cmp1_, _mm_and_ps(cmp2_, cmp3_)));
    return _math_mat4_test_all_ones(res_);
}

#else

Mat4 _math_mat4_add(Mat4 *left, Mat4 *right) {
    return VT_UNION(Mat4, left->_elements[0] + right->_elements[0], left->_elements[1] + right->_elements[1], left->_elements[2] + right->_elements[2], left->_elements[3] + right->_elements[3], left->_elements[4] + right->_elements[4],
                    left->_elements[5] + right->_elements[5], left->_elements[6] + right->_elements[6], left->_elements[7] + right->_elements[7], left->_elements[8] + right->_elements[8], left->_elements[9] + right->_elements[9],
                    left->_elements[10] + right->_elements[10], left->_elements[11] + right->_elements[11], left->_elements[12] + right->_elements[12], left->_elements[13] + right->_elements[13], left->_elements[14] + right->_elements[14],
                    left->_elements[15] + right->_elements[15]);
}
Mat4 _math_mat4_sub(Mat4 *left, Mat4 *right) {
    return VT_UNION(Mat4, left->_elements[0] - right->_elements[0], left->_elements[1] - right->_elements[1], left->_elements[2] - right->_elements[2], left->_elements[3] - right->_elements[3], left->_elements[4] - right->_elements[4],
                    left->_elements[5] - right->_elements[5], left->_elements[6] - right->_elements[6], left->_elements[7] - right->_elements[7], left->_elements[8] - right->_elements[8], left->_elements[9] - right->_elements[9],
                    left->_elements[10] - right->_elements[10], left->_elements[11] - right->_elements[11], left->_elements[12] - right->_elements[12], left->_elements[13] - right->_elements[13], left->_elements[14] - right->_elements[14],
                    left->_elements[15] - right->_elements[15]);
}
Mat4 _math_mat4_mul(Mat4 *left, Mat4 *right) {
    return VT_UNION(Mat4, (left->_elements[0] * right->_elements[0]) + (left->_elements[1] * right->_elements[4]) + (left->_elements[2] * right->_elements[8]) + (left->_elements[3] * right->_elements[12]),
                    (left->_elements[0] * right->_elements[1]) + (left->_elements[1] * right->_elements[5]) + (left->_elements[2] * right->_elements[9]) + (left->_elements[3] * right->_elements[13]),
                    (left->_elements[0] * right->_elements[2]) + (left->_elements[1] * right->_elements[6]) + (left->_elements[2] * right->_elements[10]) + (left->_elements[3] * right->_elements[14]),
                    (left->_elements[0] * right->_elements[3]) + (left->_elements[1] * right->_elements[7]) + (left->_elements[2] * right->_elements[11]) + (left->_elements[3] * right->_elements[15]),
                    (left->_elements[4] * right->_elements[0]) + (left->_elements[5] * right->_elements[4]) + (left->_elements[6] * right->_elements[8]) + (left->_elements[7] * right->_elements[12]),
                    (left->_elements[4] * right->_elements[1]) + (left->_elements[5] * right->_elements[5]) + (left->_elements[6] * right->_elements[9]) + (left->_elements[7] * right->_elements[13]),
                    (left->_elements[4] * right->_elements[2]) + (left->_elements[5] * right->_elements[6]) + (left->_elements[6] * right->_elements[10]) + (left->_elements[7] * right->_elements[14]),
                    (left->_elements[4] * right->_elements[3]) + (left->_elements[5] * right->_elements[7]) + (left->_elements[6] * right->_elements[11]) + (left->_elements[7] * right->_elements[15]),
                    (left->_elements[8] * right->_elements[0]) + (left->_elements[9] * right->_elements[4]) + (left->_elements[10] * right->_elements[8]) + (left->_elements[11] * right->_elements[12]),
                    (left->_elements[8] * right->_elements[1]) + (left->_elements[9] * right->_elements[5]) + (left->_elements[10] * right->_elements[9]) + (left->_elements[11] * right->_elements[13]),
                    (left->_elements[8] * right->_elements[2]) + (left->_elements[9] * right->_elements[6]) + (left->_elements[10] * right->_elements[10]) + (left->_elements[11] * right->_elements[14]),
                    (left->_elements[8] * right->_elements[3]) + (left->_elements[9] * right->_elements[7]) + (left->_elements[10] * right->_elements[11]) + (left->_elements[11] * right->_elements[15]),
                    (left->_elements[12] * right->_elements[0]) + (left->_elements[13] * right->_elements[4]) + (left->_elements[14] * right->_elements[8]) + (left->_elements[15] * right->_elements[12]),
                    (left->_elements[12] * right->_elements[1]) + (left->_elements[13] * right->_elements[5]) + (left->_elements[14] * right->_elements[9]) + (left->_elements[15] * right->_elements[13]),
                    (left->_elements[12] * right->_elements[2]) + (left->_elements[13] * right->_elements[6]) + (left->_elements[14] * right->_elements[10]) + (left->_elements[15] * right->_elements[14]),
                    (left->_elements[12] * right->_elements[3]) + (left->_elements[13] * right->_elements[7]) + (left->_elements[14] * right->_elements[11]) + (left->_elements[15] * right->_elements[15]));
}
Mat4 _math_mat4_mul_scalar(Mat4 *mat, Flt32 scalar) {
    return VT_UNION(Mat4, mat->_elements[0] * scalar, mat->_elements[1] * scalar, mat->_elements[2] * scalar, mat->_elements[3] * scalar, mat->_elements[4] * scalar, mat->_elements[5] * scalar, mat->_elements[6] * scalar, mat->_elements[7] * scalar,
                    mat->_elements[8] * scalar, mat->_elements[9] * scalar, mat->_elements[10] * scalar, mat->_elements[11] * scalar, mat->_elements[12] * scalar, mat->_elements[13] * scalar, mat->_elements[14] * scalar, mat->_elements[15] * scalar);
}
void _math_mat4_inverse(Mat4 *mat) {
    Mat4 mat_adj_ = VT_UNION(Mat4,
                             (mat->_elements[5] * mat->_elements[10] * mat->_elements[15] - mat->_elements[5] * mat->_elements[11] * mat->_elements[14] - mat->_elements[9] * mat->_elements[6] * mat->_elements[15] +
                              mat->_elements[9] * mat->_elements[7] * mat->_elements[14] + mat->_elements[13] * mat->_elements[6] * mat->_elements[11] - mat->_elements[13] * mat->_elements[7] * mat->_elements[10]),
                             (-mat->_elements[1] * mat->_elements[10] * mat->_elements[15] + mat->_elements[1] * mat->_elements[11] * mat->_elements[14] + mat->_elements[9] * mat->_elements[2] * mat->_elements[15] -
                              mat->_elements[9] * mat->_elements[3] * mat->_elements[14] - mat->_elements[13] * mat->_elements[2] * mat->_elements[11] + mat->_elements[13] * mat->_elements[3] * mat->_elements[10]),
                             (mat->_elements[1] * mat->_elements[6] * mat->_elements[15] - mat->_elements[1] * mat->_elements[7] * mat->_elements[14] - mat->_elements[5] * mat->_elements[2] * mat->_elements[15] +
                              mat->_elements[5] * mat->_elements[3] * mat->_elements[14] + mat->_elements[13] * mat->_elements[2] * mat->_elements[7] - mat->_elements[13] * mat->_elements[3] * mat->_elements[6]),
                             (-mat->_elements[1] * mat->_elements[6] * mat->_elements[11] + mat->_elements[1] * mat->_elements[7] * mat->_elements[10] + mat->_elements[5] * mat->_elements[2] * mat->_elements[11] -
                              mat->_elements[5] * mat->_elements[3] * mat->_elements[10] - mat->_elements[9] * mat->_elements[2] * mat->_elements[7] + mat->_elements[9] * mat->_elements[3] * mat->_elements[6]),
                             (-mat->_elements[4] * mat->_elements[10] * mat->_elements[15] + mat->_elements[4] * mat->_elements[11] * mat->_elements[14] + mat->_elements[8] * mat->_elements[6] * mat->_elements[15] -
                              mat->_elements[8] * mat->_elements[7] * mat->_elements[14] - mat->_elements[12] * mat->_elements[6] * mat->_elements[11] + mat->_elements[12] * mat->_elements[7] * mat->_elements[10]),
                             (mat->_elements[0] * mat->_elements[10] * mat->_elements[15] - mat->_elements[0] * mat->_elements[11] * mat->_elements[14] - mat->_elements[8] * mat->_elements[2] * mat->_elements[15] +
                              mat->_elements[8] * mat->_elements[3] * mat->_elements[14] + mat->_elements[12] * mat->_elements[2] * mat->_elements[11] - mat->_elements[12] * mat->_elements[3] * mat->_elements[10]),
                             (-mat->_elements[0] * mat->_elements[6] * mat->_elements[15] + mat->_elements[0] * mat->_elements[7] * mat->_elements[14] + mat->_elements[4] * mat->_elements[2] * mat->_elements[15] -
                              mat->_elements[4] * mat->_elements[3] * mat->_elements[14] - mat->_elements[12] * mat->_elements[2] * mat->_elements[7] + mat->_elements[12] * mat->_elements[3] * mat->_elements[6]),
                             (mat->_elements[0] * mat->_elements[6] * mat->_elements[11] - mat->_elements[0] * mat->_elements[7] * mat->_elements[10] - mat->_elements[4] * mat->_elements[2] * mat->_elements[11] +
                              mat->_elements[4] * mat->_elements[3] * mat->_elements[10] + mat->_elements[8] * mat->_elements[2] * mat->_elements[7] - mat->_elements[8] * mat->_elements[3] * mat->_elements[6]),
                             (mat->_elements[4] * mat->_elements[9] * mat->_elements[15] - mat->_elements[4] * mat->_elements[11] * mat->_elements[13] - mat->_elements[8] * mat->_elements[5] * mat->_elements[15] +
                              mat->_elements[8] * mat->_elements[7] * mat->_elements[13] + mat->_elements[12] * mat->_elements[5] * mat->_elements[11] - mat->_elements[12] * mat->_elements[7] * mat->_elements[9]),
                             (-mat->_elements[0] * mat->_elements[9] * mat->_elements[15] + mat->_elements[0] * mat->_elements[11] * mat->_elements[13] + mat->_elements[8] * mat->_elements[1] * mat->_elements[15] -
                              mat->_elements[8] * mat->_elements[3] * mat->_elements[13] - mat->_elements[12] * mat->_elements[1] * mat->_elements[11] + mat->_elements[12] * mat->_elements[3] * mat->_elements[9]),
                             (mat->_elements[0] * mat->_elements[5] * mat->_elements[15] - mat->_elements[0] * mat->_elements[7] * mat->_elements[13] - mat->_elements[4] * mat->_elements[1] * mat->_elements[15] +
                              mat->_elements[4] * mat->_elements[3] * mat->_elements[13] + mat->_elements[12] * mat->_elements[1] * mat->_elements[7] - mat->_elements[12] * mat->_elements[3] * mat->_elements[5]),
                             (-mat->_elements[0] * mat->_elements[5] * mat->_elements[11] + mat->_elements[0] * mat->_elements[7] * mat->_elements[9] + mat->_elements[4] * mat->_elements[1] * mat->_elements[11] -
                              mat->_elements[4] * mat->_elements[3] * mat->_elements[9] - mat->_elements[8] * mat->_elements[1] * mat->_elements[7] + mat->_elements[8] * mat->_elements[3] * mat->_elements[5]),
                             (-mat->_elements[4] * mat->_elements[9] * mat->_elements[14] + mat->_elements[4] * mat->_elements[10] * mat->_elements[13] + mat->_elements[8] * mat->_elements[5] * mat->_elements[14] -
                              mat->_elements[8] * mat->_elements[6] * mat->_elements[13] - mat->_elements[12] * mat->_elements[5] * mat->_elements[10] + mat->_elements[12] * mat->_elements[6] * mat->_elements[9]),
                             (mat->_elements[0] * mat->_elements[9] * mat->_elements[14] - mat->_elements[0] * mat->_elements[10] * mat->_elements[13] - mat->_elements[8] * mat->_elements[1] * mat->_elements[14] +
                              mat->_elements[8] * mat->_elements[2] * mat->_elements[13] + mat->_elements[12] * mat->_elements[1] * mat->_elements[10] - mat->_elements[12] * mat->_elements[2] * mat->_elements[9]),
                             (-mat->_elements[0] * mat->_elements[5] * mat->_elements[14] + mat->_elements[0] * mat->_elements[6] * mat->_elements[13] + mat->_elements[4] * mat->_elements[1] * mat->_elements[14] -
                              mat->_elements[4] * mat->_elements[2] * mat->_elements[13] - mat->_elements[12] * mat->_elements[1] * mat->_elements[6] + mat->_elements[12] * mat->_elements[2] * mat->_elements[5]),
                             (mat->_elements[0] * mat->_elements[5] * mat->_elements[10] - mat->_elements[0] * mat->_elements[6] * mat->_elements[9] - mat->_elements[4] * mat->_elements[1] * mat->_elements[10] +
                              mat->_elements[4] * mat->_elements[2] * mat->_elements[9] + mat->_elements[8] * mat->_elements[1] * mat->_elements[6] - mat->_elements[8] * mat->_elements[2] * mat->_elements[5]));

    *mat = math_mat4_mul_scalar(&mat_adj_, 1.0f / math_mat4_determinant(mat));
}
Flt32 _math_mat4_determinant(Mat4 *mat) {
    return (mat->_elements[0] * ((mat->_elements[5] * (mat->_elements[10] * mat->_elements[15] - mat->_elements[11] * mat->_elements[14])) - (mat->_elements[6] * (mat->_elements[9] * mat->_elements[15] - mat->_elements[11] * mat->_elements[13])) +
                                 (mat->_elements[7] * (mat->_elements[9] * mat->_elements[14] - mat->_elements[10] * mat->_elements[13]))) -
            mat->_elements[1] * ((mat->_elements[4] * (mat->_elements[10] * mat->_elements[15] - mat->_elements[11] * mat->_elements[14])) - (mat->_elements[6] * (mat->_elements[8] * mat->_elements[15] - mat->_elements[11] * mat->_elements[12])) +
                                 (mat->_elements[7] * (mat->_elements[8] * mat->_elements[14] - mat->_elements[10] * mat->_elements[12]))) +
            mat->_elements[2] * ((mat->_elements[4] * (mat->_elements[9] * mat->_elements[15] - mat->_elements[11] * mat->_elements[13])) - (mat->_elements[5] * (mat->_elements[8] * mat->_elements[15] - mat->_elements[11] * mat->_elements[12])) +
                                 (mat->_elements[7] * (mat->_elements[8] * mat->_elements[13] - mat->_elements[9] * mat->_elements[12]))) -
            mat->_elements[3] * ((mat->_elements[4] * (mat->_elements[9] * mat->_elements[14] - mat->_elements[10] * mat->_elements[13])) - (mat->_elements[5] * (mat->_elements[8] * mat->_elements[14] - mat->_elements[10] * mat->_elements[12])) +
                                 (mat->_elements[6] * (mat->_elements[8] * mat->_elements[13] - mat->_elements[9] * mat->_elements[12]))));
}
Vec3 _math_mat4_mat4_mul_vec3(Mat4 *mat, Vec3 *vec) {
    return VT_UNION(Vec3, (mat->_elements[0] * vec->_elements[0]) + (mat->_elements[1] * vec->_elements[1]) + (mat->_elements[2] * vec->_elements[2]) + mat->_elements[3],
                    (mat->_elements[4] * vec->_elements[0]) + (mat->_elements[5] * vec->_elements[1]) + (mat->_elements[6] * vec->_elements[2]) + mat->_elements[7],
                    (mat->_elements[8] * vec->_elements[0]) + (mat->_elements[9] * vec->_elements[1]) + (mat->_elements[10] * vec->_elements[2]) + mat->_elements[11]);
}
Vec3 _math_mat4_vec3_mul_mat4(Vec3 *vec, Mat4 *mat) {
    return VT_UNION(Vec3, (vec->_elements[0] * mat->_elements[0]) + (vec->_elements[1] * mat->_elements[4]) + (vec->_elements[2] * mat->_elements[8]) + mat->_elements[12],
                    (vec->_elements[0] * mat->_elements[1]) + (vec->_elements[1] * mat->_elements[5]) + (vec->_elements[2] * mat->_elements[9]) + mat->_elements[13],
                    (vec->_elements[0] * mat->_elements[2]) + (vec->_elements[1] * mat->_elements[6]) + (vec->_elements[2] * mat->_elements[10]) + mat->_elements[14]);
}
Vec4 _math_mat4_mat4_mul_vec4(Mat4 *mat, Vec4 *vec) {
    return VT_UNION(Vec4, (mat->_elements[0] * vec->_elements[0]) + (mat->_elements[1] * vec->_elements[1]) + (mat->_elements[2] * vec->_elements[2]) + (mat->_elements[3] * vec->_elements[3]),
                    (mat->_elements[4] * vec->_elements[0]) + (mat->_elements[5] * vec->_elements[1]) + (mat->_elements[6] * vec->_elements[2]) + (mat->_elements[7] * vec->_elements[3]),
                    (mat->_elements[8] * vec->_elements[0]) + (mat->_elements[9] * vec->_elements[1]) + (mat->_elements[10] * vec->_elements[2]) + (mat->_elements[11] * vec->_elements[3]),
                    (mat->_elements[12] * vec->_elements[0]) + (mat->_elements[13] * vec->_elements[1]) + (mat->_elements[14] * vec->_elements[2]) + (mat->_elements[15] * vec->_elements[3]));
}
Vec4 _math_mat4_vec4_mul_mat4(Vec4 *vec, Mat4 *mat) {
    return VT_UNION(Vec4, (vec->_elements[0] * mat->_elements[0]) + (vec->_elements[1] * mat->_elements[4]) + (vec->_elements[2] * mat->_elements[8]) + (vec->_elements[3] * mat->_elements[12]),
                    (vec->_elements[0] * mat->_elements[1]) + (vec->_elements[1] * mat->_elements[5]) + (vec->_elements[2] * mat->_elements[9]) + (vec->_elements[3] * mat->_elements[13]),
                    (vec->_elements[0] * mat->_elements[2]) + (vec->_elements[1] * mat->_elements[6]) + (vec->_elements[2] * mat->_elements[10]) + (vec->_elements[3] * mat->_elements[14]),
                    (vec->_elements[0] * mat->_elements[3]) + (vec->_elements[1] * mat->_elements[7]) + (vec->_elements[2] * mat->_elements[11]) + (vec->_elements[3] * mat->_elements[15]));
}
Bool _math_mat4_equals(Mat4 *left, Mat4 *right) {
    return ((left->_elements[0] - right->_elements[0]) < VT_MATH_EPSILON) && ((left->_elements[1] - right->_elements[1]) < VT_MATH_EPSILON) && ((left->_elements[2] - right->_elements[2]) < VT_MATH_EPSILON) &&
           ((left->_elements[3] - right->_elements[3]) < VT_MATH_EPSILON) && ((left->_elements[4] - right->_elements[4]) < VT_MATH_EPSILON) && ((left->_elements[5] - right->_elements[5]) < VT_MATH_EPSILON) &&
           ((left->_elements[6] - right->_elements[6]) < VT_MATH_EPSILON) && ((left->_elements[7] - right->_elements[7]) < VT_MATH_EPSILON) && ((left->_elements[8] - right->_elements[8]) < VT_MATH_EPSILON) &&
           ((left->_elements[9] - right->_elements[9]) < VT_MATH_EPSILON) && ((left->_elements[10] - right->_elements[10]) < VT_MATH_EPSILON) && ((left->_elements[11] - right->_elements[11]) < VT_MATH_EPSILON) &&
           ((left->_elements[12] - right->_elements[12]) < VT_MATH_EPSILON) && ((left->_elements[13] - right->_elements[13]) < VT_MATH_EPSILON) && ((left->_elements[14] - right->_elements[14]) < VT_MATH_EPSILON) &&
           ((left->_elements[15] - right->_elements[15]) < VT_MATH_EPSILON);
}
Bool _math_mat4_equals_approx(Mat4 *left, Mat4 *right, Flt32 limit) {
    return ((left->_elements[0] - right->_elements[0]) <= limit) && ((left->_elements[1] - right->_elements[1]) <= limit) && ((left->_elements[2] - right->_elements[2]) <= limit) && ((left->_elements[3] - right->_elements[3]) <= limit) &&
           ((left->_elements[4] - right->_elements[4]) <= limit) && ((left->_elements[5] - right->_elements[5]) <= limit) && ((left->_elements[6] - right->_elements[6]) <= limit) && ((left->_elements[7] - right->_elements[7]) <= limit) &&
           ((left->_elements[8] - right->_elements[8]) <= limit) && ((left->_elements[9] - right->_elements[9]) <= limit) && ((left->_elements[10] - right->_elements[10]) <= limit) && ((left->_elements[11] - right->_elements[11]) <= limit) &&
           ((left->_elements[12] - right->_elements[12]) <= limit) && ((left->_elements[13] - right->_elements[13]) <= limit) && ((left->_elements[14] - right->_elements[14]) <= limit) && ((left->_elements[15] - right->_elements[15]) <= limit);
}
Bool _math_mat4_is_zero(Mat4 *mat) {
    return (mat->_elements[0] == 0.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 0.0f) && (mat->_elements[5] == 0.0f) && (mat->_elements[6] == 0.0f) &&
           (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 0.0f) && (mat->_elements[9] == 0.0f) && (mat->_elements[10] == 0.0f) && (mat->_elements[11] == 0.0f) && (mat->_elements[12] == 0.0f) && (mat->_elements[13] == 0.0f) &&
           (mat->_elements[14] == 0.0f) && (mat->_elements[15] == 0.0f);
}
Bool _math_mat4_is_identity(Mat4 *mat) {
    return (mat->_elements[0] == 1.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 0.0f) && (mat->_elements[5] == 1.0f) && (mat->_elements[6] == 0.0f) &&
                        (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 0.0f) && (mat->_elements[9] == 0.0f) && (mat->_elements[10] == 1.0f) && (mat->_elements[11] == 0.0f) && (mat->_elements[12] == 0.0f) && (mat->_elements[13] == 0.0f) &&
                        (mat->_elements[14] == 0.0f) && (mat->_elements[15] == 1.0f)
}

#endif

Mat4 math_mat4_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x3y1, Flt32 x4y1, Flt32 x1y2, Flt32 x2y2, Flt32 x3y2, Flt32 x4y2, Flt32 x1y3, Flt32 x2y3, Flt32 x3y3, Flt32 x4y3, Flt32 x1y4, Flt32 x2y4, Flt32 x3y4, Flt32 x4y4) {
    return VT_UNION(Mat4, x1y1, x2y1, x3y1, x4y1, x1y2, x2y2, x3y2, x4y2, x1y3, x2y3, x3y3, x4y3, x1y4, x2y4, x3y4, x4y4);
}
Mat4 math_mat4_zero(void) { return VT_UNION(Mat4, 0.0f); }
Mat4 math_mat4_identity(void) { return VT_UNION(Mat4, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat4 math_mat4_copy(Mat4 *mat) { return !mat ? math_mat4_identity() : *mat; }
Mat4 math_mat4_from_vectors(Vec3 vec1, Vec3 vec2) {
    return VT_UNION(Mat4, vec1._elements[0] * vec2._elements[0], vec1._elements[0] * vec2._elements[1], vec1._elements[0] * vec2._elements[2], 0.0f, vec1._elements[1] * vec2._elements[0], vec1._elements[1] * vec2._elements[1],
                    vec1._elements[1] * vec2._elements[2], 0.0f, vec1._elements[2] * vec2._elements[0], vec1._elements[2] * vec2._elements[1], vec1._elements[2] * vec2._elements[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}
Mat4 math_mat4_add(Mat4 *left, Mat4 *right) { return (!left || !right) ? math_mat4_identity() : _math_mat4_add(left, right); }
Mat4 math_mat4_sub(Mat4 *left, Mat4 *right) { return (!left || !right) ? math_mat4_identity() : _math_mat4_sub(left, right); }
Mat4 math_mat4_mul(Mat4 *left, Mat4 *right) { return (!left || !right) ? math_mat4_identity() : _math_mat4_mul(left, right); }
Mat4 math_mat4_mul_scalar(Mat4 *mat, Flt32 scalar) { return (!mat) ? math_mat4_identity() : _math_mat4_mul_scalar(mat, scalar); }
Mat4 math_mat4_orthographic(Flt32 top, Flt32 bottom, Flt32 left, Flt32 right, Flt32 near, Flt32 far) {
    return VT_UNION(Mat4, (2.0f / (right - left)), 0.0f, 0.0f, 0.0f, 0.0f, (2.0f / (top - bottom)), 0.0f, 0.0f, 0.0f, 0.0f, (-2.0f / (far - near)), 0.0f, (((left + right) * -1.0f) / (right - left)), (((top + bottom) * -1.0f) / (top - bottom)),
                    (((near + far) * -1.0f) / (far - near)), 1.0f);
}
Mat4 math_mat4_perspective(Flt32 rad_fov, Flt32 ratio, Flt32 near, Flt32 far) {
    return VT_UNION(Mat4, (1.0f / (ratio * math_tan(rad_fov * 0.5f))), 0.0f, 0.0f, 0.0f, 0.0f, (1.0f / math_tan(rad_fov * 0.5f)), 0.0f, 0.0f, 0.0f, 0.0f, ((near + far) / (near - far)), -1.0f, 0.0f, 0.0f, ((2.0f * far * near) / (near - far)), 0.0f);
}
Mat4 math_mat4_look_at(Vec3 position, Vec3 target, Vec3 up) {
    const Vec3 z_ = math_vec3_normalized(math_vec3_sub(position, target));
    const Vec3 x_ = math_vec3_normalized(math_vec3_cross(up, z_));
    const Vec3 y_ = math_vec3_cross(z_, x_);

    return VT_UNION(Mat4, x_._elements[0], x_._elements[1], x_._elements[2], 0.0f, y_._elements[0], y_._elements[1], y_._elements[2], 0.0f, (z_._elements[0] * -1.0f), (z_._elements[1] * -1.0f), (z_._elements[2] * -1.0f), 0.0f, position._elements[0],
                    position._elements[1], position._elements[2], 1.0f);
}
void math_mat4_transpose(Mat4 *mat) {
    if (!mat)
        return;

    hal_mem_memswap(&(mat->_elements[1]), &(mat->_elements[4]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[2]), &(mat->_elements[8]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[3]), &(mat->_elements[12]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[6]), &(mat->_elements[9]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[7]), &(mat->_elements[13]), sizeof(Flt32));
    hal_mem_memswap(&(mat->_elements[11]), &(mat->_elements[14]), sizeof(Flt32));
}
Mat4 math_mat4_transposed(Mat4 *mat) {
    math_mat4_transpose(mat);
    return *mat;
}
Flt32 math_mat4_determinant(Mat4 *mat) { return (!mat) ? 0.0f : _math_mat4_determinant(mat); }
void  math_mat4_inverse(Mat4 *mat) {
    if (!mat || (math_mat4_determinant(mat) == 0))
        return;

    _math_mat4_inverse(mat);
}
Mat4 math_mat4_inversed(Mat4 *mat) {
    _math_mat4_inverse(mat);
    return *mat;
}
Mat4 math_mat4_make_translation(Vec3 position) { return VT_UNION(Mat4, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, position._elements[0], position._elements[1], position._elements[2], 1.0f); }
Mat4 math_mat4_make_rotation_x(Flt32 rad_angle) { return VT_UNION(Mat4, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, math_cos(rad_angle), math_sign(rad_angle), 0.0f, 0.0f, (math_sin(rad_angle) * -1.0f), math_cos(rad_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat4 math_mat4_make_rotation_y(Flt32 rad_angle) { return VT_UNION(Mat4, math_cos(rad_angle), 0.0f, (math_sin(rad_angle) * -1.0f), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, math_sin(rad_angle), 0.0f, math_cos(rad_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat4 math_mat4_make_rotation_z(Flt32 rad_angle) { return VT_UNION(Mat4, math_cos(rad_angle), math_sin(rad_angle), 0.0f, 0.0f, (math_sin(rad_angle) * -1.0f), math_cos(rad_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat4 math_mat4_make_rotation_xyz(Flt32 rad_x, Flt32 rad_y, Flt32 rad_z) {
    Mat4 mat_rot_x_   = math_mat4_make_rotation_x(rad_x);
    Mat4 mat_rot_y_   = math_mat4_make_rotation_y(rad_y);
    Mat4 mat_rot_z_   = math_mat4_make_rotation_z(rad_z);
    Mat4 mat_rot_xy_  = math_mat4_mul(&mat_rot_x_, &mat_rot_y_);
    Mat4 mat_rot_xyz_ = math_mat4_mul(&mat_rot_xy_, &mat_rot_z_);
    return mat_rot_xyz_;
}
Mat4 math_mat4_make_scale(Vec3 scale) { return VT_UNION(Mat4, scale._elements[0], 0.0f, 0.0f, 0.0f, 0.0f, scale._elements[1], 0.0f, 0.0f, 0.0f, 0.0f, scale._elements[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat4 math_mat4_transform(Vec3 translation, Vec3 rotation, Vec3 scale) {
    Mat4 mat_trans_           = math_mat4_make_translation(translation);
    Mat4 mat_rot_             = math_mat4_make_rotation_xyz(rotation._elements[0], rotation._elements[1], rotation._elements[2]);
    Mat4 mat_scale_           = math_mat4_make_scale(scale);
    Mat4 mat_rot_scale_       = math_mat4_mul(&mat_rot_, &mat_scale_);
    Mat4 mat_rot_scale_trans_ = math_mat4_mul(&mat_rot_scale_, &mat_trans_);
    return mat_rot_scale_trans_;
}
Vec3 math_mat4_make_forward_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[2] * -1.0f, mat->_elements[6] * -1.0f, mat->_elements[10] * -1.0f)); }
Vec3 math_mat4_make_backward_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[2], mat->_elements[6], mat->_elements[10])); }
Vec3 math_mat4_make_up_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[1], mat->_elements[5], mat->_elements[9])); }
Vec3 math_mat4_make_down_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[1] * -1.0f, mat->_elements[5] * -1.0f, mat->_elements[9] * -1.0f)); }
Vec3 math_mat4_make_left_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[0] * -1.0f, mat->_elements[4] * -1.0f, mat->_elements[8] * -1.0f)); }
Vec3 math_mat4_make_right_vector(Mat4 *mat) { return !mat ? math_vec3_zero() : math_vec3_normalized(math_vec3_construct(mat->_elements[0], mat->_elements[4], mat->_elements[8])); }
Vec3 math_mat4_mat4_mul_vec3(Mat4 *mat, Vec3 *vec) { return (!mat || !vec) ? math_vec3_zero() : _math_mat4_mat4_mul_vec3(mat, vec); }
Vec3 math_mat4_vec3_mul_mat4(Vec3 *vec, Mat4 *mat) { return (!mat || !vec) ? math_vec3_zero() : _math_mat4_vec3_mul_mat4(vec, mat); }
Vec4 math_mat4_mat4_mul_vec4(Mat4 *mat, Vec4 *vec) { return (!mat || !vec) ? math_vec4_zero() : _math_mat4_mat4_mul_vec4(mat, vec); }
Vec4 math_mat4_vec4_mul_mat4(Vec4 *vec, Mat4 *mat) { return (!mat || !vec) ? math_vec4_zero() : _math_mat4_vec4_mul_mat4(vec, mat); }
Vec4 math_mat4_get_row(Mat4 *mat, Int32 row) { return (!mat || (row < 0) || (row > 3)) ? math_vec4_zero() : VT_UNION(Vec4, mat->_elements[(row * 4) + 0], mat->_elements[(row * 4) + 1], mat->_elements[(row * 4) + 2], mat->_elements[(row * 4) + 3]); }
Vec4 math_mat4_get_column(Mat4 *mat, Int32 column) { return (!mat || (column < 0) || (column > 3)) ? math_vec4_zero() : VT_UNION(Vec4, mat->_elements[column + 0], mat->_elements[column + 4], mat->_elements[column + 8], mat->_elements[column + 12]); }
void math_mat4_set_row(Mat4 *mat, Int32 row, Vec4 *data) {
    if (!mat || !data || (row < 0) || (row > 3))
        return;

    hal_mem_memcpy(&(mat->_elements[row * 4]), data, sizeof(Vec4));
}
void math_mat4_set_column(Mat4 *mat, Int32 column, Vec4 *data) {
    if (!mat || !data || (column < 0) || (column > 3))
        return;

    mat->_elements[column + 0]  = data->_elements[0];
    mat->_elements[column + 4]  = data->_elements[1];
    mat->_elements[column + 8]  = data->_elements[2];
    mat->_elements[column + 12] = data->_elements[3];
}
Mat3 math_mat4_get_topleft_mat3(Mat4 *mat) {
    return (!mat) ? math_mat3_identity() : VT_UNION(Mat3, mat->_elements[0], mat->_elements[1], mat->_elements[2], mat->_elements[4], mat->_elements[5], mat->_elements[6], mat->_elements[8], mat->_elements[9], mat->_elements[10]);
}
void math_mat4_set_topleft_mat3(Mat4 *mat, Mat3 *data) {
    if (!mat || !data)
        return;

    mat->_elements[0]  = data->_elements[0];
    mat->_elements[1]  = data->_elements[1];
    mat->_elements[2]  = data->_elements[2];
    mat->_elements[4]  = data->_elements[3];
    mat->_elements[5]  = data->_elements[4];
    mat->_elements[6]  = data->_elements[5];
    mat->_elements[8]  = data->_elements[6];
    mat->_elements[9]  = data->_elements[7];
    mat->_elements[10] = data->_elements[8];
}
Bool math_mat4_equals(Mat4 *left, Mat4 *right) { return (!left || !right) ? false : _math_mat4_equals(left, right); }
Bool math_mat4_equals_approx(Mat4 *left, Mat4 *right, Flt32 limit) { return (!left || !right) ? false : _math_mat4_equals_approx(left, right, limit); }
Bool math_mat4_is_zero(Mat4 *mat) { return _math_mat4_is_zero(mat); }
Bool math_mat4_is_identity(Mat4 *mat) { return _math_mat4_is_identity(mat); }
