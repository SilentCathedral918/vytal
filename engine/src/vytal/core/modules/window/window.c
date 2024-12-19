#include "window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"
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

// ------------------------- GLFW callbacks ------------------------- //

void _on_window_close(GLFWwindow *window) {
    InputEventData data_ = {._event_code = VT_EVENTCODE_APP_QUIT};
    input_module_invoke_event(VT_EVENTCODE_APP_QUIT, &data_);
}

void _on_key_pressed(GLFWwindow *window, Int32 key, Int32 scancode, Int32 action, Int32 mods) {
    Bool         pressed_ = (action != GLFW_RELEASE);
    InputKeyCode code_    = VT_CAST(InputKeyCode, key);
    input_module_process_key_pressed(code_, pressed_);
}

void _on_mouse_pressed(GLFWwindow *window, Int32 button, Int32 action, Int32 mods) {
    Bool           pressed_ = (action == GLFW_PRESS);
    InputMouseCode code_;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        code_ = VT_MOUSECODE_LEFT;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        code_ = VT_MOUSECODE_MIDDLE;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        code_ = VT_MOUSECODE_RIGHT;
        break;

    default:
        break;
    }

    input_module_process_mouse_pressed(code_, pressed_);
}

void _on_mouse_moved(GLFWwindow *window, Flt64 xpos, Flt64 ypos) {
    UInt16 x_ = VT_CAST(UInt16, xpos);
    UInt16 y_ = VT_CAST(UInt16, ypos);

    input_module_process_mouse_moved(x_, y_);
}

void _on_mouse_scrolled(GLFWwindow *window, Flt64 xoffset, Flt64 yoffset) {
    Int8 value_ = VT_CAST(Int8, yoffset);

    if (value_ != 0)
        input_module_process_mouse_scrolled(value_);
}

// ------------------------- window module  ------------------------- //

Bool _window_module_setup_callbacks(GLFWwindow *window) {
    if (!window)
        return false;

    glfwSetWindowCloseCallback(window, _on_window_close);
    glfwSetKeyCallback(window, _on_key_pressed);
    glfwSetMouseButtonCallback(window, _on_mouse_pressed);
    glfwSetCursorPosCallback(window, _on_mouse_moved);
    glfwSetScrollCallback(window, _on_mouse_scrolled);

    return true;
}

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

Bool window_module_update(void) {
    if (!state)
        return false;

    // poll GLFW events
    glfwPollEvents();

    // swap buffer
    glfwSwapBuffers(VT_CAST(GLFWwindow *, platform_window_get(state->_main_window)));

    return true;
}

Bool window_module_construct_main(void) {
    if (!state)
        return false;

    state->_main_window = platform_window_construct(state->_main_window_props);

    if (!state->_main_window)
        return false;

    // setup GLFW callbacks
    if (!_window_module_setup_callbacks(platform_window_get(state->_main_window)))
        return false;

    // associate GLFW window with the engine's context
    // glfwSetWindowUserPointer(platform_window_get(state->_main_window), state);

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
