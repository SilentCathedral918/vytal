#pragma once

#include "types.h"

#define VYTAL_MATH_MIN(left, right) ((left < right) ? left : right)
#define VYTAL_MATH_MAX(left, right) ((left > right) ? left : right)
#define VYTAL_MATH_CLAMP(value, min, max) ((value > max) ? max : ((value < min) ? min : value))
