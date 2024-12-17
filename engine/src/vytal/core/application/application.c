#include "application.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/window/window.h"
#include "vytal/core/platform/window/window.h"
#include "vytal/managers/memory/memmgr.h"
#include "vytal/managers/module/modmgr.h"

#include <GLFW/glfw3.h>

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
    misc_console_writeln("vytal: %s", status);
    misc_console_reset();
}

VT_INLINE Int32 _application_window_active(GLFWwindow *window) { return !glfwWindowShouldClose(window); }
VT_INLINE void  _application_window_poll() { glfwPollEvents(); }
VT_INLINE void  _application_window_swapbuf(GLFWwindow *window) { glfwSwapBuffers(window); }

Bool application_preconstruct(void) {
    if (!_application_core_startup())
        return false;

    // allocate application state
    state = memory_manager_allocate(sizeof(Application_State), MEMORY_TAG_APPLICATION);

    // init state members
    {
        state->_initialized = true;
    }

    // perform modules startup
    if (!module_manager_startup_modules())
        return false;

    _application_report_status("pre_construct state completed, proceeding to construct stage...");
    return true;
}

Bool application_construct(void) {
    if (!state)
        return false;

    // construct main window
    if (!window_module_construct_main())
        return false;

    _application_report_status("construct state completed, proceeding to game loop...");
    return true;
}

Bool application_update(void) {
    if (!state)
        return false;

    GLFWwindow *win_ = platform_window_get(window_module_get_main());

    do {
        // swap buffer
        _application_window_swapbuf(win_);

        // poll events
        _application_window_poll();
    } while (_application_window_active(win_));

    _application_report_status("game loop terminated, proceeding to cleanup...");
    return true;
}

Bool application_destruct(void) {
    if (!state)
        return false;

    // perform modules shutdown
    if (!module_manager_shutdown_modules())
        return false;

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
