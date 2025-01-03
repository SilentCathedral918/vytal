#pragma once

#include "input.h"
#include "mem.h"
#include "types.h"

// ------------------------- type ------------------------- //

typedef struct Window_Platform_Struct *PlatformWindow;

typedef struct Window_Callbacks {
    // window events
    void (*_on_close)(VoidPtr context);

    // keyboard events
    void (*_on_key_pressed)(VoidPtr context, InputKeyCode code, Bool pressed);

    // mouse events
    void (*_on_mouse_pressed)(VoidPtr context, InputMouseCode code, Bool pressed);
    void (*_on_mouse_moved)(VoidPtr context, Int32 x, Int32 y);
    void (*_on_mouse_scrolled)(VoidPtr context, Int8 scroll_value);
} WindowCallbacks;

typedef enum Window_Titlebar_Flag {
    WINDOW_TITLEBAR_FLAG_NONE = VT_BITFLAG_FIELD(0),
    WINDOW_TITLEBAR_FLAG_FPS  = VT_BITFLAG_FIELD(1)
} WindowTitlebarFlag;

// ------------------------- backends ------------------------- //

typedef enum Window_Backend { WINDOW_BACKEND_GLFW } WindowBackend;

// ---------------------- properties ---------------------- //

typedef struct Window_Properties {
    Char                _title[64];
    Int32               _x, _y;
    Int32               _width, _height;
    UInt64              _titlebar_flags;
    WindowCallbacks     _callbacks;
    const Bool          _resizable;
    const Bool          _fullscreen;
    const WindowBackend _backend;
} WindowProps;
