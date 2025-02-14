#pragma once

#include <setjmp.h>

#include "types.h"

// types ---------------------------------------------------------------- //

typedef enum Exception_Type {
    EXCEPTION_UNKNOWN,
    EXCEPTION_MEMORY,
    EXCEPTION_IO,
    EXCEPTION_LOGIC,
    EXCEPTION_THREAD
} ExceptionType;

// handle --------------------------------------------------------------- //

typedef struct Exception_Handle {
    ExceptionType _type;
    ConstStr      _message;
    ConstStr      _file;
    Int32         _line;
} Exception;

// handling frame ------------------------------------------------------- //

typedef struct Exception_Frame {
    jmp_buf   _buffer;
    Exception _exception;
} ExceptionFrame;