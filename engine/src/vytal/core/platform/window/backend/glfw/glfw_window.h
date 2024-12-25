#pragma once

#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VT_API Bool           glfw_window_startup(void);
VT_API void           glfw_window_shutdown(void);
VT_API PlatformWindow glfw_window_construct(const WindowProps props, const WindowCallbacks *callbacks,
                                            const UInt64 titlebar_flags);
VT_API Bool           glfw_window_destruct(PlatformWindow window);
VT_API VoidPtr        glfw_window_get_handle(PlatformWindow window);
VT_API ConstStr       glfw_window_get_title(PlatformWindow window);
VT_API Int32          glfw_window_get_x(PlatformWindow window);
VT_API Int32          glfw_window_get_y(PlatformWindow window);
VT_API Int32          glfw_window_get_width(PlatformWindow window);
VT_API Int32          glfw_window_get_height(PlatformWindow window);
VT_API Flt64          glfw_window_get_frame(PlatformWindow window);
VT_API Bool           glfw_window_set_title(PlatformWindow window, ConstStr title);
VT_API Bool           glfw_window_set_x(PlatformWindow window, const Int32 x);
VT_API Bool           glfw_window_set_y(PlatformWindow window, const Int32 y);
VT_API Bool           glfw_window_set_position(PlatformWindow window, const Int32 x, const Int32 y);
VT_API Bool           glfw_window_set_width(PlatformWindow window, const Int32 width);
VT_API Bool           glfw_window_set_height(PlatformWindow window, const Int32 height);
VT_API Bool           glfw_window_set_size(PlatformWindow window, const Int32 width, const Int32 height);
VT_API Bool           glfw_window_poll_events(PlatformWindow window);
VT_API Bool           glfw_window_swap_buffers(PlatformWindow window);