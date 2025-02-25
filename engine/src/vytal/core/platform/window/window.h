#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VYTAL_API WindowResult platform_window_startup(const WindowBackend backend);
VYTAL_API WindowResult platform_window_shutdown(void);
VYTAL_API WindowResult platform_window_construct_window(
    const WindowProperties properties,
    const WindowCallbacks *callbacks,
    Window                *out_new_window);
VYTAL_API WindowResult platform_window_destruct_window(Window window);
VYTAL_API WindowResult platform_window_set_title(Window window, ConstStr title);
VYTAL_API WindowResult platform_window_set_x(Window window, const Int32 x);
VYTAL_API WindowResult platform_window_set_y(Window window, const Int32 y);
VYTAL_API WindowResult platform_window_set_position(Window window, const Int32 x, const Int32 y);
VYTAL_API WindowResult platform_window_set_width(Window window, const Int32 width);
VYTAL_API WindowResult platform_window_set_height(Window window, const Int32 height);
VYTAL_API WindowResult platform_window_set_size(Window window, const Int32 width, const Int32 height);
VYTAL_API WindowResult platform_window_poll_events(Window window);
VYTAL_API WindowResult platform_window_swap_buffers(Window window);

VYTAL_API VoidPtr  platform_window_get_handle(Window window);
VYTAL_API ConstStr platform_window_get_title(Window window);
VYTAL_API Int32    platform_window_get_x(Window window);
VYTAL_API Int32    platform_window_get_y(Window window);
VYTAL_API Int32    platform_window_get_width(Window window);
VYTAL_API Int32    platform_window_get_height(Window window);
VYTAL_API Flt64    platform_window_get_frame(Window window);
