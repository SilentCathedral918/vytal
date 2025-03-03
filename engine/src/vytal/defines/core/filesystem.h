#pragma once

#include <stdio.h>

#include "types.h"

// return codes  -------------------------------------------------------- //

typedef enum File_Result {
    FILE_SUCCESS                   = 0,
    FILE_ERROR_INVALID_PARAM       = -1,
    FILE_ERROR_INVALID_MODE        = -2,
    FILE_ERROR_OPEN_FAILED         = -3,
    FILE_ERROR_ALREADY_OPEN        = -4,
    FILE_ERROR_NOT_OPEN            = -5,
    FILE_ERROR_CLOSE_FAILED        = -6,
    FILE_ERROR_READ_FAILED         = -7,
    FILE_ERROR_WRITE_FAILED        = -8,
    FILE_ERROR_IO                  = -9,
    FILE_ERROR_EOF                 = -10,
    FILE_ERROR_INSUFFICIENT_BUFFER = -11
} FileResult;

// modes ---------------------------------------------------------------- //

typedef enum Filesystem_File_IO_Mode {
    FILE_IO_MODE_READ,
    FILE_IO_MODE_WRITE,
    FILE_IO_MODE_READ_WRITE,
    FILE_IO_MODE_APPEND
} FileIOMode;

typedef enum Filesystem_File_Mode {
    FILE_MODE_TEXT,
    FILE_MODE_BINARY
} FileMode;

// structures ----------------------------------------------------------- //

typedef struct Filesystem_File {
    FILE *_stream;
    Bool  _active;
} File;
