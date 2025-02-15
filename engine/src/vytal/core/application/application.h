#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

typedef enum Application_Result {
    APP_SUCCESS,
    APP_ERROR_PRECONSTRUCT_LOGIC,
    APP_ERROR_CONSTRUCT_LOGIC,
    APP_ERROR_UPDATE_LOGIC,
    APP_ERROR_DESTRUCT_LOGIC,
} AppResult;

VYTAL_API AppResult application_preconstruct(void);
VYTAL_API AppResult application_construct(void);
VYTAL_API AppResult application_update(void);
VYTAL_API AppResult application_destruct(void);
