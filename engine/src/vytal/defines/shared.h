#pragma once

// ------------------------- import/export DLL ------------------------- //

#if defined(VT_EXPORT_DLL)
#    if defined(_MSC_VER)
#        define VT_API __declspec(dllexport)
#    else
#        define VT_API __attribute__((visibility("default")))
#    endif

#else
#    if defined(_MSC_VER)
#        define VT_API __declspec(dllimport)
#    else
#        define VT_API
#    endif

#endif

// ------------------------- explicit casting ------------------------- //

#define VT_CAST(type, value) ((type)(value))

// ------------------------- struct assignment ------------------------- //

#define VT_STRUCT(type, ...)                                                                                                                                                                                                                                   \
    (type) { __VA_ARGS__ }

// ------------------------- force-inlining ------------------------- //

#if defined(_MSC_VER)
#    define VT_INLINE __forceinline
#    define VT_NOINLINE __declspec(noinline)

#elif defined(__clang__) || defined(__GNUC__)
#    define VT_INLINE __attribute__((always_inline)) inline
#    define VT_NOINLINE __attribute__((noinline))

#else
#    define VT_INLINE static inline
#    define VT_NOINLINE

#endif

// ------------------------- static array size ------------------------- //

#define VT_STATIC_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

// ------------------------- platform detection  ------------------------- //

// --- Windows
#if defined(_WIN32) || defined(__MINGW32__)
#    if !defined(_WIN64) || !defined(__MINGW64__)
#        error "64-bit is required."
#    else
#        define VT_PLATFORM_WINDOWS 1
#    endif

// --- Linux
#elif defined(__linux__)
#    define VT_PLATFORM_LINUX 1

#endif

// ------------------------- disable warnings  ------------------------- //

#if defined(__GNUC__)
#    define VT_VAR_NOT_USED __attribute__((unused))
#else
#    define VT_VAR_NOT_USED
#endif
