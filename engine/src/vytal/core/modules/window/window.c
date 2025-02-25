#include "window.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/string/string.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/core/platform/window/window.h"

typedef struct Window_Module_State {
    Window           _main_window;
    WindowProperties _main_window_props;
    Bool             _initialized;
    ByteSize         _memory_size;
} WindowModuleState;

static WindowModuleState *state                     = NULL;
static ConstStr           main_window_cvar_filepath = "main_window.cfg";

VYTAL_INLINE Str _window_module_trim_whitespace(Str str) {
    // trim leading space
    {
        while (isspace((Int8)*str)) ++str;
        if (*str == '\0') return str;
    }

    // trim trailing space
    {
        Str end_ = str + strlen(str) - 1;
        while (end_ > str && isspace((unsigned char)*end_)) --end_;
        *(end_ + 1) = '\0';
    }

    return str;
}

// callbacks ------------------------------------------------------------ //

void _window_module_callback_on_window_close(VoidPtr context) {
    InputEventData data = {._event_code = VYTAL_EVENTCODE_WINDOW_CLOSE};
    input_module_invoke_event(VYTAL_EVENTCODE_WINDOW_CLOSE, NULL, &data);
}

void _window_module_callback_on_key_pressed(VoidPtr context, InputKeyCode key_code, Bool pressed) {
    InputKeyEventData data = {._event_code = pressed ? VYTAL_EVENTCODE_KEY_PRESSED : VYTAL_EVENTCODE_KEY_RELEASED, ._key_code = key_code};
    input_module_invoke_event(data._event_code, NULL, &data);
}

void _window_module_callback_on_mouse_pressed(VoidPtr context, InputMouseCode mouse_code, Bool pressed) {
    InputMouseEventData data = {._event_code = pressed ? VYTAL_EVENTCODE_MOUSE_PRESSED : VYTAL_EVENTCODE_MOUSE_RELEASED, ._mouse_code = mouse_code};
    input_module_invoke_event(data._event_code, NULL, &data);
}

void _window_module_callback_on_mouse_moved(VoidPtr context, Int32 x, Int32 y) {
    InputMouseMoveEventData data = {._event_code = VYTAL_EVENTCODE_MOUSE_MOVED, ._x = x, ._y = y};
    input_module_invoke_event(VYTAL_EVENTCODE_MOUSE_MOVED, NULL, &data);
}

void _window_module_callback_on_mouse_scrolled(VoidPtr context, Int8 scroll_value) {
    InputMouseScrollEventData data = {._event_code = VYTAL_EVENTCODE_MOUSE_SCROLLED, ._scroll_value = scroll_value};
    input_module_invoke_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, NULL, &data);
}

WindowModuleResult window_module_startup(void) {
    if (state) return WINDOW_MODULE_ERROR_ALREADY_INITIALIZED;

    // allocate state
    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("Modules", sizeof(WindowModuleState), (VoidPtr *)&state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(WindowModuleState));

    // configure state members
    {
        WindowProperties props_ = {0};
        // window properties
        {
            WindowCallbacks callbacks_ = {
                ._on_close          = _window_module_callback_on_window_close,
                ._on_key_pressed    = _window_module_callback_on_key_pressed,
                ._on_mouse_pressed  = _window_module_callback_on_mouse_pressed,
                ._on_mouse_moved    = _window_module_callback_on_mouse_moved,
                ._on_mouse_scrolled = _window_module_callback_on_mouse_scrolled,
            };
            props_._callbacks = callbacks_;

            File cvar_file_ = {0};
            Str  cvar_line_ = calloc(1, LINE_BUFFER_MAX_SIZE);
            if (!cvar_line_)
                return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;

            if (platform_filesystem_open_file(&cvar_file_, main_window_cvar_filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT) != FILE_SUCCESS)
                return WINDOW_MODULE_ERROR_FILE_OPEN_FAILED;

            while (platform_filesystem_read_line(&cvar_file_, NULL, &cvar_line_) == FILE_SUCCESS) {
                Str trimmed_ = _window_module_trim_whitespace(cvar_line_);
                if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

                Str pequal_ = strchr(trimmed_, '=');
                if (!pequal_) continue;

                *pequal_ = '\0';

                Str key_   = _window_module_trim_whitespace(trimmed_);
                Str value_ = _window_module_trim_whitespace(pequal_ + 1);

                // handle title
                if (!strcmp(key_, "title")) {
                    // props_._title
                    if (container_string_construct(value_, &props_._title) != CONTAINER_SUCCESS) {
                        if (memory_zone_deallocate("Modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
                            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

                        free(cvar_line_);
                        if (platform_filesystem_close_file(&cvar_file_) != FILE_SUCCESS)
                            return WINDOW_MODULE_ERROR_FILE_CLOSE_FAILED;

                        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
                    }
                }

                // handle x position
                else if (!strcmp(key_, "x")) {
                    if (!strcmp(value_, "RANDOM_POSITION")) {
                        props_._x = 0x1fff0000u;
                        continue;
                    }

                    Str   end_;
                    Int64 parsed_value_ = strtol(value_, &end_, 10);
                    if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                    props_._x = (Int32)parsed_value_;
                }

                // handle y position
                else if (!strcmp(key_, "y")) {
                    if (!strcmp(value_, "RANDOM_POSITION")) {
                        props_._y = 0x1fff0000u;
                        continue;
                    }

                    Str   end_;
                    Int64 parsed_value_ = strtol(value_, &end_, 10);
                    if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                    props_._y = (Int32)parsed_value_;
                }

                // handle width
                else if (!strcmp(key_, "width")) {
                    Str   end_;
                    Int64 parsed_value_ = strtol(value_, &end_, 10);
                    if (*end_ != '\0')
                        return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                    props_._width = (Int32)parsed_value_;
                }

                // handle height
                else if (!strcmp(key_, "height")) {
                    Str   end_;
                    Int64 parsed_value_ = strtol(value_, &end_, 10);
                    if (*end_ != '\0')
                        return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                    props_._height = (Int32)parsed_value_;
                }

                // handle title_bar flags
                else if (!strcmp(key_, "titlebar_flags")) {
                    Str   end_;
                    Int32 flags_ = strtol(value_, &end_, 2);
                    if (*end_ != '\0' || flags_ < 0)
                        return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                    props_._title_bar_flags = flags_;
                }

                // handle window backend
                else if (!strcmp(key_, "backend"))
                    props_._backend = WINDOW_BACKEND_GLFW;  // only GLFW is supported
            }
            free(cvar_line_);

            if (platform_filesystem_close_file(&cvar_file_) != FILE_SUCCESS)
                return WINDOW_MODULE_ERROR_FILE_CLOSE_FAILED;

            memcpy(&state->_main_window_props, &props_, sizeof(WindowProperties));
        }

        // startup platform window system
        if (platform_window_startup(state->_main_window_props._backend) != WINDOW_SUCCESS) {
            if (container_string_destruct(state->_main_window_props._title) != CONTAINER_SUCCESS)
                return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

            if (memory_zone_deallocate("Modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
                return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

            return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
        }

        state->_memory_size = alloc_size_;
        state->_initialized = true;
    }

    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_shutdown(void) {
    if (!state) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    // deallocate state members
    {
        // shutdown platform window system
        if (platform_window_shutdown() != WINDOW_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

        // main window properties
        if (container_string_destruct(state->_main_window_props._title) != CONTAINER_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;
        memset(&state->_main_window_props, 0, sizeof(WindowProperties));
    }

    if (memory_zone_deallocate("Modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_update(void) {
    if (!state) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    // do nothing.
    // the application will have explicit control over when to poll events or swap buffers.

    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_construct_main(void) {
    if (!state) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    if (platform_window_construct_window(
            state->_main_window_props,
            &state->_main_window_props._callbacks,
            &state->_main_window) != WINDOW_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;

    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_destruct_main(void) {
    if (!state) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    if (platform_window_destruct_window(state->_main_window) != WINDOW_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;

    return WINDOW_MODULE_SUCCESS;
}

Window window_module_get_main(void) {
    if (!state || !state->_initialized) return NULL;
    return state->_main_window;
}

WindowProperties window_module_get_properties(void) {
    if (!state || !state->_initialized) return (WindowProperties){0};
    return state->_main_window_props;
}
