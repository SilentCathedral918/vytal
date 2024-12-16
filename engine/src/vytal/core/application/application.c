#include "application.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/managers/memory/memmgr.h"

typedef struct Application_State {
    Bool _initialized;

} Application_State;

static Application_State *state = NULL;

Bool _application_core_startup(void) {
    // memory manager startup
    memory_manager_startup();

    // console startup
    misc_console_startup();

    // logger startup
    if (!logger_startup()) {
        misc_console_writeln("Logger startup failed.");
        return false;
    }

    return true;
}

Bool _application_core_shutdown(void) {
    // logger startup
    if (!logger_shutdown()) {
        misc_console_writeln("Logger shutdown failed.");
        return false;
    }

    // console startup
    misc_console_shutdown();

    // memory manager startup
    memory_manager_shutdown();

    return true;
}

void _application_report_status(ConstStr status) {
    misc_console_setforeground_rgb(101, 191, 104);
    misc_console_writeln("Application: %s", status);
    misc_console_reset();
}

Bool application_preconstruct(void) {
    if (!_application_core_startup())
        return false;

    // allocate application state
    state = memory_manager_allocate(sizeof(Application_State), MEMORY_TAG_APPLICATION);

    // init state members
    {
        state->_initialized = true;
    }

    _application_report_status("pre_construct state completed, proceeding to construct stage...");
    return true;
}

Bool application_construct(void) {

    _application_report_status("construct state completed, proceeding to game loop...");
    return true;
}

Bool application_update(void) {

    _application_report_status("game loop terminated, proceeding to cleanup...");
    return true;
}

Bool application_destruct(void) {
    // free and set state members to zero
    {
        hal_mem_memzero(state, sizeof(Application_State));
    }

    // free application state
    memory_manager_deallocate(state, MEMORY_TAG_APPLICATION);

    // memory manager shutdown
    if (!_application_core_shutdown())
        return false;

    _application_report_status("cleanup completed, exiting application...");
    return true;
}
