#pragma once

#include "types.h"

// input codes ---------------------------------------------------------- //

typedef enum Input_Key_Code {
    VYTAL_KEYCODE_SPACE         = 32,
    VYTAL_KEYCODE_APOSTROPHE    = 39,
    VYTAL_KEYCODE_COMMA         = 44,
    VYTAL_KEYCODE_MINUS         = 45,
    VYTAL_KEYCODE_PERIOD        = 46,
    VYTAL_KEYCODE_SLASH         = 47,
    VYTAL_KEYCODE_0             = 48,
    VYTAL_KEYCODE_1             = 49,
    VYTAL_KEYCODE_2             = 50,
    VYTAL_KEYCODE_3             = 51,
    VYTAL_KEYCODE_4             = 52,
    VYTAL_KEYCODE_5             = 53,
    VYTAL_KEYCODE_6             = 54,
    VYTAL_KEYCODE_7             = 55,
    VYTAL_KEYCODE_8             = 56,
    VYTAL_KEYCODE_9             = 57,
    VYTAL_KEYCODE_SEMICOLON     = 59,
    VYTAL_KEYCODE_EQUAL         = 61,
    VYTAL_KEYCODE_A             = 65,
    VYTAL_KEYCODE_B             = 66,
    VYTAL_KEYCODE_C             = 67,
    VYTAL_KEYCODE_D             = 68,
    VYTAL_KEYCODE_E             = 69,
    VYTAL_KEYCODE_F             = 70,
    VYTAL_KEYCODE_G             = 71,
    VYTAL_KEYCODE_H             = 72,
    VYTAL_KEYCODE_I             = 73,
    VYTAL_KEYCODE_J             = 74,
    VYTAL_KEYCODE_K             = 75,
    VYTAL_KEYCODE_L             = 76,
    VYTAL_KEYCODE_M             = 77,
    VYTAL_KEYCODE_N             = 78,
    VYTAL_KEYCODE_O             = 79,
    VYTAL_KEYCODE_P             = 80,
    VYTAL_KEYCODE_Q             = 81,
    VYTAL_KEYCODE_R             = 82,
    VYTAL_KEYCODE_S             = 83,
    VYTAL_KEYCODE_T             = 84,
    VYTAL_KEYCODE_U             = 85,
    VYTAL_KEYCODE_V             = 86,
    VYTAL_KEYCODE_W             = 87,
    VYTAL_KEYCODE_X             = 88,
    VYTAL_KEYCODE_Y             = 89,
    VYTAL_KEYCODE_Z             = 90,
    VYTAL_KEYCODE_LEFT_BRACKET  = 91,
    VYTAL_KEYCODE_BACKSLASH     = 92,
    VYTAL_KEYCODE_RIGHT_BRACKET = 93,
    VYTAL_KEYCODE_GRAVE_ACCENT  = 96,
    VYTAL_KEYCODE_WORLD_1       = 161,
    VYTAL_KEYCODE_WORLD_2       = 162,
    VYTAL_KEYCODE_ESCAPE        = 256,
    VYTAL_KEYCODE_ENTER         = 257,
    VYTAL_KEYCODE_TAB           = 258,
    VYTAL_KEYCODE_BACKSPACE     = 259,
    VYTAL_KEYCODE_INSERT        = 260,
    VYTAL_KEYCODE_DELETE        = 261,
    VYTAL_KEYCODE_RIGHT         = 262,
    VYTAL_KEYCODE_LEFT          = 263,
    VYTAL_KEYCODE_DOWN          = 264,
    VYTAL_KEYCODE_UP            = 265,
    VYTAL_KEYCODE_PAGE_UP       = 266,
    VYTAL_KEYCODE_PAGE_DOWN     = 267,
    VYTAL_KEYCODE_HOME          = 268,
    VYTAL_KEYCODE_END           = 269,
    VYTAL_KEYCODE_CAPS_LOCK     = 280,
    VYTAL_KEYCODE_SCROLL_LOCK   = 281,
    VYTAL_KEYCODE_NUM_LOCK      = 282,
    VYTAL_KEYCODE_PRINT_SCREEN  = 283,
    VYTAL_KEYCODE_PAUSE         = 284,
    VYTAL_KEYCODE_F1            = 290,
    VYTAL_KEYCODE_F2            = 291,
    VYTAL_KEYCODE_F3            = 292,
    VYTAL_KEYCODE_F4            = 293,
    VYTAL_KEYCODE_F5            = 294,
    VYTAL_KEYCODE_F6            = 295,
    VYTAL_KEYCODE_F7            = 296,
    VYTAL_KEYCODE_F8            = 297,
    VYTAL_KEYCODE_F9            = 298,
    VYTAL_KEYCODE_F10           = 299,
    VYTAL_KEYCODE_F11           = 300,
    VYTAL_KEYCODE_F12           = 301,
    VYTAL_KEYCODE_F13           = 302,
    VYTAL_KEYCODE_F14           = 303,
    VYTAL_KEYCODE_F15           = 304,
    VYTAL_KEYCODE_F16           = 305,
    VYTAL_KEYCODE_F17           = 306,
    VYTAL_KEYCODE_F18           = 307,
    VYTAL_KEYCODE_F19           = 308,
    VYTAL_KEYCODE_F20           = 309,
    VYTAL_KEYCODE_F21           = 310,
    VYTAL_KEYCODE_F22           = 311,
    VYTAL_KEYCODE_F23           = 312,
    VYTAL_KEYCODE_F24           = 313,
    VYTAL_KEYCODE_F25           = 314,
    VYTAL_KEYCODE_KP_0          = 320,
    VYTAL_KEYCODE_KP_1          = 321,
    VYTAL_KEYCODE_KP_2          = 322,
    VYTAL_KEYCODE_KP_3          = 323,
    VYTAL_KEYCODE_KP_4          = 324,
    VYTAL_KEYCODE_KP_5          = 325,
    VYTAL_KEYCODE_KP_6          = 326,
    VYTAL_KEYCODE_KP_7          = 327,
    VYTAL_KEYCODE_KP_8          = 328,
    VYTAL_KEYCODE_KP_9          = 329,
    VYTAL_KEYCODE_KP_DECIMAL    = 330,
    VYTAL_KEYCODE_KP_DIVIDE     = 331,
    VYTAL_KEYCODE_KP_MULTIPLY   = 332,
    VYTAL_KEYCODE_KP_SUBTRACT   = 333,
    VYTAL_KEYCODE_KP_ADD        = 334,
    VYTAL_KEYCODE_KP_ENTER      = 335,
    VYTAL_KEYCODE_KP_EQUAL      = 336,
    VYTAL_KEYCODE_LEFT_SHIFT    = 340,
    VYTAL_KEYCODE_LEFT_CONTROL  = 341,
    VYTAL_KEYCODE_LEFT_ALT      = 342,
    VYTAL_KEYCODE_LEFT_SUPER    = 343,
    VYTAL_KEYCODE_RIGHT_SHIFT   = 344,
    VYTAL_KEYCODE_RIGHT_CONTROL = 345,
    VYTAL_KEYCODE_RIGHT_ALT     = 346,
    VYTAL_KEYCODE_RIGHT_SUPER   = 347,
    VYTAL_KEYCODE_MENU          = 348,
    VYTAL_KEYCODE_LAST          = VYTAL_KEYCODE_MENU,

    VYTAL_KEYCODES_TOTAL = 512
} InputKeyCode;

typedef enum Input_Mouse_Code {
    VYTAL_MOUSECODE_1 = 0,
    VYTAL_MOUSECODE_3 = 2,
    VYTAL_MOUSECODE_2 = 1,
    VYTAL_MOUSECODE_4 = 3,
    VYTAL_MOUSECODE_5 = 4,
    VYTAL_MOUSECODE_6 = 5,
    VYTAL_MOUSECODE_7 = 6,
    VYTAL_MOUSECODE_8 = 7,

    VYTAL_MOUSECODES_TOTAL,

    VYTAL_MOUSECODE_LAST   = VYTAL_MOUSECODE_8,
    VYTAL_MOUSECODE_LEFT   = VYTAL_MOUSECODE_1,
    VYTAL_MOUSECODE_RIGHT  = VYTAL_MOUSECODE_2,
    VYTAL_MOUSECODE_MIDDLE = VYTAL_MOUSECODE_3,

} InputMouseCode;

typedef enum Input_Event_Code {
    // application events
    VYTAL_EVENTCODE_WINDOW_CLOSE = 0x01,

    // key events
    VYTAL_EVENTCODE_KEY_PRESSED  = 0x02,
    VYTAL_EVENTCODE_KEY_RELEASED = 0x03,

    // mouse events
    VYTAL_EVENTCODE_MOUSE_PRESSED  = 0x04,
    VYTAL_EVENTCODE_MOUSE_RELEASED = 0x05,
    VYTAL_EVENTCODE_MOUSE_MOVED    = 0x06,
    VYTAL_EVENTCODE_MOUSE_SCROLLED = 0x07,

    // window events
    VYTAL_EVENTCODE_RESIZED = 0x08,

    // test events
    VYTAL_EVENTCODE_TESTUNIT_00 = 0x09,
    VYTAL_EVENTCODE_TESTUNIT_01 = 0x0A,
    VYTAL_EVENTCODE_TESTUNIT_02 = 0x0B,
    VYTAL_EVENTCODE_TESTUNIT_03 = 0x0C,
    VYTAL_EVENTCODE_TESTUNIT_04 = 0x0D,

    VYTAL_EVENTCODES_TOTAL
} InputEventCode;

// input data ----------------------------------------------------------- //

typedef struct Input_Event_Data {
    InputEventCode _event_code;
} InputEventData;

typedef struct Input_Key_Event_Data {
    InputEventCode _event_code;
    Int32          _key_code;
} InputKeyEventData;

typedef struct Input_Mouse_Event_Data {
    InputEventCode _event_code;
    Int32          _mouse_code;
} InputMouseEventData;

typedef struct Input_Mouse_Move_Event_Data {
    InputEventCode _event_code;
    Int32          _x, _y;
} InputMouseMoveEventData;

typedef struct Input_Mouse_Scroll_Event_Data {
    InputEventCode _event_code;
    Int8           _scroll_value;
} InputMouseScrollEventData;

typedef struct Input_Window_Resize_Event_Data {
    InputEventCode _event_code;
    Int32          _width, _height;
} InputWindowResizeEventData;

// module return codes -------------------------------------------------- //

typedef enum Input_Module_Result {
    INPUT_MODULE_SUCCESS                   = 0,
    INPUT_MODULE_ERROR_ALREADY_INITIALIZED = -1,
    INPUT_MODULE_ERROR_NOT_INITIALIZED     = -2,
    INPUT_MODULE_ERROR_ALLOCATION_FAILED   = -3,
    INPUT_MODULE_ERROR_DEALLOCATION_FAILED = -4,
    INPUT_MODULE_ERROR_EVENT_BIND_FAILED   = -5,
    INPUT_MODULE_ERROR_EVENT_UNBIND_FAILED = -6,
    INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED = -7,
    INPUT_MODULE_ERROR_INVALID_PARAM       = -8,
    INPUT_MODULE_ERROR_PARSE_FAILED        = -9,
    INPUT_MODULE_ERROR_DATA_INSERT_FAILED  = -10,
} InputModuleResult;
