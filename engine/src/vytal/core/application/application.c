#include "application.h"

#include <stdio.h>
#include <string.h>

#include "vytal/core/configuration/cvar/cvar.h"
#include "vytal/core/hal/exception/exception.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/memory/manager/memory_manager.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/platform/window/window.h"

typedef struct Application_State {
    WindowBackend _window_backend;
    ByteSize      _memory_size;
} ApplicationState;

static ApplicationState *app_state;

AppResult _application_core_startup(void) {
    exception_startup();

    if (cvar_startup() != CVAR_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (memory_manager_startup() != MEMORY_MANAGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (console_startup() != CONSOLE_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    if (logger_startup() != LOGGER_SUCCESS)
        return APP_ERROR_PRECONSTRUCT_LOGIC;

    // allocate and configure application state
    {
        ByteSize         allocated_size_ = 0;
        MemoryZoneResult allocate_app_   = memory_zone_allocate("Application", sizeof(ApplicationState), (VoidPtr *)&app_state, &allocated_size_);
        if (allocate_app_ != MEMORY_ZONE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;
        memset(app_state, 0, sizeof(ApplicationState));

        app_state->_memory_size    = allocated_size_;
        app_state->_window_backend = WINDOW_BACKEND_GLFW;
    }

    if (window_startup(app_state->_window_backend) != WINDOW_SUCCESS) return APP_ERROR_PRECONSTRUCT_LOGIC;

    return APP_SUCCESS;
}

AppResult _application_core_shutdown(void) {
    if (window_shutdown() != WINDOW_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    // deallocate application state
    {
        MemoryZoneResult deallocate_app_ = memory_zone_deallocate("Application", app_state, app_state->_memory_size);
        if (deallocate_app_ != MEMORY_ZONE_SUCCESS)
            return APP_ERROR_PRECONSTRUCT_LOGIC;

        app_state = NULL;
    }

    if (logger_shutdown() != LOGGER_SUCCESS)
        return APP_ERROR_DESTRUCT_LOGIC;

    if (console_shutdown() != CONSOLE_SUCCESS)
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
