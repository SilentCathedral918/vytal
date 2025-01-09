#include "glfw_window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include <GLFW/glfw3.h>

#define OUTPUT_BUFFER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB

typedef struct Window_Platform_Struct {
    GLFWwindow *_handle;
} Window_Platform_Struct;

static WindowCallbacks *vytal_glfw_window_callbacks = NULL;

void _glfw_window_key_callback(GLFWwindow *window, Int32 key, Int32 scancode, Int32 action, Int32 mods) {
    InputKeyCode code_    = VT_CAST(InputKeyCode, key);
    Bool         pressed_ = (action != GLFW_RELEASE);

    if (vytal_glfw_window_callbacks->_on_key_pressed) {
        vytal_glfw_window_callbacks->_on_key_pressed(window, code_, pressed_);
    }
}

void _glfw_window_mouse_button_callback(GLFWwindow *window, Int32 button, Int32 action, Int32 mods) {
    InputMouseCode code_;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        code_ = VT_MOUSECODE_LEFT;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        code_ = VT_MOUSECODE_RIGHT;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        code_ = VT_MOUSECODE_MIDDLE;
        break;

    default:
        return;
    }

    Bool pressed_ = (action == GLFW_PRESS);

    if (vytal_glfw_window_callbacks->_on_mouse_pressed) {
        vytal_glfw_window_callbacks->_on_mouse_pressed(window, code_, pressed_);
    }
}

void _glfw_window_mouse_move_callback(GLFWwindow *window, Flt64 xpos, Flt64 ypos) {
    if (vytal_glfw_window_callbacks->_on_mouse_moved) {
        vytal_glfw_window_callbacks->_on_mouse_moved(window, VT_CAST(Int32, xpos), VT_CAST(Int32, ypos));
    }
}

void _glfw_window_mouse_scroll_callback(GLFWwindow *window, Flt64 xoffset, Flt64 yoffset) {
    if (vytal_glfw_window_callbacks->_on_mouse_scrolled) {
        vytal_glfw_window_callbacks->_on_mouse_scrolled(window, VT_CAST(Int8, yoffset));
    }
}

Bool glfw_window_startup(void) { return glfwInit() == GLFW_TRUE; }
void glfw_window_shutdown(void) { glfwTerminate(); }

PlatformWindow glfw_window_construct(const WindowProps props, const WindowCallbacks *callbacks, const UInt64 titlebar_flags) {
    if (!callbacks)
        return NULL;

    GLFWwindow *handle_ = glfwCreateWindow(props._width, props._height, props._title, props._fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!handle_)
        return NULL;

    PlatformWindow window = memory_manager_allocate(sizeof(Window_Platform_Struct), MEMORY_TAG_PLATFORM);
    window->_handle       = handle_;

    // callbacks setup
    {
        vytal_glfw_window_callbacks = VT_CAST(WindowCallbacks *, callbacks);

        glfwSetWindowCloseCallback(window->_handle, VT_CAST(GLFWwindowclosefun, vytal_glfw_window_callbacks->_on_close));
        glfwSetKeyCallback(window->_handle, VT_CAST(GLFWkeyfun, _glfw_window_key_callback));
        glfwSetMouseButtonCallback(window->_handle, VT_CAST(GLFWmousebuttonfun, _glfw_window_mouse_button_callback));
        glfwSetCursorPosCallback(window->_handle, VT_CAST(GLFWcursorposfun, _glfw_window_mouse_move_callback));
        glfwSetScrollCallback(window->_handle, VT_CAST(GLFWscrollfun, _glfw_window_mouse_scroll_callback));
    }

    return window;
}

Bool glfw_window_destruct(PlatformWindow window) {
    if (!window)
        return false;

    glfwDestroyWindow(window->_handle);
    window->_handle = NULL;

    // no need to free the platform window
    // let the memory manager handle the work
    return true;
}

VoidPtr glfw_window_get_handle(PlatformWindow window) { return !window ? NULL : VT_CAST(VoidPtr, window->_handle); }

ConstStr glfw_window_get_title(PlatformWindow window) { return !window ? "" : glfwGetWindowTitle(window->_handle); }

Int32 glfw_window_get_x(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 x_ = 0;
    glfwGetWindowPos(window->_handle, &x_, NULL);
    return x_;
}

Int32 glfw_window_get_y(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 y_ = 0;
    glfwGetWindowPos(window->_handle, NULL, &y_);
    return y_;
}

Int32 glfw_window_get_width(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 width_ = 0;
    glfwGetWindowSize(window->_handle, &width_, NULL);
    return width_;
}

Int32 glfw_window_get_height(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 height_ = 0;
    glfwGetWindowSize(window->_handle, NULL, &height_);
    return height_;
}

Flt64 glfw_window_get_frame(PlatformWindow window) {
    if (!window)
        return 0;

    return glfwGetTime();
}

Bool glfw_window_set_title(PlatformWindow window, ConstStr title) {
    if (!window)
        return false;

    glfwSetWindowTitle(window->_handle, title);
    return true;
}

Bool glfw_window_set_x(PlatformWindow window, const Int32 x) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, x, glfw_window_get_y(window));
    return true;
}

Bool glfw_window_set_y(PlatformWindow window, const Int32 y) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, glfw_window_get_x(window), y);
    return true;
}

Bool glfw_window_set_position(PlatformWindow window, const Int32 x, const Int32 y) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, x, y);
    return true;
}

Bool glfw_window_set_width(PlatformWindow window, const Int32 width) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, width, glfw_window_get_height(window));
    return true;
}

Bool glfw_window_set_height(PlatformWindow window, const Int32 height) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, glfw_window_get_width(window), height);
    return true;
}

Bool glfw_window_set_size(PlatformWindow window, const Int32 width, const Int32 height) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, width, height);
    return true;
}

Bool glfw_window_poll_events(PlatformWindow window) {
    if (!window)
        return false;

    glfwPollEvents();
    return true;
}

Bool glfw_window_swap_buffers(PlatformWindow window) {
    if (!window)
        return false;

    glfwSwapBuffers(window->_handle);
    return true;
}
