#pragma once

#include "vytal/defines/core/types.h"

// ----------------------------- event enums ----------------------------- //

typedef enum Input_Key_Code {
    VT_KEYCODE_SPACE         = 32,
    VT_KEYCODE_APOSTROPHE    = 39,
    VT_KEYCODE_COMMA         = 44,
    VT_KEYCODE_MINUS         = 45,
    VT_KEYCODE_PERIOD        = 46,
    VT_KEYCODE_SLASH         = 47,
    VT_KEYCODE_0             = 48,
    VT_KEYCODE_1             = 49,
    VT_KEYCODE_2             = 50,
    VT_KEYCODE_3             = 51,
    VT_KEYCODE_4             = 52,
    VT_KEYCODE_5             = 53,
    VT_KEYCODE_6             = 54,
    VT_KEYCODE_7             = 55,
    VT_KEYCODE_8             = 56,
    VT_KEYCODE_9             = 57,
    VT_KEYCODE_SEMICOLON     = 59,
    VT_KEYCODE_EQUAL         = 61,
    VT_KEYCODE_A             = 65,
    VT_KEYCODE_B             = 66,
    VT_KEYCODE_C             = 67,
    VT_KEYCODE_D             = 68,
    VT_KEYCODE_E             = 69,
    VT_KEYCODE_F             = 70,
    VT_KEYCODE_G             = 71,
    VT_KEYCODE_H             = 72,
    VT_KEYCODE_I             = 73,
    VT_KEYCODE_J             = 74,
    VT_KEYCODE_K             = 75,
    VT_KEYCODE_L             = 76,
    VT_KEYCODE_M             = 77,
    VT_KEYCODE_N             = 78,
    VT_KEYCODE_O             = 79,
    VT_KEYCODE_P             = 80,
    VT_KEYCODE_Q             = 81,
    VT_KEYCODE_R             = 82,
    VT_KEYCODE_S             = 83,
    VT_KEYCODE_T             = 84,
    VT_KEYCODE_U             = 85,
    VT_KEYCODE_V             = 86,
    VT_KEYCODE_W             = 87,
    VT_KEYCODE_X             = 88,
    VT_KEYCODE_Y             = 89,
    VT_KEYCODE_Z             = 90,
    VT_KEYCODE_LEFT_BRACKET  = 91,
    VT_KEYCODE_BACKSLASH     = 92,
    VT_KEYCODE_RIGHT_BRACKET = 93,
    VT_KEYCODE_GRAVE_ACCENT  = 96,
    VT_KEYCODE_WORLD_1       = 161,
    VT_KEYCODE_WORLD_2       = 162,
    VT_KEYCODE_ESCAPE        = 256,
    VT_KEYCODE_ENTER         = 257,
    VT_KEYCODE_TAB           = 258,
    VT_KEYCODE_BACKSPACE     = 259,
    VT_KEYCODE_INSERT        = 260,
    VT_KEYCODE_DELETE        = 261,
    VT_KEYCODE_RIGHT         = 262,
    VT_KEYCODE_LEFT          = 263,
    VT_KEYCODE_DOWN          = 264,
    VT_KEYCODE_UP            = 265,
    VT_KEYCODE_PAGE_UP       = 266,
    VT_KEYCODE_PAGE_DOWN     = 267,
    VT_KEYCODE_HOME          = 268,
    VT_KEYCODE_END           = 269,
    VT_KEYCODE_CAPS_LOCK     = 280,
    VT_KEYCODE_SCROLL_LOCK   = 281,
    VT_KEYCODE_NUM_LOCK      = 282,
    VT_KEYCODE_PRINT_SCREEN  = 283,
    VT_KEYCODE_PAUSE         = 284,
    VT_KEYCODE_F1            = 290,
    VT_KEYCODE_F2            = 291,
    VT_KEYCODE_F3            = 292,
    VT_KEYCODE_F4            = 293,
    VT_KEYCODE_F5            = 294,
    VT_KEYCODE_F6            = 295,
    VT_KEYCODE_F7            = 296,
    VT_KEYCODE_F8            = 297,
    VT_KEYCODE_F9            = 298,
    VT_KEYCODE_F10           = 299,
    VT_KEYCODE_F11           = 300,
    VT_KEYCODE_F12           = 301,
    VT_KEYCODE_F13           = 302,
    VT_KEYCODE_F14           = 303,
    VT_KEYCODE_F15           = 304,
    VT_KEYCODE_F16           = 305,
    VT_KEYCODE_F17           = 306,
    VT_KEYCODE_F18           = 307,
    VT_KEYCODE_F19           = 308,
    VT_KEYCODE_F20           = 309,
    VT_KEYCODE_F21           = 310,
    VT_KEYCODE_F22           = 311,
    VT_KEYCODE_F23           = 312,
    VT_KEYCODE_F24           = 313,
    VT_KEYCODE_F25           = 314,
    VT_KEYCODE_KP_0          = 320,
    VT_KEYCODE_KP_1          = 321,
    VT_KEYCODE_KP_2          = 322,
    VT_KEYCODE_KP_3          = 323,
    VT_KEYCODE_KP_4          = 324,
    VT_KEYCODE_KP_5          = 325,
    VT_KEYCODE_KP_6          = 326,
    VT_KEYCODE_KP_7          = 327,
    VT_KEYCODE_KP_8          = 328,
    VT_KEYCODE_KP_9          = 329,
    VT_KEYCODE_KP_DECIMAL    = 330,
    VT_KEYCODE_KP_DIVIDE     = 331,
    VT_KEYCODE_KP_MULTIPLY   = 332,
    VT_KEYCODE_KP_SUBTRACT   = 333,
    VT_KEYCODE_KP_ADD        = 334,
    VT_KEYCODE_KP_ENTER      = 335,
    VT_KEYCODE_KP_EQUAL      = 336,
    VT_KEYCODE_LEFT_SHIFT    = 340,
    VT_KEYCODE_LEFT_CONTROL  = 341,
    VT_KEYCODE_LEFT_ALT      = 342,
    VT_KEYCODE_LEFT_SUPER    = 343,
    VT_KEYCODE_RIGHT_SHIFT   = 344,
    VT_KEYCODE_RIGHT_CONTROL = 345,
    VT_KEYCODE_RIGHT_ALT     = 346,
    VT_KEYCODE_RIGHT_SUPER   = 347,
    VT_KEYCODE_MENU          = 348,
    VT_KEYCODE_LAST          = VT_KEYCODE_MENU,
} InputKeyCode;

typedef enum Input_Mouse_Code {
    VT_MOUSECODE_1      = 0,
    VT_MOUSECODE_3      = 2,
    VT_MOUSECODE_2      = 1,
    VT_MOUSECODE_4      = 3,
    VT_MOUSECODE_5      = 4,
    VT_MOUSECODE_6      = 5,
    VT_MOUSECODE_7      = 6,
    VT_MOUSECODE_8      = 7,
    VT_MOUSECODE_LAST   = VT_MOUSECODE_8,
    VT_MOUSECODE_LEFT   = VT_MOUSECODE_1,
    VT_MOUSECODE_RIGHT  = VT_MOUSECODE_2,
    VT_MOUSECODE_MIDDLE = VT_MOUSECODE_3,
} InputMouseCode;

typedef enum Input_Event_Code {
    // application events
    VT_EVENTCODE_WINDOW_CLOSE = 0x01,

    // key events
    VT_EVENTCODE_KEY_PRESSED  = 0x02,
    VT_EVENTCODE_KEY_RELEASED = 0x03,

    // mouse events
    VT_EVENTCODE_MOUSE_PRESSED  = 0x04,
    VT_EVENTCODE_MOUSE_RELEASED = 0x05,
    VT_EVENTCODE_MOUSE_MOVED    = 0x06,
    VT_EVENTCODE_MOUSE_SCROLLED = 0x07,

    // window events
    VT_EVENTCODE_RESIZED = 0x08,

    // test events
    VT_EVENTCODE_TESTUNIT_00 = 0x09,
    VT_EVENTCODE_TESTUNIT_01 = 0x0A,
    VT_EVENTCODE_TESTUNIT_02 = 0x0B,
    VT_EVENTCODE_TESTUNIT_03 = 0x0C,
    VT_EVENTCODE_TESTUNIT_04 = 0x0D,

    VT_EVENTCODES_TOTAL
} InputEventCode;

// ----------------------------- event datas ----------------------------- //

typedef struct Input_Event_Data {
    InputEventCode _event_code;
} InputEventData;

typedef struct Input_Key_Event_Data {
    InputEventCode _event_code;
    UInt16         _key_code;
} InputKeyEventData;

typedef struct Input_Mouse_Event_Data {
    InputEventCode _event_code;
    UInt16         _mouse_code;
} InputMouseEventData;

typedef struct Input_Mouse_Move_Event_Data {
    InputEventCode _event_code;
    UInt16         _x, _y;
} InputMouseMoveEventData;

typedef struct Input_Mouse_Scroll_Event_Data {
    InputEventCode _event_code;
    Int8           _scroll_value;
} InputMouseScrollEventData;

typedef struct Input_Window_Resize_Event_Data {
    InputEventCode _event_code;
    UInt16         _width, _height;
} InputWindowResizeEventData;
