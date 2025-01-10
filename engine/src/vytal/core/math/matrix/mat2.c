#include "mat2.h"

#include "vytal/core/math/helper/math_helper.h"

Mat2 math_mat2_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x1y2, Flt32 x2y2) { return VT_UNION(Mat2, x1y1, x2y1, x1y2, x2y2); }
Mat2 math_mat2_zero(void) { return VT_UNION(Mat2, 0.0f); }
Mat2 math_mat2_identity(void) { return VT_UNION(Mat2, 1.0f, 0.0f, 0.0f, 1.0f); }
Mat2 math_mat2_copy(Mat2 *mat) { return !mat ? math_mat2_zero() : *mat; }
Mat2 math_mat2_mul(Mat2 left, Mat2 right) {
    return VT_UNION(Mat2, left._x1y1 * right._x1y1 + left._x2y1 * right._x1y2, left._x1y1 * right._x2y1 + left._x2y1 * right._x2y2, left._x1y2 * right._x1y1 + left._x2y2 * right._x1y2, left._x1y2 * right._x2y1 + left._x2y2 * right._x2y2);
}
Flt32 math_mat2_determinant(Mat2 mat) { return (mat._x1y1 * mat._x2y2 - mat._x2y1 * mat._x1y2); }
Mat2  math_mat2_inverse(Mat2 mat) {
    Flt32 det_ = math_mat2_determinant(mat);
    if (math_abs(det_) < VT_MATH_EPSILON)
        return math_mat2_zero();

    return VT_UNION(Mat2, mat._x2y2 / det_, (mat._x2y1 * -1.0f) / det_, (mat._x1y2 * -1.0f) / det_, mat._x1y1 / det_);
}
