#include "mat3.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/math/helper/math_helper.h"
#include "vytal/core/math/vector/vec2.h"
#include "vytal/core/math/vector/vec3.h"

Mat3 math_mat3_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x3y1, Flt32 x1y2, Flt32 x2y2, Flt32 x3y2, Flt32 x1y3, Flt32 x2y3, Flt32 x3y3) { return VT_UNION(Mat3, x1y1, x2y1, x3y1, x1y2, x2y2, x3y2, x1y3, x2y3, x3y3); }
Mat3 math_mat3_zero(void) { return VT_UNION(Mat3, 0.0f); }
Mat3 math_mat3_identity(void) { return VT_UNION(Mat3, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
Mat3 math_mat3_copy(Mat3 *mat) { return !mat ? math_mat3_identity() : *mat; }
Mat3 math_mat3_from_vectors(Vec2 vec1, Vec2 vec2) {
    return VT_UNION(Mat3, vec1._elements[0] * vec2._elements[0], vec1._elements[0] * vec2._elements[1], 0.0f, vec1._elements[1] * vec2._elements[0], vec1._elements[1] * vec2._elements[1], 0.0f, 0.0f, 0.0f, 1.0f);
}
Mat3 math_mat3_add(Mat3 *left, Mat3 *right) {
    return (!left || !right) ? math_mat3_identity()
                             : VT_UNION(Mat3, left->_elements[0] + right->_elements[0], left->_elements[1] + right->_elements[1], left->_elements[2] + right->_elements[2], left->_elements[3] + right->_elements[3], left->_elements[4] + right->_elements[4],
                                        left->_elements[5] + right->_elements[5], left->_elements[6] + right->_elements[6], left->_elements[7] + right->_elements[7], left->_elements[8] + right->_elements[8]);
}
Mat3 math_mat3_sub(Mat3 *left, Mat3 *right) {
    return (!left || !right) ? math_mat3_identity()
                             : VT_UNION(Mat3, left->_elements[0] - right->_elements[0], left->_elements[1] - right->_elements[1], left->_elements[2] - right->_elements[2], left->_elements[3] - right->_elements[3], left->_elements[4] - right->_elements[4],
                                        left->_elements[5] - right->_elements[5], left->_elements[6] - right->_elements[6], left->_elements[7] - right->_elements[7], left->_elements[8] - right->_elements[8]);
}
Mat3 math_mat3_mul(Mat3 *left, Mat3 *right) {
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
Mat3 math_mat3_mul_scalar(Mat3 *mat, Flt32 scalar) {
    return !mat ? math_mat3_identity()
                : VT_UNION(Mat3, mat->_elements[0] * scalar, mat->_elements[1] * scalar, mat->_elements[2] * scalar, mat->_elements[3] * scalar, mat->_elements[4] * scalar, mat->_elements[5] * scalar, mat->_elements[6] * scalar, mat->_elements[7] * scalar,
                           mat->_elements[8] * scalar);
}
Vec2 math_mat3_mat3_mul_vec2(Mat3 *mat, Vec2 *vec) {
    return (!mat || !vec) ? math_vec2_zero() : VT_UNION(Vec2, (mat->_elements[0] * vec->_elements[0]) + (mat->_elements[1] * vec->_elements[1]), (mat->_elements[3] * vec->_elements[0]) + (mat->_elements[4] * vec->_elements[1]));
}
Vec2 math_mat3_vec2_mul_mat3(Vec2 *vec, Mat3 *mat) {
    return (!mat || !vec) ? math_vec2_zero()
                          : VT_UNION(Vec2, ((vec->_elements[0] * mat->_elements[0]) + (vec->_elements[1] * mat->_elements[3]) + mat->_elements[6]), ((vec->_elements[0] * mat->_elements[1]) + (vec->_elements[1] * mat->_elements[4]) + mat->_elements[6]));
}
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

    Flt32 det_        = math_mat3_determinant(mat);
    Mat3  transposed_ = math_mat3_transposed(mat);

    Mat3 mat_adj_ = VT_UNION(Mat3, transposed_._elements[4] * transposed_._elements[8] - transposed_._elements[5] * transposed_._elements[7], -transposed_._elements[3] * transposed_._elements[8] + transposed_._elements[5] * transposed_._elements[6],
                             transposed_._elements[3] * transposed_._elements[7] - transposed_._elements[4] * transposed_._elements[6], -transposed_._elements[1] * transposed_._elements[8] + transposed_._elements[2] * transposed_._elements[7],
                             transposed_._elements[0] * transposed_._elements[8] - transposed_._elements[2] * transposed_._elements[6], -transposed_._elements[0] * transposed_._elements[7] + transposed_._elements[1] * transposed_._elements[6],
                             transposed_._elements[1] * transposed_._elements[5] - transposed_._elements[2] * transposed_._elements[4], -transposed_._elements[0] * transposed_._elements[5] + transposed_._elements[2] * transposed_._elements[3],
                             transposed_._elements[0] * transposed_._elements[4] - transposed_._elements[1] * transposed_._elements[3]);

    *mat = math_mat3_mul_scalar(&mat_adj_, 1.0f / det_);
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
Bool math_mat3_equals(Mat3 *left, Mat3 *right) {
    return (!left || !right) ? false
                             : ((left->_elements[0] - right->_elements[0]) < VT_MATH_EPSILON) && ((left->_elements[1] - right->_elements[1]) < VT_MATH_EPSILON) && ((left->_elements[2] - right->_elements[2]) < VT_MATH_EPSILON) &&
                                   ((left->_elements[3] - right->_elements[3]) < VT_MATH_EPSILON) && ((left->_elements[4] - right->_elements[4]) < VT_MATH_EPSILON) && ((left->_elements[5] - right->_elements[5]) < VT_MATH_EPSILON) &&
                                   ((left->_elements[6] - right->_elements[6]) < VT_MATH_EPSILON) && ((left->_elements[7] - right->_elements[7]) < VT_MATH_EPSILON) && ((left->_elements[8] - right->_elements[8]) < VT_MATH_EPSILON);
}
Bool math_mat3_equals_approx(Mat3 *left, Mat3 *right, Flt32 limit) {
    return (!left || !right) ? false
                             : ((left->_elements[0] - right->_elements[0]) <= limit) && ((left->_elements[1] - right->_elements[1]) <= limit) && ((left->_elements[2] - right->_elements[2]) <= limit) &&
                                   ((left->_elements[3] - right->_elements[3]) <= limit) && ((left->_elements[4] - right->_elements[4]) <= limit) && ((left->_elements[5] - right->_elements[5]) <= limit) &&
                                   ((left->_elements[6] - right->_elements[6]) <= limit) && ((left->_elements[7] - right->_elements[7]) <= limit) && ((left->_elements[8] - right->_elements[8]) <= limit);
}
Bool math_mat3_is_zero(Mat3 *mat) {
    return (!mat) ? false
                  : (mat->_elements[0] == 0.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 0.0f) && (mat->_elements[5] == 0.0f) && (mat->_elements[6] == 0.0f) &&
                        (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 0.0f);
}
Bool math_mat3_is_identity(Mat3 *mat) {
    return (!mat) ? false
                  : (mat->_elements[0] == 1.0f) && (mat->_elements[1] == 0.0f) && (mat->_elements[2] == 0.0f) && (mat->_elements[3] == 0.0f) && (mat->_elements[4] == 1.0f) && (mat->_elements[5] == 0.0f) && (mat->_elements[6] == 0.0f) &&
                        (mat->_elements[7] == 0.0f) && (mat->_elements[8] == 1.0f);
}
