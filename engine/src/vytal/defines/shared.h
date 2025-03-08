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

// stack array size ----------------------------------------------------- //

#define VYTAL_ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))
