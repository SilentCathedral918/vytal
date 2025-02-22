#pragma once

#include "vytal/core/logger/logger.h"

#if !defined(VYTAL_DEBUG_BREAK)
#    if defined(_MSC_VER)
#        include <intrin.h>
#        define VYTAL_DEBUG_BREAK __debugbreak()
#    elif defined(__clang__) || defined(__GNUC__)
#        define VYTAL_DEBUG_BREAK __builtin_trap()
#    else
#        include <stdlib.h>
#        define VYTAL_DEBUG_BREAK abort()
#    endif
#endif

#if defined(VYTAL_ENABLE_ASSERTIONS)
#    define VYTAL_ASSERT(condition)                                    \
        do {                                                           \
            if (!(condition)) {                                        \
                VYTAL_LOG_FATAL("Invalid Condition '%s'", #condition); \
                VYTAL_DEBUG_BREAK;                                     \
            }                                                          \
        } while (0)

#    define VYTAL_ASSERT_WITH_MESSAGE(condition, message)                                    \
        do {                                                                                 \
            if (!(condition)) {                                                              \
                VYTAL_LOG_FATAL("Invalid Condition '%s', Message: %s", #condition, message); \
                VYTAL_DEBUG_BREAK;                                                           \
            }                                                                                \
        } while (0)

#else
#    define VYTAL_ASSERT(condition) ((void)0)
#    define VYTAL_ASSERT_WITH_MESSAGE(condition, message) ((void)0)

#endif
