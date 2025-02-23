#pragma once

#include "containers.h"
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

// properties ----------------------------------------------------------- //

typedef struct Window_Properties {
    String             _title;
    Int32              _x, _y;
    Int32              _width, _height;
    WindowTitleBarFlag _title_bar_flags;
    Bool               _fullscreen;

    const WindowBackend _backend;
    const Bool          _resizable;
} WindowProperties;

// return code----------------------------------------------------------- //

typedef enum Window_Result {
    WINDOW_SUCCESS                    = 0,
    WINDOW_ERROR_NOT_INITIALIZED      = -1,
    WINDOW_ERROR_ALREADY_INITIALIZED  = -2,
    WINDOW_ERROR_INITILIZATION_FAILED = -3,
    WINDOW_ERROR_INVALID_PARAM        = -4,
    WINDOW_ERROR_INVALID_BACKEND      = -5
} WindowResult;
