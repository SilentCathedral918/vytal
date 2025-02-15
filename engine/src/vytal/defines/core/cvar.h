#pragma once

#include "hash.h"

// return codes --------------------------------------------------------- //

typedef enum Console_Variable_Result {
    CVAR_SUCCESS                 = 0,
    CVAR_ERROR_NOT_INITIALIZED   = -1,
    CVAR_ERROR_INVALID_PARAM     = -2,
    CVAR_ERROR_INVALID_DATA_TYPE = -3,
    CVAR_ERROR_MEMORY_ALLOCATION = -4,
    CVAR_ERROR_NOT_REGISTERED    = -5,
    CVAR_ERROR_SET_FAILED        = -6,
    CVAR_ERROR_FILE_OPEN_FAILED  = -7,
    CVAR_ERROR_FILE_CLOSE_FAILED = -8,
    CVAR_ERROR_FILE_READ_FAILED  = -9,
    CVAR_ERROR_FILE_WRITE_FAILED = -10
} CVarResult;

// types ---------------------------------------------------------------- //

typedef enum Console_Variable_Type {
    CVAR_TYPE_INT,
    CVAR_TYPE_FLOAT,
    CVAR_TYPE_BOOL,
    CVAR_TYPE_STRING
} CVarType;

typedef union Console_Variable_Data {
    Int32    _int;
    Flt32    _flt;
    Bool     _bool;
    ConstStr _str;
} CVarData;

// handle --------------------------------------------------------------- //

typedef struct Console_Variable_Handle {
    ConstStr _name;

    CVarType _type;
    CVarData _value;
    CVarData _default_value;

    Bool     _persistent;
    ConstStr _description;
} CVar;
