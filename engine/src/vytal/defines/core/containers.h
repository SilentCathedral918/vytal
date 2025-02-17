#pragma once

#include "types.h"

// return codes --------------------------------------------------------- //

typedef enum Container_Result {
    CONTAINER_SUCCESS                   = 0,
    CONTAINER_ERROR_INVALID_PARAM       = -1,
    CONTAINER_ERROR_NOT_ALLOCATED       = -2,
    CONTAINER_ERROR_ALLOCATION_FAILED   = -3,
    CONTAINER_ERROR_DEALLOCATION_FAILED = -4
} ContainerResult;

// types ---------------------------------------------------------------- //

typedef struct Container_String *String;
