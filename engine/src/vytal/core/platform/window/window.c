#include "window.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include <GLFW/glfw3.h>

typedef struct Window_Platform_Struct {
    GLFWwindow *_handle;
} Window_Platform_Struct;

Bool platform_window_startup(void) { return (glfwInit() == GLFW_TRUE); }

void platform_window_shutdown(void) { glfwTerminate(); }

PlatformWindow platform_window_construct(const WindowProps properties) {
    GLFWwindow *handle_ = glfwCreateWindow(properties._width, properties._height, properties._title,
                                           properties._fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!handle_)
        return NULL;

    PlatformWindow window_ = memory_manager_allocate(sizeof(Window_Platform_Struct), MEMORY_TAG_PLATFORM);
    window_->_handle       = handle_;

    return window_;
}

Bool platform_window_destruct(PlatformWindow window) {
    if (!window)
        return false;

    glfwDestroyWindow(window->_handle);
    window->_handle = NULL;

    // no need to free the platform window
    // let the memory manager handle the work
    return true;
}

VoidPtr platform_window_get(PlatformWindow window) { return !window ? NULL : VT_CAST(VoidPtr, window->_handle); }

ConstStr platform_window_get_title(PlatformWindow window) { return !window ? "" : glfwGetWindowTitle(window->_handle); }

Int32 platform_window_get_x(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 x_ = 0;
    glfwGetWindowPos(window->_handle, &x_, NULL);
    return x_;
}

Int32 platform_window_get_y(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 y_ = 0;
    glfwGetWindowPos(window->_handle, NULL, &y_);
    return y_;
}

Int32 platform_window_get_width(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 width_ = 0;
    glfwGetWindowSize(window->_handle, &width_, NULL);
    return width_;
}

Int32 platform_window_get_height(PlatformWindow window) {
    if (!window)
        return 0;

    Int32 height_ = 0;
    glfwGetWindowSize(window->_handle, NULL, &height_);
    return height_;
}

Bool platform_window_set_title(PlatformWindow window, ConstStr title) {
    if (!window)
        return false;

    glfwSetWindowTitle(window->_handle, title);
    return true;
}

Bool platform_window_set_x(PlatformWindow window, const Int32 x) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, x, platform_window_get_y(window));
    return true;
}

Bool platform_window_set_y(PlatformWindow window, const Int32 y) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, platform_window_get_x(window), y);
    return true;
}

Bool platform_window_set_position(PlatformWindow window, const Int32 x, const Int32 y) {
    if (!window)
        return false;

    glfwSetWindowPos(window->_handle, x, y);
    return true;
}

Bool platform_window_set_width(PlatformWindow window, const Int32 width) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, width, platform_window_get_height(window));
    return true;
}

Bool platform_window_set_height(PlatformWindow window, const Int32 height) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, platform_window_get_width(window), height);
    return true;
}

Bool platform_window_set_size(PlatformWindow window, const Int32 width, const Int32 height) {
    if (!window)
        return false;

    glfwSetWindowSize(window->_handle, width, height);
    return true;
}
