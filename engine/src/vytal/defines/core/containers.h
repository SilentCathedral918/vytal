#pragma once

#include "types.h"

// return codes --------------------------------------------------------- //

typedef enum Container_Result {
    CONTAINER_SUCCESS                   = 0,
    CONTAINER_ERROR_INVALID_PARAM       = -1,
    CONTAINER_ERROR_NOT_ALLOCATED       = -2,
    CONTAINER_ERROR_ALLOCATION_FAILED   = -3,
    CONTAINER_ERROR_DEALLOCATION_FAILED = -4,
    CONTAINER_ERROR_EMPTY_DATA          = -5,
    CONTAINER_ERROR_SIZES_MISMATCHED    = -6,

    // map container specifics
    CONTAINER_ERROR_MAP_KEY_ALREADY_EXISTS     = -7,
    CONTAINER_ERROR_MAP_KEY_NOT_FOUND          = -8,
    CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS = -9,
} ContainerResult;

// types ---------------------------------------------------------------- //

typedef struct Container_String *String;
typedef struct Container_Map    *Map;
