#pragma once

#include "types.h"

// return codes --------------------------------------------------------- //

typedef enum Console_Result {
    CONSOLE_SUCCESS               = 0,
    CONSOLE_ERROR_INVALID_HANDLE  = -1,
    CONSOLE_ERROR_GET_MODE_FAILED = -2,
    CONSOLE_ERROR_SET_MODE_FAILED = -3,
    CONSOLE_ERROR_READ_FAILED     = -4
} ConsoleResult;

// colors --------------------------------------------------------------- //

typedef enum Console_Color {
    CONSOLE_COLOR_BLACK   = 0,
    CONSOLE_COLOR_RED     = 1,
    CONSOLE_COLOR_GREEN   = 2,
    CONSOLE_COLOR_YELLOW  = 3,
    CONSOLE_COLOR_BLUE    = 4,
    CONSOLE_COLOR_MAGENTA = 5,
    CONSOLE_COLOR_CYAN    = 6,
    CONSOLE_COLOR_WHITE   = 7
} ConsoleColor;
