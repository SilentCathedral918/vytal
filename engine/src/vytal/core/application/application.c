#include "application.h"

#include <stdio.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/delay/delay.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/misc/console/console.h"

AppResult _application_core_startup(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (misc_console_startup() != CONSOLE_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult _application_core_shutdown(void) {
    if (misc_console_shutdown() != CONSOLE_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (memory_manager_shutdown() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (cvar_shutdown() != CVAR_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    exception_shutdown();

    return APP_SUCCESS;
}

AppResult application_preconstruct(void) {
    AppResult core_startup_ = _application_core_startup();
    if (core_startup_ != APP_SUCCESS)
        return core_startup_;

    return APP_SUCCESS;
}

AppResult application_construct(void) {
    return APP_SUCCESS;
}

AppResult application_update(void) {
    return APP_SUCCESS;
}

AppResult application_destruct(void) {
    AppResult core_shutdown_ = _application_core_shutdown();
    if (core_shutdown_ != APP_SUCCESS)
        return core_shutdown_;

    return APP_SUCCESS;
}
