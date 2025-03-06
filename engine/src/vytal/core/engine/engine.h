#pragma once

#include "vytal/assets/mesh/mesh.h"
#include "vytal/core/containers/string/string.h"
#include "vytal/core/delegates/unicast/unicast.h"
#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/core/platform/window/window.h"
#include "vytal/defines/shared.h"

typedef enum Engine_Result {
    ENGINE_SUCCESS                   = 0,
    ENGINE_ERROR_ALREADY_INITIALIZED = -1,
    ENGINE_ERROR_NOT_INITIALIZED     = -2,

    // preconstruct
    ENGINE_ERROR_PRECONSTRUCT_INVALID_PARAM                 = -100,
    ENGINE_ERROR_PRECONSTRUCT_FILE_OPEN_FAILED              = -101,
    ENGINE_ERROR_PRECONSTRUCT_FILE_CLOSE_FAILED             = -102,
    ENGINE_ERROR_PRECONSTRUCT_PARSE_FAILED                  = -103,
    ENGINE_ERROR_PRECONSTRUCT_MEMORY_MANAGER_STARTUP_FAILED = -104,
    ENGINE_ERROR_PRECONSTRUCT_CONSOLE_STARTUP_FAILED        = -105,
    ENGINE_ERROR_PRECONSTRUCT_LOGGER_STARTUP_FAILED         = -106,
    ENGINE_ERROR_PRECONSTRUCT_WINDOW_MODULE_STARTUP_FAILED  = -106,
    ENGINE_ERROR_PRECONSTRUCT_INPUT_MODULE_STARTUP_FAILED   = -107,
    ENGINE_ERROR_PRECONSTRUCT_ALLOCATION_FAILED             = -108,

    // construct

    // update
    ENGINE_ERROR_UPDATE_INPUT_MODULE_UPDATE_FAILED  = -300,
    ENGINE_ERROR_UPDATE_WINDOW_MODULE_UPDATE_FAILED = -301,

    // destruct
    ENGINE_ERROR_DESTRUCT_DEALLOCATION_FAILED = -400,

} EngineResult;

VYTAL_API EngineResult engine_preconstruct(ConstStr config_filepath);
VYTAL_API EngineResult engine_construct(void);
VYTAL_API EngineResult engine_update(void);
VYTAL_API EngineResult engine_destruct(void);
