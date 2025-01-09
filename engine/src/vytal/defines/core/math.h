#pragma once

#include "types.h"

// ------------------------------------- vectors ------------------------------------- //

typedef union Math_Vector2_Float {
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
} Vec2f;

typedef union Math_Vector3_Float {
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
} Vec3f;

typedef union Math_Vector4_Float {
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
} Vec4f;

typedef union Math_Vector2_Integer {
    struct {
        // first component
        union {
            Int32 _x, _w, _u;
        };

        // second component
        union {
            Int32 _y, _h, _v;
        };
    };

    Int32 _elements[2];
} Vec2i;

typedef union Math_Vector3_Integer {
    struct {
        // first component
        union {
            Int32 _x, _r;
        };

        // second component
        union {
            Int32 _y, _g;
        };

        // third component
        union {
            Int32 _z, _b;
        };
    };

    Int32 _elements[3];
} Vec3i;

typedef union Math_Vector4_Integer {
    struct {
        // first component
        union {
            Int32 _x, _r;
        };

        // second component
        union {
            Int32 _y, _g;
        };

        // third component
        union {
            Int32 _z, _b;
        };

        // fourth component
        union {
            Int32 _w, _a;
        };
    };

    Int32 _elements[4];
} Vec4i;

// ------------------------------------- matrices ------------------------------------- //

typedef union Math_Matrix_2x2_Float {
    struct {
        Flt32 _x1y1;
        Flt32 _x2y1;

        Flt32 _x1y2;
        Flt32 _x2y2;
    };

    Flt32 _elements[4];
} Mat2f;

typedef union Math_Matrix_3x3_Float {
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
} Mat3f;

typedef union Math_Matrix_4x4_Float {
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
} Mat4f;

typedef union Math_Matrix_2x2_Integer {
    struct {
        Int32 _x1y1;
        Int32 _x2y1;

        Int32 _x1y2;
        Int32 _x2y2;
    };

    Int32 _elements[4];
} Mat2i;

typedef union Math_Matrix_3x3_Integer {
    struct {
        Int32 _x1y1;
        Int32 _x2y1;
        Int32 _x3y1;

        Int32 _x1y2;
        Int32 _x2y2;
        Int32 _x3y2;

        Int32 _x1y3;
        Int32 _x2y3;
        Int32 _x3y3;
    };

    Int32 _elements[9];
} Mat3i;

typedef union Math_Matrix_4x4_Integer {
    struct {
        Int32 _x1y1;
        Int32 _x2y1;
        Int32 _x3y1;
        Int32 _x4y1;

        Int32 _x1y2;
        Int32 _x2y2;
        Int32 _x3y2;
        Int32 _x4y2;

        Int32 _x1y3;
        Int32 _x2y3;
        Int32 _x3y3;
        Int32 _x4y3;

        Int32 _x1y4;
        Int32 _x2y4;
        Int32 _x3y4;
        Int32 _x4y4;
    };

    Int32 _elements[16];
} Mat4i;

// ------------------------------------- quaternion ------------------------------------- //

typedef Vec4f Quat4f;
typedef Vec4i Quat4i;

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
