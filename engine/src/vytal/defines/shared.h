#pragma once

// import/export dll ---------------------------------------------------- //

#if defined(VYTAL_EXPORT_DLL)
#    if defined(_MSC_VER)
#        define VYTAL_API __declspec(dllexport)
#    else
#        define VYTAL_API __attribute__((visibility("default")))
#    endif

#else
#    if defined(_MSC_VER)
#        define VYTAL_API __declspec(dllimport)
#    else
#        define VYTAL_API
#    endif

#endif

// force-inlining ------------------------------------------------------- //

#if defined(_MSC_VER)
#    define VYTAL_INLINE __forceinline
#    define VYTAL_NOINLINE __declspec(noinline)

#elif defined(__clang__) || defined(__GNUC__)
#    define VYTAL_INLINE __attribute__((always_inline)) inline
#    define VYTAL_NOINLINE __attribute__((noinline))

#else
#    define VYTAL_INLINE static inline
#    define VYTAL_NOINLINE

#endif

// platform detection --------------------------------------------------- //

#if defined(_MSC_VER)  // MSVC
#    if !defined(_WIN64)
#        error "64-bit is required."
#    else
#        define VYTAL_PLATFORM_WINDOWS 1
#    endif

#elif defined(__MINGW32__)  // MinGW
#    if !defined(__MINGW64__)
#        error "64-bit is required."
#    else
#        define VYTAL_PLATFORM_WINDOWS 1
#    endif
#endif

// endian-check --------------------------------------------------------- //

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define IS_LITTLE_ENDIAN 1
#        define IS_BIG_ENDIAN 0
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#        define IS_LITTLE_ENDIAN 0
#        define IS_BIG_ENDIAN 1
#    else
#        error "Unknown endianness detected!"
#    endif

#elif defined(_WIN32) || defined(__LITTLE_ENDIAN__) || defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
// Windows and x86/x86-64 architectures are little-endian
#    define IS_LITTLE_ENDIAN 1
#    define IS_BIG_ENDIAN 0

#elif defined(__BIG_ENDIAN__) || defined(__sparc__) || defined(__powerpc__) || defined(__s390__)
// Common big-endian architectures
#    define IS_LITTLE_ENDIAN 0
#    define IS_BIG_ENDIAN 1

#else
#    include <stdint.h>
static inline int check_endianness(void) {
    uint16_t test = 0x0001;
    return (*(uint8_t*)&test == 0x01) ? 1 : 0;
}
#    define IS_LITTLE_ENDIAN check_endianness()
#    define IS_BIG_ENDIAN (!IS_LITTLE_ENDIAN)
#endif

// binary-swap ---------------------------------------------------------- //

#if defined(__GNUC__) || defined(__clang__)
#    define BSWAP64(x) __builtin_bswap64(x)
#else
#    define BSWAP64(x)                         \
        (((x >> 56) & 0x00000000000000FFULL) | \
         ((x >> 40) & 0x000000000000FF00ULL) | \
         ((x >> 24) & 0x0000000000FF0000ULL) | \
         ((x >> 8) & 0x00000000FF000000ULL) |  \
         ((x << 8) & 0x000000FF00000000ULL) |  \
         ((x << 24) & 0x0000FF0000000000ULL) | \
         ((x << 40) & 0x00FF000000000000ULL) | \
         ((x << 56) & 0xFF00000000000000ULL))
#endif
