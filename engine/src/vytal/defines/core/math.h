#pragma once

#include "types.h"

#define VYTAL_MATH_MIN(left, right) ((left < right) ? left : right)
#define VYTAL_MATH_MAX(left, right) ((left > right) ? left : right)
#define VYTAL_MATH_CLAMP(value, min, max) ((value > max) ? max : ((value < min) ? min : value))

// vectors -------------------------------------------------------------- //

typedef Flt32 Vec2[2];
typedef Flt32 Vec3[3];
typedef Flt32 Vec4[4];

// matrices ------------------------------------------------------------- //

typedef Vec2 Mat2[2];
typedef Vec3 Mat3[3];
typedef Vec4 Mat4[4];

// quaternion ----------------------------------------------------------- //

typedef Mat4 Quat;
