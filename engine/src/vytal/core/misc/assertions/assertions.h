#pragma once

#include "vytal/core/logger/logger.h"

#if !defined(VT_DEBUG_BREAK)

#    if defined(_MSC_VER)
#        include <intrin.h>
#        define VT_DEBUG_BREAK __debugbreak()

#    elif defined(__clang__) || defined(__GNUC__)
#        define VT_DEBUG_BREAK __builtin_trap()

#    endif

#endif

#if defined(VT_ENABLE_ASSERTIONS)

#    define VT_ASSERT(condition)                                                                                                                                                                                                                               \
        {                                                                                                                                                                                                                                                      \
            if (!(condition)) {                                                                                                                                                                                                                                \
                VT_LOG_FATAL("Engine", "Invalid Condition '%s'", #condition);                                                                                                                                                                                  \
                VT_DEBUG_BREAK;                                                                                                                                                                                                                                \
            }                                                                                                                                                                                                                                                  \
        }

#    define VT_ASSERT_MESSAGE(condition, message)                                                                                                                                                                                                              \
        {                                                                                                                                                                                                                                                      \
            if (!(condition)) {                                                                                                                                                                                                                                \
                VT_LOG_FATAL("Engine", "Invalid Condition '%s', Message: %s", #condition, message);                                                                                                                                                            \
                VT_DEBUG_BREAK;                                                                                                                                                                                                                                \
            }                                                                                                                                                                                                                                                  \
        }

#else

#    define VT_ASSERT(condition) condition
#    define VT_ASSERT_MESSAGE(condition, message) condition

#endif
