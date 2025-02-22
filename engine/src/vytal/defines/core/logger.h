#pragma once

#include "filesystem.h"
#include "memory.h"

// verbosity ------------------------------------------------------------ //

typedef enum Logger_Verbosity {
    LOG_VERBOSITY_FATAL,
    LOG_VERBOSITY_ERROR,
    LOG_VERBOSITY_WARNING,
    LOG_VERBOSITY_INFO,
    LOG_VERBOSITY_VERBOSE
} LoggerVerbosity;

// flags ---------------------------------------------------------------- //

typedef enum Logger_Flag {
    LOG_FLAG_NONE,
    LOG_FLAG_TIMESTAMP = VYTAL_BITFLAG_FIELD(0),
    LOG_FLAG_FILE_LINE = VYTAL_BITFLAG_FIELD(1),
    LOG_FLAG_FUNC_NAME = VYTAL_BITFLAG_FIELD(2)
} LoggerFlag;

// return codes --------------------------------------------------------- //

typedef enum Logger_Result {
    LOGGER_SUCCESS                               = 0,
    LOGGER_ERROR_STATE_NOT_INITIALIZED           = -1,
    LOGGER_ERROR_STATE_ALREADY_INITIALIZED       = -2,
    LOGGER_ERROR_MAP_CONSTRUCTION_FAILED         = -3,
    LOGGER_ERROR_MAP_DESTRUCTION_FAILED          = -4,
    LOGGER_ERROR_MAP_ITEM_ALLOCATION_FAILED      = -5,
    LOGGER_ERROR_MAP_ITEM_DEALLOCATION_FAILED    = -6,
    LOGGER_ERROR_INVALID_FORMAT                  = -7,
    LOGGER_ERROR_FILE_OPEN_FAILED                = -8,
    LOGGER_ERROR_FILE_CLOSE_FAILED               = -9,
    LOGGER_ERROR_FILE_INACTIVE_OR_INVALID_STREAM = -10,
    LOGGER_ERROR_FILE_WRITE_FAILED               = -11,
    LOGGER_ERROR_INVALID_PARAM                   = -12,
    LOGGER_ERROR_INVALID_LOGGER_NAME             = -13
} LoggerResult;

// handle --------------------------------------------------------------- //

typedef struct Logger_Handle *Logger;
