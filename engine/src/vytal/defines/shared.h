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
