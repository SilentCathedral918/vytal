#include "application.h"

#include <assert.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"

AppResult application_preconstruct(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (cvar_shutdown() != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}
