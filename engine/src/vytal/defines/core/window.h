#pragma once

#include "containers.h"
#include "input.h"
#include "memory.h"

// types ---------------------------------------------------------------- //

typedef struct Window_Handle *Window;

// flags ---------------------------------------------------------------- //

typedef enum Window_Title_Bar_Flag {
    WINDOW_TITLE_BAR_FLAG_NONE       = 0,
    WINDOW_TITLE_BAR_FLAG_TITLE      = VYTAL_BITFLAG_FIELD(0),
    WINDOW_TITLE_BAR_FLAG_FRAME_TIME = VYTAL_BITFLAG_FIELD(1),
    WINDOW_TITLE_BAR_FLAG_DRAW_TIME  = VYTAL_BITFLAG_FIELD(2)
} WindowTitleBarFlag;

// backends ------------------------------------------------------------- //

typedef enum Window_Backend {
    WINDOW_BACKEND_GLFW
} WindowBackend;

// window modes --------------------------------------------------------- //

typedef enum Window_Mode {
    WINDOW_MODE_WINDOWED,
    WINDOW_MODE_FULLSCREEN,
    WINDOW_MODE_BORDERLESS
} WindowMode;

// callbacks ------------------------------------------------------------ //

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

// properties ----------------------------------------------------------- //

typedef struct Window_Properties {
    String             _title;
    Int32              _x, _y;
    Int32              _width, _height;
    WindowTitleBarFlag _title_bar_flags;
    WindowMode         _mode;
    WindowBackend      _backend;
    Bool               _enable_vsync;
    WindowCallbacks    _callbacks;
} WindowProperties;

// return codes --------------------------------------------------------- //

typedef enum Window_Result {
    WINDOW_SUCCESS                    = 0,
    WINDOW_ERROR_NOT_INITIALIZED      = -1,
    WINDOW_ERROR_ALREADY_INITIALIZED  = -2,
    WINDOW_ERROR_INITILIZATION_FAILED = -3,
    WINDOW_ERROR_INVALID_PARAM        = -4,
    WINDOW_ERROR_INVALID_BACKEND      = -5,
    WINDOW_ERROR_ALLOCATION_FAILED    = -6,
    WINDOW_ERROR_DEALLOCATION_FAILED  = -7
} WindowResult;

typedef enum Window_Module_Result {
    WINDOW_MODULE_SUCCESS                    = 0,
    WINDOW_MODULE_ERROR_NOT_INITIALIZED      = -1,
    WINDOW_MODULE_ERROR_ALREADY_INITIALIZED  = -2,
    WINDOW_MODULE_ERROR_INITILIZATION_FAILED = -3,
    WINDOW_MODULE_ERROR_INVALID_PARAM        = -4,
    WINDOW_MODULE_ERROR_INVALID_BACKEND      = -5,
    WINDOW_MODULE_ERROR_ALLOCATION_FAILED    = -6,
    WINDOW_MODULE_ERROR_DEALLOCATION_FAILED  = -7,
    WINDOW_MODULE_ERROR_FILE_OPEN_FAILED     = -8,
    WINDOW_MODULE_ERROR_FILE_CLOSE_FAILED    = -9,
    WINDOW_MODULE_ERROR_INVALID_CONFIG       = -10,
    WINDOW_MODULE_ERROR_PARSE_FAILED         = -11
} WindowModuleResult;
