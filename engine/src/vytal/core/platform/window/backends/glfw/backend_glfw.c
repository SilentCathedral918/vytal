#include "backend_glfw.h"

#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vytal/core/containers/string/string.h"
#include "vytal/core/memory/zone/memory_zone.h"

struct Window_Handle {
    GLFWwindow *_handle;
    ByteSize    _memory_size;
};

static Bool             backend_glfw_initialized = false;
static WindowCallbacks *backend_glfw_callbacks   = NULL;

void _backend_glfw_key_callback(GLFWwindow *window, Int32 key, Int32 scancode, Int32 action, Int32 mods) {
    InputKeyCode code_    = (InputKeyCode)key;
    Bool         pressed_ = (action != GLFW_RELEASE);

    if (backend_glfw_callbacks->_on_key_pressed)
        backend_glfw_callbacks->_on_key_pressed(window, code_, pressed_);
}

void _backend_glfw_mouse_button_callback(GLFWwindow *window, Int32 button, Int32 action, Int32 mods) {
    InputMouseCode code_;

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            code_ = VYTAL_MOUSECODE_LEFT;
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            code_ = VYTAL_MOUSECODE_RIGHT;
            break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            code_ = VYTAL_MOUSECODE_MIDDLE;
            break;

        default:
            return;
    }

    Bool pressed_ = (action == GLFW_PRESS);

    if (backend_glfw_callbacks->_on_mouse_pressed)
        backend_glfw_callbacks->_on_mouse_pressed(window, code_, pressed_);
}

void _backend_glfw_mouse_move_callback(GLFWwindow *window, Flt64 xpos, Flt64 ypos) {
    if (backend_glfw_callbacks->_on_mouse_moved)
        backend_glfw_callbacks->_on_mouse_moved(window, (Int32)xpos, (Int32)ypos);
}

void _backend_glfw_mouse_scroll_callback(GLFWwindow *window, Flt64 xoffset, Flt64 yoffset) {
    if (backend_glfw_callbacks->_on_mouse_scrolled)
        backend_glfw_callbacks->_on_mouse_scrolled(window, (Int8)yoffset);
}

WindowResult window_backend_glfw_startup(void) {
    if (backend_glfw_initialized)
        return WINDOW_ERROR_ALREADY_INITIALIZED;

    if (glfwInit() != GLFW_TRUE)
        return WINDOW_ERROR_INITILIZATION_FAILED;

    backend_glfw_initialized = true;
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_shutdown(void) {
    if (!backend_glfw_initialized)
        return WINDOW_ERROR_NOT_INITIALIZED;

    glfwTerminate();

    backend_glfw_initialized = false;
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_construct_handle(
    const WindowProperties props,
    const WindowCallbacks *callbacks,
    Window                *out_new_window) {
    if (!backend_glfw_initialized)
        return WINDOW_ERROR_NOT_INITIALIZED;

    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("Application", sizeof(struct Window_Handle), (VoidPtr *)out_new_window, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return WINDOW_ERROR_ALLOCATION_FAILED;
    memset((*out_new_window), 0, sizeof(struct Window_Handle));
    (*out_new_window)->_memory_size = alloc_size_;

    GLFWwindow *handle_ = glfwCreateWindow(props._width, props._height, container_string_get(props._title), props._fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!handle_)
        return WINDOW_ERROR_ALLOCATION_FAILED;
    (*out_new_window)->_handle = handle_;

    // setup callbacks
    {
        backend_glfw_callbacks = (WindowCallbacks *)callbacks;

        glfwSetWindowCloseCallback((*out_new_window)->_handle, (GLFWwindowclosefun)backend_glfw_callbacks->_on_close);
        glfwSetKeyCallback((*out_new_window)->_handle, (GLFWkeyfun)_backend_glfw_key_callback);
        glfwSetMouseButtonCallback((*out_new_window)->_handle, (GLFWmousebuttonfun)_backend_glfw_mouse_button_callback);
        glfwSetCursorPosCallback((*out_new_window)->_handle, (GLFWcursorposfun)_backend_glfw_mouse_move_callback);
        glfwSetScrollCallback((*out_new_window)->_handle, (GLFWscrollfun)_backend_glfw_mouse_scroll_callback);
    }

    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_destruct_handle(Window window) {
    if (!window || !window->_handle) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    if (window->_handle) glfwDestroyWindow(window->_handle);
    if (memory_zone_deallocate("Application", window, window->_memory_size) != MEMORY_ZONE_SUCCESS)
        return WINDOW_ERROR_ALLOCATION_FAILED;

    window = NULL;
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_title(Window window, ConstStr title) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowTitle(window->_handle, title);
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_x(Window window, const Int32 x) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowPos(window->_handle, x, window_backend_glfw_get_y(window));
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_y(Window window, const Int32 y) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowPos(window->_handle, window_backend_glfw_get_x(window), y);
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_position(Window window, const Int32 x, const Int32 y) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowPos(window->_handle, x, y);
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_width(Window window, const Int32 width) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowSize(window->_handle, width, window_backend_glfw_get_height(window));
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_height(Window window, const Int32 height) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowSize(window->_handle, window_backend_glfw_get_width(window), height);
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_set_size(Window window, const Int32 width, const Int32 height) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSetWindowSize(window->_handle, width, height);
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_poll_events(Window window) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwPollEvents();
    return WINDOW_SUCCESS;
}

WindowResult window_backend_glfw_swap_buffers(Window window) {
    if (!window) return WINDOW_ERROR_INVALID_PARAM;
    if (!backend_glfw_initialized) return WINDOW_ERROR_NOT_INITIALIZED;

    glfwSwapBuffers(window->_handle);
    return WINDOW_SUCCESS;
}

VoidPtr window_backend_glfw_get_handle(Window window) {
    return !window ? NULL : (VoidPtr)window->_handle;
}

ConstStr window_backend_glfw_get_title(Window window) {
    return !window ? "" : glfwGetWindowTitle(window->_handle);
}

Int32 window_backend_glfw_get_x(Window window) {
    if (!window)
        return 0;

    Int32 x_ = 0;
    glfwGetWindowPos(window->_handle, &x_, NULL);
    return x_;
}

Int32 window_backend_glfw_get_y(Window window) {
    if (!window)
        return 0;

    Int32 y_ = 0;
    glfwGetWindowPos(window->_handle, NULL, &y_);
    return y_;
}

Int32 window_backend_glfw_get_width(Window window) {
    if (!window)
        return 0;

    Int32 width_ = 0;
    glfwGetWindowSize(window->_handle, &width_, NULL);
    return width_;
}

Int32 window_backend_glfw_get_height(Window window) {
    if (!window)
        return 0;

    Int32 height_ = 0;
    glfwGetWindowSize(window->_handle, NULL, &height_);
    return height_;
}

Flt64 window_backend_glfw_get_frame(Window window) {
    if (!window)
        return 0;

    return glfwGetTime();
}
