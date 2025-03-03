#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VYTAL_API WindowResult window_backend_glfw_startup(void);
VYTAL_API WindowResult window_backend_glfw_shutdown(void);
VYTAL_API WindowResult window_backend_glfw_construct_handle(
    const WindowProperties props,
    const WindowCallbacks *callbacks,
    Window                *out_new_window);
VYTAL_API WindowResult window_backend_glfw_destruct_handle(Window window);
VYTAL_API WindowResult window_backend_glfw_set_title(Window window, ConstStr title);
VYTAL_API WindowResult window_backend_glfw_set_x(Window window, const Int32 x);
VYTAL_API WindowResult window_backend_glfw_set_y(Window window, const Int32 y);
VYTAL_API WindowResult window_backend_glfw_set_position(Window window, const Int32 x, const Int32 y);
VYTAL_API WindowResult window_backend_glfw_set_width(Window window, const Int32 width);
VYTAL_API WindowResult window_backend_glfw_set_height(Window window, const Int32 height);
VYTAL_API WindowResult window_backend_glfw_set_size(Window window, const Int32 width, const Int32 height);
VYTAL_API WindowResult window_backend_glfw_poll_events(Window window);
VYTAL_API WindowResult window_backend_glfw_swap_buffers(Window window);

VYTAL_API VoidPtr  window_backend_glfw_get_handle(Window window);
VYTAL_API ConstStr window_backend_glfw_get_title(Window window);
VYTAL_API Int32    window_backend_glfw_get_x(Window window);
VYTAL_API Int32    window_backend_glfw_get_y(Window window);
VYTAL_API Int32    window_backend_glfw_get_width(Window window);
VYTAL_API Int32    window_backend_glfw_get_height(Window window);
VYTAL_API Flt64    window_backend_glfw_get_frame(Window window);