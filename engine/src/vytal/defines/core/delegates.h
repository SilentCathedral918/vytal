#pragma once

#include "containers.h"
#include "types.h"

typedef void (*Delegate_Function)(VoidPtr sender, VoidPtr listener, VoidPtr data);
typedef Delegate_Function DelegateFunction;

// uni-cast ------------------------------------------------------------------ //

typedef struct Delegate_Unicast_Handle {
    VoidPtr          _listener;
    DelegateFunction _callback;
} *UnicastDelegate;

// multi-cast ---------------------------------------------------------------- //

typedef struct Delegate_Multicast_Handle {
    VoidPtr _listener;
    Array   _callbacks;
} *MulticastDelegate;

// return codes -------------------------------------------------------------- //

typedef enum Delegate_Result {
    DELEGATE_SUCCESS                   = 0,
    DELEGATE_ERROR_ALREADY_INITIALIZED = -1,
    DELEGATE_ERROR_NOT_INITIALIZED     = -2,
    DELEGATE_ERROR_ALLOCATION_FAILED   = -3,
    DELEGATE_ERROR_DEALLOCATION_FAILED = -4,
    DELEGATE_ERROR_INVALID_PARAM       = -5,
    DELEGATE_ERROR_DATA_INSERT_FAILED  = -6,
    DELEGATE_ERROR_DATA_REMOVE_FAILED  = -7,
    DELEGATE_ERROR_DATA_NOT_EXIST      = -8,
    DELEGATE_ERROR_DATA_SEARCH_FAILED  = -9,
    DELEGATE_ERROR_INVALID_CALLBACK    = -10
} DelegateResult;
