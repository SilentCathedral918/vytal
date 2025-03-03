#pragma once

#include "types.h"

// parse return codes --------------------------------------------------- //

typedef enum Parse_Result {
    PARSE_SUCCESS             = 0,
    PARSE_ERROR_INVALID_PARAM = -1,
    PARSE_ERROR_EMPTY_STRING  = -2
} ParseResult;
