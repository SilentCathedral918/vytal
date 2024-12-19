#include "window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/platform/window/window.h"

#define ENGINE_WINDOW_TITLE "Vytal Engine"
#define ENGINE_WINDOW_DEFAULT_WIDTH 1024
#define ENGINE_WINDOW_DEFAULT_HEIGHT 768
#define ENGINE_WINDOW_DEFAULT_BACKEND (WINDOW_BACKEND_GLFW)
#define ENGINE_WINDOW_DEFAULT_X (0x1fff0000u | (0))
#define ENGINE_WINDOW_DEFAULT_Y (0x1fff0000u | (0))

typedef struct Window_Module_State {
    PlatformWindow _main_window;
    WindowProps    _main_window_props;
} WindowModuleState;

static WindowModuleState *state = NULL;

// --------------------------- callbacks --------------------------- //

void _window_module_callback_on_window_close(VoidPtr context) {
    InputEventData data = {._event_code = VT_EVENTCODE_WINDOW_CLOSE};
    input_module_invoke_event(VT_EVENTCODE_WINDOW_CLOSE, &data);
}

void _window_module_callback_on_key_pressed(VoidPtr context, InputKeyCode key_code, Bool pressed) {
    InputKeyEventData data = {._event_code = pressed ? VT_EVENTCODE_KEY_PRESSED : VT_EVENTCODE_KEY_RELEASED,
                              ._key_code   = key_code};
    input_module_invoke_event(data._event_code, &data);
}

void _window_module_callback_on_mouse_pressed(VoidPtr context, InputMouseCode mouse_code, Bool pressed) {
    InputMouseEventData data = {._event_code = pressed ? VT_EVENTCODE_MOUSE_PRESSED : VT_EVENTCODE_MOUSE_RELEASED,
                                ._mouse_code = mouse_code};
    input_module_invoke_event(data._event_code, &data);
}

void _window_module_callback_on_mouse_moved(VoidPtr context, UInt16 x, UInt16 y) {
    InputMouseMoveEventData data = {._event_code = VT_EVENTCODE_MOUSE_MOVED, ._x = x, ._y = y};
    input_module_invoke_event(VT_EVENTCODE_MOUSE_MOVED, &data);
}

void _window_module_callback_on_mouse_scrolled(VoidPtr context, Int8 scroll_value) {
    InputMouseScrollEventData data = {._event_code = VT_EVENTCODE_MOUSE_SCROLLED, ._scroll_value = scroll_value};
    input_module_invoke_event(VT_EVENTCODE_MOUSE_SCROLLED, &data);
}

// ------------------------- window module  ------------------------- //

ByteSize window_module_get_size(void) { return sizeof(WindowModuleState); }

Bool window_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // window backend startup
    if (!platform_window_startup(ENGINE_WINDOW_DEFAULT_BACKEND))
        return false;

    WindowCallbacks callbacks_ = {._on_close          = _window_module_callback_on_window_close,
                                  ._on_key_pressed    = _window_module_callback_on_key_pressed,
                                  ._on_mouse_pressed  = _window_module_callback_on_mouse_pressed,
                                  ._on_mouse_moved    = _window_module_callback_on_mouse_moved,
                                  ._on_mouse_scrolled = _window_module_callback_on_mouse_scrolled};

    // window default properties
    WindowProps props_ = {._title      = ENGINE_WINDOW_TITLE,
                          ._x          = ENGINE_WINDOW_DEFAULT_X,
                          ._y          = ENGINE_WINDOW_DEFAULT_Y,
                          ._width      = ENGINE_WINDOW_DEFAULT_WIDTH,
                          ._height     = ENGINE_WINDOW_DEFAULT_HEIGHT,
                          ._callbacks  = callbacks_,
                          ._resizable  = true,
                          ._fullscreen = false};

    // assign module to state and init its members
    state = VT_CAST(WindowModuleState *, module);
    hal_mem_memcpy(&state->_main_window_props, &props_, sizeof(WindowProps));

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

    // window backend shutdown
    platform_window_shutdown();

    state = NULL;
    return true;
}

Bool window_module_update(void) {
    if (!state)
        return false;

    // poll events
    if (!platform_window_poll_events(state->_main_window))
        return false;

    // swap buffers
    if (!platform_window_swap_buffers(state->_main_window))
        return false;

    return true;
}

Bool window_module_construct_main(void) {
    if (!state)
        return false;

    // callbacks setup
    state->_main_window = platform_window_construct(state->_main_window_props, &(state->_main_window_props._callbacks));

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
