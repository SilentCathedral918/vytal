#pragma once

// Vytal Engine
#include <vytal.h>

typedef enum Editor_Result {
    EDITOR_SUCCESS                   = 0,
    EDITOR_ERROR_INVALID_PARAM       = -1,
    EDITOR_ERROR_ALREADY_INITIALIZED = -2,
    EDITOR_ERROR_NOT_INITIALIZED     = -3,
    EDITOR_ERROR_FILE_OPEN_FAILED    = -4,
    EDITOR_ERROR_FILE_CLOSE_FAILED   = -5,
    EDITOR_ERROR_ALLOCATION_FAILED   = -6,
    EDITOR_ERROR_DEALLOCATION_FAILED = -7,
    EDITOR_ERROR_PARSE_FAILED        = -8,

    EDITOR_ERROR_ENGINE_PRECONSTRUCT_FAILED = -9,
    EDITOR_ERROR_ENGINE_CONSTRUCT_FAILED    = -10,
    EDITOR_ERROR_ENGINE_UPDATE_FAILED       = -11,
    EDITOR_ERROR_ENGINE_DESTRUCT_FAILED     = -12,

    EDITOR_ERROR_WINDOW_OPEN_FAILED      = -13,
    EDITOR_ERROR_WINDOW_CLOSE_FAILED     = -14,
    EDITOR_ERROR_WINDOW_OPERATION_FAILED = -15,
} EditorResult;

EditorResult editor_startup(ConstStr project_filepath);
EditorResult editor_update(void);
EditorResult editor_shutdown(void);

EditorResult editor_open_window(ConstStr id, Window *out_window);
EditorResult editor_close_window(Window window);
