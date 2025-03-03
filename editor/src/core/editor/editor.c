#include "editor.h"

#include <stdlib.h>
#include <string.h>

#include "core/input/input.h"
#include "core/logger/logger.h"

typedef struct Editor_State {
    Char   _base_path[LINE_BUFFER_MAX_SIZE];
    String _title;
    Window _main_window;
    Bool   _active;
    Bool   _suspended;
    Bool   _auto_play_on_launch;
    Flt64  _prev_frame;
    Flt64  _frame_time_ms;
    Flt64  _draw_time_ms;
    Flt64  _delta_time;
    UInt32 _last_report_time;
    Bool   _initialized;
} EditorState;

static EditorState *state = NULL;

static Window _editor_search_context_owner(VoidPtr context) {
    if (context == platform_window_get_handle(state->_main_window))
        return state->_main_window;

    return NULL;
}

static void _editor_on_event(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputEventData *data_   = (InputEventData *)data;
    Window          window_ = _editor_search_context_owner(sender);

    switch (data_->_event_code) {
        case VYTAL_EVENTCODE_WINDOW_CLOSE:
            editor_close_window(window_);
            return;

        default:
            break;
    }
}

static void _editor_on_key_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputKeyEventData *data_ = (InputKeyEventData *)data;
    input_module_process_key_pressed(data_->_key_code, data_->_event_code == VYTAL_EVENTCODE_KEY_PRESSED);
}

static void _editor_on_mouse_pressed(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseEventData *data_ = (InputMouseEventData *)data;
    input_module_process_mouse_pressed(data_->_mouse_code, data_->_event_code == VYTAL_EVENTCODE_MOUSE_PRESSED);
}

static void _editor_on_mouse_moved(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseMoveEventData *data_ = (InputMouseMoveEventData *)data;
    input_module_process_mouse_moved(data_->_x, data_->_y);
}

static void _editor_on_mouse_scrolled(VoidPtr sender, VoidPtr listener, VoidPtr data) {
    InputMouseScrollEventData *data_ = (InputMouseScrollEventData *)data;
    input_module_process_mouse_scrolled(data_->_scroll_value);
}

EditorResult _editor_parse_config(ConstStr config_filepath) {
    Char section_[LINE_BUFFER_MAX_SIZE] = {0};

    File file_ = {0};
    if (platform_filesystem_open_file(&file_, config_filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT) != FILE_SUCCESS)
        return EDITOR_ERROR_FILE_OPEN_FAILED;

    Str line_ = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(&file_, NULL, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS) continue;
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[') {
            sscanf(trimmed_, "[%[^]]]", section_);
            continue;
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;

        // general section
        if (!strcmp(section_, "general")) {
            // editor title
            if (!strcmp(key_, "title")) {
                if (container_string_construct(value_, &state->_title) != CONTAINER_SUCCESS)
                    return EDITOR_ERROR_ALLOCATION_FAILED;
            }

            // 'auto play on launch' setting
            else if (!strcmp(key_, "auto_play_on_launch")) {
                state->_auto_play_on_launch = (!strcmp(value_, "true"));
            }
        }
    }

    if (platform_filesystem_close_file(&file_) != FILE_SUCCESS)
        return EDITOR_ERROR_FILE_CLOSE_FAILED;

    free(line_);
    return EDITOR_SUCCESS;
}

EditorResult _editor_load_project(ConstStr project_filepath) {
    Char engine_path_[LINE_BUFFER_MAX_SIZE] = {0};
    Char editor_path_[LINE_BUFFER_MAX_SIZE] = {0};
    Char section_[LINE_BUFFER_MAX_SIZE]     = {0};

    File file_ = {0};
    if (platform_filesystem_open_file(&file_, project_filepath, FILE_IO_MODE_READ, FILE_MODE_TEXT) != FILE_SUCCESS)
        return EDITOR_ERROR_FILE_OPEN_FAILED;

    Str line_ = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(&file_, NULL, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS) continue;
        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[') {
            sscanf(trimmed_, "[%[^]]]", section_);
            continue;
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;

        // handle configurations
        {
            if (!strcmp(section_, "paths")) {
                if (!strcmp(key_, "config")) {
                    // engine configurations
                    {
                        snprintf(engine_path_, sizeof(engine_path_), "%s/%s/engine.cfg", state->_base_path, value_);
                        if (engine_preconstruct(engine_path_) != ENGINE_SUCCESS)
                            return EDITOR_ERROR_ENGINE_PRECONSTRUCT_FAILED;
                    }

                    // editor configurations
                    {
                        snprintf(editor_path_, sizeof(editor_path_), "%s/%s/editor.cfg", state->_base_path, value_);
                        if (_editor_parse_config(editor_path_) != EDITOR_SUCCESS)
                            return EDITOR_ERROR_PARSE_FAILED;
                    }
                }
            }

            else {
                // project basepath
                if (!strcmp(key_, "base_path"))
                    strcpy(state->_base_path, value_);
            }
        }
    }

    if (platform_filesystem_close_file(&file_) != FILE_SUCCESS)
        return EDITOR_ERROR_FILE_CLOSE_FAILED;

    free(line_);
    return EDITOR_SUCCESS;
}

EditorResult editor_startup(ConstStr project_filepath) {
    if (!project_filepath) return EDITOR_ERROR_INVALID_PARAM;
    if (state) return EDITOR_ERROR_ALREADY_INITIALIZED;

    // allocate editor state
    state = calloc(1, sizeof(EditorState));
    if (!state)
        return EDITOR_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(EditorState));

    EditorResult load_project_ = _editor_load_project(project_filepath);
    if (load_project_ != EDITOR_SUCCESS)
        return load_project_;

    // register events
    {
        input_module_register_event(VYTAL_EVENTCODE_WINDOW_CLOSE, NULL, _editor_on_event);
        input_module_register_event(VYTAL_EVENTCODE_KEY_PRESSED, NULL, _editor_on_key_pressed);
        input_module_register_event(VYTAL_EVENTCODE_KEY_RELEASED, NULL, _editor_on_key_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_PRESSED, NULL, _editor_on_mouse_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_RELEASED, NULL, _editor_on_mouse_pressed);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_MOVED, NULL, _editor_on_mouse_moved);
        input_module_register_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, NULL, _editor_on_mouse_scrolled);
    }

    // open main window
    {
        EditorResult open_main_window_ = editor_open_window(container_string_get(state->_title), &state->_main_window);
        if (open_main_window_ != EDITOR_SUCCESS)
            return open_main_window_;
    }

    // handle engine construction
    if (engine_construct() != ENGINE_SUCCESS)
        return EDITOR_ERROR_ENGINE_CONSTRUCT_FAILED;

    state->_active      = true;
    state->_initialized = true;
    return EDITOR_SUCCESS;
}

EditorResult editor_update(void) {
    String staging_title_;
    container_string_construct("", &staging_title_);

    WindowProperties props_ = window_module_default_properties();

    state->_prev_frame = platform_window_get_frame(state->_main_window);

    do {
        Flt64 frame_start_ = platform_window_get_frame(state->_main_window);

        {
            // update logics here...
        }

        // handle engine update
        if (engine_update() != ENGINE_SUCCESS)
            return EDITOR_ERROR_ENGINE_UPDATE_FAILED;

        state->_delta_time = platform_window_get_frame(state->_main_window) - frame_start_;

        // render
        {
            Flt64 render_frame_start_ = platform_window_get_frame(state->_main_window);

            // rendering goes here...

            Flt64 render_frame_end_ = platform_window_get_frame(state->_main_window);
            state->_draw_time_ms    = (render_frame_end_ - render_frame_start_) * 1000.0;
        }

        // swap buffers and poll events for main window
        {
            if (platform_window_swap_buffers(state->_main_window) != WINDOW_SUCCESS)
                return EDITOR_ERROR_ENGINE_UPDATE_FAILED;

            if (platform_window_poll_events(state->_main_window) != WINDOW_SUCCESS)
                return EDITOR_ERROR_ENGINE_UPDATE_FAILED;

            // make sure that the editor loop exits immediately in case shutdown is triggered
            // in order to prevent unnecessary frame processing and causing issues
            if (!state->_active) break;
        }

        Flt64 frame_end_      = platform_window_get_frame(state->_main_window);
        state->_frame_time_ms = (frame_end_ - frame_start_) * 1000.0;

        // report framerate, update window title
        {
            // clear the title string first
            container_string_clear(&staging_title_);

            // editor title
            container_string_append(&staging_title_, container_string_get(state->_title));

            // frame time
            if (VYTAL_BITFLAG_IF_SET(props_._title_bar_flags, WINDOW_TITLE_BAR_FLAG_FRAME_TIME))
                container_string_append_formatted(&staging_title_, "      frame_time: %.3f ms", state->_frame_time_ms);

            // draw time
            if (VYTAL_BITFLAG_IF_SET(props_._title_bar_flags, WINDOW_TITLE_BAR_FLAG_DRAW_TIME))
                container_string_append_formatted(&staging_title_, "      draw_time: %.3f ms", state->_draw_time_ms);

            // update title (based on specified rate)
            {
                Flt64 curr_ = platform_window_get_frame(state->_main_window) * 1000.0;
                if (curr_ - state->_last_report_time >= WINDOW_REPORT_INTERVAL_MS) {
                    platform_window_set_title(state->_main_window, container_string_get(staging_title_));
                    state->_last_report_time = curr_;
                }
            }
        }
    } while (state->_active);

    container_string_destruct(staging_title_);

    return EDITOR_SUCCESS;
}

EditorResult editor_shutdown(void) {
    if (!state || !state->_initialized) return EDITOR_ERROR_NOT_INITIALIZED;

    if (container_string_destruct(state->_title) != CONTAINER_SUCCESS)
        return EDITOR_ERROR_DEALLOCATION_FAILED;

    // unregister events
    {
        input_module_unregister_event(VYTAL_EVENTCODE_WINDOW_CLOSE, _editor_on_event);
        input_module_unregister_event(VYTAL_EVENTCODE_KEY_PRESSED, _editor_on_key_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_KEY_RELEASED, _editor_on_key_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_PRESSED, _editor_on_mouse_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_RELEASED, _editor_on_mouse_pressed);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_MOVED, _editor_on_mouse_moved);
        input_module_unregister_event(VYTAL_EVENTCODE_MOUSE_SCROLLED, _editor_on_mouse_scrolled);
    }

    // handle engine destruction
    if (engine_destruct() != ENGINE_SUCCESS)
        return EDITOR_ERROR_ENGINE_DESTRUCT_FAILED;

    free(state);
    state = NULL;

    return EDITOR_SUCCESS;
}

EditorResult editor_open_window(ConstStr title, Window *out_window) {
    if (!title || !out_window) return EDITOR_ERROR_INVALID_PARAM;

    if (window_module_construct_window(out_window) != WINDOW_MODULE_SUCCESS)
        return EDITOR_ERROR_WINDOW_OPEN_FAILED;

    if (platform_window_set_title(*out_window, title) != WINDOW_SUCCESS)
        return EDITOR_ERROR_WINDOW_OPERATION_FAILED;

    return EDITOR_SUCCESS;
}

EditorResult editor_close_window(Window window) {
    if (!window) return EDITOR_ERROR_INVALID_PARAM;

    if (platform_window_get_handle(window) == platform_window_get_handle(state->_main_window))
        state->_active = false;

    if (window_module_destruct_window(window) != WINDOW_MODULE_SUCCESS)
        return EDITOR_ERROR_WINDOW_CLOSE_FAILED;

    return EDITOR_SUCCESS;
}
