#pragma once

#include "vytal/core/logger/logger.h"

// debug break
#if !defined(VYTAL_DEBUG_BREAK)
#    if defined(_MSC_VER)
#        include <intrin.h>
#        define VYTAL_DEBUG_BREAK() __debugbreak()
#    elif defined(__GNUC__) || defined(__clang__)
#        define VYTAL_DEBUG_BREAK() __builtin_trap()
#    else
#        define VYTAL_DEBUG_BREAK() ((void)0)  // fallback: no-op
#    endif
#endif

// assertions
#if defined(VYTAL_ENABLE_ASSERTIONS)
#    define VYTAL_ASSERT(condition)                                    \
        do {                                                           \
            if (!(condition)) {                                        \
                VYTAL_LOG_FATAL("Assertion Failed: '%s'", #condition); \
                VYTAL_DEBUG_BREAK();                                   \
            }                                                          \
        } while (0)

#    define VYTAL_ASSERT_MESSAGE(condition, message)                                         \
        do {                                                                                 \
            if (!(condition)) {                                                              \
                VYTAL_LOG_FATAL("Assertion Failed: '%s', Message: %s", #condition, message); \
                VYTAL_DEBUG_BREAK();                                                         \
            }                                                                                \
        } while (0)

#else
#    define VYTAL_ASSERT(condition) ((void)0)                   // no-op
#    define VYTAL_ASSERT_MESSAGE(condition, message) ((void)0)  // no-op

#endif

// verification (for critical runtime checks, available on both debug and release modes)
#define VYTAL_VERIFY(condition)                                  \
    do {                                                         \
        if (!(condition)) {                                      \
            VYTAL_LOG_FATAL("Critical Error: '%s'", #condition); \
        }                                                        \
    } while (0)
