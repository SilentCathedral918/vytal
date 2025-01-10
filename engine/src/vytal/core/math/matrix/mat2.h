#pragma once

#include "vytal/defines/core/math.h"
#include "vytal/defines/shared.h"

VT_API Mat2  math_mat2_construct(Flt32 x1y1, Flt32 x2y1, Flt32 x1y2, Flt32 x2y2);
VT_API Mat2  math_mat2_zero(void);
VT_API Mat2  math_mat2_identity(void);
VT_API Mat2  math_mat2_copy(Mat2 *mat);
VT_API Mat2  math_mat2_mul(Mat2 left, Mat2 right);
VT_API Flt32 math_mat2_determinant(Mat2 mat);
VT_API Mat2  math_mat2_inverse(Mat2 mat);
