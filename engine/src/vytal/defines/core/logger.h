#pragma once

#include "ctnr.h"
#include "filesystem.h"
#include "mem.h"

// ------------------------------ verbosity ------------------------------ //

typedef enum Logger_LogVerbosity {
    LOG_VERBOSITY_FATAL,
    LOG_VERBOSITY_ERROR,
    LOG_VERBOSITY_WARNING,
    LOG_VERBOSITY_INFO,
    LOG_VERBOSITY_DEBUG,
    LOG_VERBOSITY_TRACE
} LogVerbosity;

// -------------------------------- flags -------------------------------- //

typedef enum Logger_LogFlag {
    LOG_FLAG_NONE      = VT_BITFLAG_FIELD(0),
    LOG_FLAG_TIMESTAMP = VT_BITFLAG_FIELD(1),
    LOG_FLAG_FILE_LINE = VT_BITFLAG_FIELD(2)
} LogFlag;

// -------------------------------- type -------------------------------- //

typedef struct Logger_Struct {
    Char       _id[24];
    FileHandle _file_handle;
} Logger;
