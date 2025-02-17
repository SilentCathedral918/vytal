#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

typedef enum Application_Result {
    APP_SUCCESS                  = 0,
    APP_ERROR_PRECONSTRUCT_LOGIC = -1,
    APP_ERROR_CONSTRUCT_LOGIC    = -2,
    APP_ERROR_UPDATE_LOGIC       = -3,
    APP_ERROR_DESTRUCT_LOGIC     = -4,
} AppResult;

VYTAL_API AppResult application_preconstruct(void);
VYTAL_API AppResult application_construct(void);
VYTAL_API AppResult application_update(void);
VYTAL_API AppResult application_destruct(void);
