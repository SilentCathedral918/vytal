#pragma once

#include "types.h"

// ---------------------- configuration ---------------------- //

typedef struct Window_Properties {
    Char  _title[64];
    Int32 _x, _y;
    Int32 _width, _height;
    Bool  _resizable;
    Bool  _fullscreen;
} WindowProps;

// ------------------------- type ------------------------- //

typedef struct Window_Platform_Struct *PlatformWindow;
