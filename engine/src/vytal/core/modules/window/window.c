#include "window.h"

#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/string/string.h"
#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/modules/input/input.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/core/platform/window/window.h"

typedef struct Window_Module_State {
    WindowProperties _default_window_props;
    Bool             _initialized;
    ByteSize         _memory_size;
} WindowModuleState;

static WindowModuleState *state = NULL;

// callbacks ------------------------------------------------------------ //

void _window_module_callback_on_window_close(VoidPtr context) {
    InputEventData data = {._event_code = VYTAL_EVENTCODE_WINDOW_CLOSE};
    input_module_invoke_event(VYTAL_EVENTCODE_WINDOW_CLOSE, context, &data);
}

void _window_module_callback_on_key_pressed(VoidPtr context, InputKeyCode key_code, Bool pressed) {
    InputKeyEventData data = {._event_code = pressed ? VYTAL_EVENTCODE_KEY_PRESSED : VYTAL_EVENTCODE_KEY_RELEASED, ._key_code = key_code};
    input_module_invoke_event(data._event_code, context, &data);
}

void _window_module_callback_on_mouse_pressed(VoidPtr context, InputMouseCode mouse_code, Bool pressed) {
    InputMouseEventData data = {._event_code = pressed ? VYTAL_EVENTCODE_MOUSE_PRESSED : VYTAL_EVENTCODE_MOUSE_RELEASED, ._mouse_code = mouse_code};
    input_module_invoke_event(data._event_code, context, &data);
}

void _window_module_callback_on_mouse_moved(VoidPtr context, Int32 x, Int32 y) {
    InputMouseMoveEventData data = {._event_code = VYTAL_EVENTCODE_MOUSE_MOVED, ._x = x, ._y = y};
    input_module_invoke_event(VYTAL_EVENTCODE_MOUSE_MOVED, context, &data);
}

void _window_module_callback_on_mouse_scrolled(VoidPtr context, Int8 scroll_value) {
    InputMouseScrollEventData data = {._event_code = VYTAL_EVENTCODE_MOUSE_SCROLLED, ._scroll_value = scroll_value};
    input_module_invoke_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, context, &data);
}

// main ----------------------------------------------------------------- //

WindowModuleResult window_module_startup(File *file) {
    if (!file) return WINDOW_MODULE_ERROR_INVALID_PARAM;

    // allocate window module state and configure its members
    ByteSize alloc_size_ = 0;
    if (memory_zone_allocate("modules", sizeof(WindowModuleState), (VoidPtr *)&state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(WindowModuleState));
    state->_memory_size = alloc_size_;

    // handle default window properties - title
    if (container_string_construct("vytal_engine", &state->_default_window_props._title) != CONTAINER_SUCCESS) {
        if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
    }

    // handle default window properties - callbacks
    {
        WindowCallbacks callbacks_ = {
            ._on_close          = _window_module_callback_on_window_close,
            ._on_key_pressed    = _window_module_callback_on_key_pressed,
            ._on_mouse_pressed  = _window_module_callback_on_mouse_pressed,
            ._on_mouse_moved    = _window_module_callback_on_mouse_moved,
            ._on_mouse_scrolled = _window_module_callback_on_mouse_scrolled,
        };

        state->_default_window_props._callbacks = callbacks_;
    }

    ByteSize seek_length_ = 0;
    Str      line_        = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(file, &seek_length_, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
            return WINDOW_MODULE_ERROR_PARSE_FAILED;

        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[') {
            platform_filesystem_seek_from_current(file, -seek_length_);
            break;
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;

        Str end_;

        // handle default properties - from config file
        {
            // x position
            if (!strcmp(key_, "x")) {
                if (!strcmp(value_, "RANDOM_POSITION")) {
                    state->_default_window_props._x = 0x1fff0000u;
                    continue;
                }

                Int64 parsed_value_ = strtol(value_, &end_, 10);
                if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                state->_default_window_props._x = (Int32)parsed_value_;
            }

            // y position
            else if (!strcmp(key_, "y")) {
                if (!strcmp(value_, "RANDOM_POSITION")) {
                    state->_default_window_props._y = 0x1fff0000u;
                    continue;
                }

                Int64 parsed_value_ = strtol(value_, &end_, 10);
                if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                state->_default_window_props._y = (Int32)parsed_value_;
            }

            // width
            else if (!strcmp(key_, "width")) {
                Int64 parsed_value_ = strtol(value_, &end_, 10);
                if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                state->_default_window_props._width = (Int32)parsed_value_;
            }

            // height
            else if (!strcmp(key_, "height")) {
                Int64 parsed_value_ = strtol(value_, &end_, 10);
                if (*end_ != '\0') return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                state->_default_window_props._height = (Int32)parsed_value_;
            }

            // window mode
            else if (!strcmp(key_, "window_mode")) {
                if (!strcmp(value_, "windowed"))
                    state->_default_window_props._mode = WINDOW_MODE_WINDOWED;

                else if (!strcmp(value_, "fullscreen"))
                    state->_default_window_props._mode = WINDOW_MODE_FULLSCREEN;

                else if (!strcmp(value_, "borderless"))
                    state->_default_window_props._mode = WINDOW_MODE_BORDERLESS;
            }

            // title-bar flags
            else if (!strcmp(key_, "title_bar_flags")) {
                Str   end_;
                Int32 flags_ = strtol(value_, &end_, 2);
                if (*end_ != '\0' || flags_ < 0)
                    return WINDOW_MODULE_ERROR_INVALID_CONFIG;
                state->_default_window_props._title_bar_flags = flags_;
            }

            // enable vsync
            else if (!strcmp(key_, "vsync"))
                state->_default_window_props._enable_vsync = (!strcmp(value_, "on"));

            // backend
            else if (!strcmp(key_, "backend"))
                state->_default_window_props._backend = WINDOW_BACKEND_GLFW;
        }
    }
    free(line_);

    // handle platform window system startup
    if (platform_window_startup(state->_default_window_props._backend) != WINDOW_SUCCESS) {
        if (container_string_destruct(state->_default_window_props._title) != CONTAINER_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

        if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;
    }

    state->_initialized = true;
    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_shutdown(void) {
    if (!state || !state->_initialized)
        return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    // handle platform window system shutdown
    if (platform_window_shutdown() != WINDOW_SUCCESS)
        return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

    // deallocate state members
    {
        // default window properties
        if (container_string_destruct(state->_default_window_props._title) != CONTAINER_SUCCESS)
            return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;
        memset(&state->_default_window_props, 0, sizeof(WindowProperties));
    }

    if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return WINDOW_MODULE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_update(void) {
    if (!state || !state->_initialized)
        return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    // do nothing.
    // the programs using the engine will have complete autonomy on polling events or swapping buffers.
    // this function is crafted for the sake of maintaining module lifecycle uniformity (all modules must have startup, shutdown, and update)

    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_construct_window(Window *out_new_window) {
    if (!state || !state->_initialized)
        return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    if (platform_window_construct_window(
            state->_default_window_props,
            &state->_default_window_props._callbacks,
            out_new_window) != WINDOW_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;

    return WINDOW_MODULE_SUCCESS;
}

WindowModuleResult window_module_destruct_window(Window window) {
    if (!state || !state->_initialized)
        return WINDOW_MODULE_ERROR_NOT_INITIALIZED;

    if (platform_window_destruct_window(window) != WINDOW_SUCCESS)
        return WINDOW_MODULE_ERROR_ALLOCATION_FAILED;

    return WINDOW_MODULE_SUCCESS;
}

WindowProperties window_module_default_properties(void) {
    if (!state || !state->_initialized) return (WindowProperties){0};
    return state->_default_window_props;
}
