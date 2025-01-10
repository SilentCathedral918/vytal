#pragma once

#include "types.h"

// ------------------------------------- vectors ------------------------------------- //

typedef union Math_Vector2 {
    struct {
        // first component
        union {
            Flt32 _x, _w, _u;
        };

        // second component
        union {
            Flt32 _y, _h, _v;
        };
    };

    Flt32 _elements[2];
} Vec2;

typedef union Math_Vector3 {
    struct {
        // first component
        union {
            Flt32 _x, _r;
        };

        // second component
        union {
            Flt32 _y, _g;
        };

        // third component
        union {
            Flt32 _z, _b;
        };
    };

    Flt32 _elements[3];
} Vec3;

typedef union Math_Vector4 {
    struct {
        // first component
        union {
            Flt32 _x, _r;
        };

        // second component
        union {
            Flt32 _y, _g;
        };

        // third component
        union {
            Flt32 _z, _b;
        };

        // fourth component
        union {
            Flt32 _w, _a;
        };
    };

    Flt32 _elements[4];
} Vec4;

// ------------------------------------- matrices ------------------------------------- //

typedef union Math_Matrix_2x2 {
    struct {
        Flt32 _x1y1;
        Flt32 _x2y1;

        Flt32 _x1y2;
        Flt32 _x2y2;
    };

    Flt32 _elements[4];
} Mat2;

typedef union Math_Matrix_3x3 {
    struct {
        Flt32 _x1y1;
        Flt32 _x2y1;
        Flt32 _x3y1;

        Flt32 _x1y2;
        Flt32 _x2y2;
        Flt32 _x3y2;

        Flt32 _x1y3;
        Flt32 _x2y3;
        Flt32 _x3y3;
    };

    Flt32 _elements[9];
} Mat3;

typedef union Math_Matrix_4x4 {
    struct {
        Flt32 _x1y1;
        Flt32 _x2y1;
        Flt32 _x3y1;
        Flt32 _x4y1;

        Flt32 _x1y2;
        Flt32 _x2y2;
        Flt32 _x3y2;
        Flt32 _x4y2;

        Flt32 _x1y3;
        Flt32 _x2y3;
        Flt32 _x3y3;
        Flt32 _x4y3;

        Flt32 _x1y4;
        Flt32 _x2y4;
        Flt32 _x3y4;
        Flt32 _x4y4;
    };

    Flt32 _elements[16];
} Mat4;

// ------------------------------------- quaternion ------------------------------------- //

typedef Vec4 Quat4;

// --------------------------------- macro utilities --------------------------------- //

// number of PI
#if defined(M_PI)
#    define VT_MATH_PI M_PI
#else
#    define VT_MATH_PI (3.14159265358979323846264338327950288)
#endif

// degree to radian conversion
#define VT_MATH_DEG_TO_RAD(degree) ((degree * VT_MATH_PI) / 180.0f)

// radian to degree conversion
#define VT_MATH_RAD_TO_DEG(radian) ((radian * 180.0f) / VT_MATH_PI)

// large number
#define VT_MATH_LARGE_NUMBER (3.4e38f)

// epsilon
#if defined(FLT_EPSILON)
#    define VT__MATH_EPSILON FLT_EPSILON
#else
#    define VT_MATH_EPSILON (1.1920928955078125e-07f)
#endif

// determine the larger value
#define VT_MATH_MAX(left, right) ((left > right) ? left : right)

// determine the smaller value
#define VT_MATH_MIN(left, right) ((left < right) ? left : right)

// constraint the value between specified boundaries
#define VT_MATH_CLAMP(value, min, max) ((value > max) ? max : ((value < min) ? min : value))
