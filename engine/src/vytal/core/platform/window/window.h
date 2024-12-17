#pragma once

#include "vytal/defines/core/hash.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

Bool           platform_window_startup(void);
void           platform_window_shutdown(void);
PlatformWindow platform_window_construct(const WindowProps properties);
Bool           platform_window_destruct(PlatformWindow window);
VoidPtr        platform_window_get(PlatformWindow window);
ConstStr       platform_window_get_title(PlatformWindow window);
Int32          platform_window_get_x(PlatformWindow window);
Int32          platform_window_get_y(PlatformWindow window);
Int32          platform_window_get_width(PlatformWindow window);
Int32          platform_window_get_height(PlatformWindow window);
Bool           platform_window_set_title(PlatformWindow window, ConstStr title);
Bool           platform_window_set_x(PlatformWindow window, const Int32 x);
Bool           platform_window_set_y(PlatformWindow window, const Int32 y);
Bool           platform_window_set_position(PlatformWindow window, const Int32 x, const Int32 y);
Bool           platform_window_set_width(PlatformWindow window, const Int32 width);
Bool           platform_window_set_height(PlatformWindow window, const Int32 height);
Bool           platform_window_set_size(PlatformWindow window, const Int32 width, const Int32 height);