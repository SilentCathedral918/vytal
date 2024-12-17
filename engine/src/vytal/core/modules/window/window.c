#include "window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/platform/window/window.h"

#include <GLFW/glfw3.h>

#define ENGINE_WINDOW_TITLE "Vytal Engine"
#define ENGINE_WINDOW_DEFAULT_WIDTH 1024
#define ENGINE_WINDOW_DEFAULT_HEIGHT 768
#define ENGINE_WINDOW_DEFAULT_X (0x1fff0000u | (0))
#define ENGINE_WINDOW_DEFAULT_Y (0x1fff0000u | (0))

typedef struct Window_Module_State {
    PlatformWindow _main_window;
    WindowProps    _main_window_props;
} WindowModuleState;

static WindowModuleState *state = NULL;

ByteSize window_module_get_size(void) { return sizeof(WindowModuleState); }

Bool window_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // init GLFW
    if (!platform_window_startup())
        return false;

    // window default properties
    WindowProps props_ = {._title      = ENGINE_WINDOW_TITLE,
                          ._x          = ENGINE_WINDOW_DEFAULT_X,
                          ._y          = ENGINE_WINDOW_DEFAULT_Y,
                          ._width      = ENGINE_WINDOW_DEFAULT_WIDTH,
                          ._height     = ENGINE_WINDOW_DEFAULT_HEIGHT,
                          ._resizable  = true,
                          ._fullscreen = false};

    // assign module to state and init its members
    state                     = VT_CAST(WindowModuleState *, module);
    state->_main_window_props = props_;

    return true;
}

Bool window_module_shutdown(void) {
    if (!state)
        return false;

    // if the main window is yet to be freed
    // the module will do the work
    if (state->_main_window) {
        if (!platform_window_destruct(state->_main_window))
            return false;
    }

    // set all members to zero
    hal_mem_memzero(state, sizeof(WindowModuleState));

    // terminate GLFW
    platform_window_shutdown();

    state = NULL;
    return true;
}

Bool window_module_construct_main(void) {
    if (!state)
        return false;

    state->_main_window = platform_window_construct(state->_main_window_props);

    if (!state->_main_window)
        return false;

    return true;
}

Bool window_module_destruct_main(void) {
    if (!state)
        return false;

    if (!platform_window_destruct(state->_main_window))
        return false;

    state->_main_window = NULL;
    return true;
}

PlatformWindow window_module_get_main(void) {
    if (!state)
        return NULL;

    return state->_main_window;
}
