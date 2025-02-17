#pragma once

#if defined(_MSC_VER)
#    include <windows.h>
#elif defined(__GNUC__) || defined(__clang__)
#    include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "vytal/defines/core/exception.h"
#include "vytal/defines/shared.h"

#if !defined(LINE_BUFFER_MAX_SIZE)
#    define LINE_BUFFER_MAX_SIZE 8192
#endif

#if defined(_MSC_VER)
static DWORD exception_stack_key;
static DWORD exception_stack_top_key;

#    pragma section(".CRT$XCU", read)
void exception_startup(void);
__declspec(allocate(".CRT$XCU")) void (*exception_startup_ptr)(void) = exception_startup;

#elif defined(__GNUC__) || defined(__clang__)
static __attribute__((unused)) pthread_key_t exception_stack_key;
static __attribute__((unused)) pthread_key_t exception_stack_top_key;

__attribute__((constructor)) void exception_startup(void);

#endif

VYTAL_API VYTAL_INLINE ConstStr exception_type_to_string(ExceptionType type) {
    switch (type) {
        case EXCEPTION_MEMORY:
            return "MEMORY";

        case EXCEPTION_IO:
            return "IO";

        case EXCEPTION_LOGIC:
            return "LOGIC";

        case EXCEPTION_THREAD:
            return "THREAD";

        default:
            return "UNKNOWN";
    }
}

VYTAL_API void            exception_startup(void);
VYTAL_API void            exception_shutdown(void);
VYTAL_API ExceptionFrame *exception_get_stack(void);
VYTAL_API Int32          *exception_get_stack_top(void);

#define try_catch(try_block, catch_block, finally_block)                 \
    {                                                                    \
        Int32 *exception_top_ = exception_get_stack_top();               \
        if (++(*exception_top_) >= MAX_EXCEPTION_DEPTH) {                \
            fprintf(stderr, "Error: Too many nested try-catch!\n");      \
            exit(EXIT_FAILURE);                                          \
        }                                                                \
        if (setjmp(exception_get_stack()[*exception_top_]._buffer) == 0) \
            try_block else catch_block;                                  \
        --(*exception_top_);                                             \
        finally_block;                                                   \
    }

#define throw_exception(type, error_message, ...)                                \
    {                                                                            \
        Char formatted_message[LINE_BUFFER_MAX_SIZE];                            \
        snprintf(formatted_message, sizeof(formatted_message), error_message,    \
                 ##__VA_ARGS__);                                                 \
        fprintf(stderr, "Exception Thrown (%s) in %s:%d: %s\n",                  \
                exception_type_to_string(type), __FILE__, __LINE__,              \
                formatted_message);                                              \
        Int32 *exception_top_ = exception_get_stack_top();                       \
        if (*exception_top_ >= 0) {                                              \
            ExceptionFrame *stack_                      = exception_get_stack(); \
            stack_[*exception_top_]._exception._type    = type;                  \
            stack_[*exception_top_]._exception._message = formatted_message;     \
            stack_[*exception_top_]._exception._file    = __FILE__;              \
            stack_[*exception_top_]._exception._line    = __LINE__;              \
            longjmp(stack_[*exception_top_]._buffer, 1);                         \
        } else {                                                                 \
            fprintf(stderr, "Unhandled exception! Exiting...\n");                \
            exit(EXIT_FAILURE);                                                  \
        }                                                                        \
    }
