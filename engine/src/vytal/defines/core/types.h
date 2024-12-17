#pragma once

#include <stdbool.h>
#include <stdint.h>

// ---------------------- signed integer types ---------------------- //

typedef int8_t  Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;

#if defined(__SIZEOF_INT128__)
typedef __int128_t Int128;
#endif

// ---------------------- unsigned integer types ---------------------- //

typedef uint8_t  UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;

#if defined(__SIZEOF_INT128__)
typedef __uint128_t UInt128;
#endif

// ---------------------- boolean type ---------------------- //

#if defined(bool)
typedef bool Bool;

#else
typedef unsigned char Bool;
#    define false VT_CAST(Bool, 0)
#    define true VT_CAST(Bool, 1)

#endif

// ---------------------- floating point types ---------------------- //

typedef float  Flt32;
typedef double Flt64;

// ---------------------- character-based types ---------------------- //

typedef char        Char;
typedef char       *Str;
typedef const char *ConstStr;

// ---------------------- memory types ---------------------- //

typedef size_t    ByteSize;
typedef int8_t   *BytePtr;
typedef uint8_t  *UBytePtr;
typedef void     *VoidPtr;
typedef size_t   *SizePtr;
typedef intptr_t  IntPtr;
typedef uintptr_t UIntPtr;
typedef ptrdiff_t PtrDiff;

// ---------------------- variable argument list ---------------------- //

#if defined(_MSC_VER)
typedef va_list VaList;

#else
typedef __builtin_va_list VaList;

#endif
