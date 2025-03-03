#include "input.h"

#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/map/map.h"
#include "vytal/core/delegates/unicast/unicast.h"
#include "vytal/core/helpers/parse/parse.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/platform/filesystem/filesystem.h"

typedef struct Input_Keyboard_State {
    Bool     _keys[VYTAL_KEYCODES_TOTAL];
    ByteSize _memory_size;
} InputKeyboardState;

typedef struct Input_Mouse_State {
    Int32    _x, _y;
    Int8     _scroll_value;
    Bool     _values[VYTAL_MOUSECODES_TOTAL];
    ByteSize _memory_size;
} InputMouseState;

typedef struct Input_Module_State {
    InputKeyboardState *_curr_keyboard_state, *_prev_keyboard_state;
    InputMouseState    *_curr_mouse_state, *_prev_mouse_state;
    Flt32               _mouse_sensitivity;
    Bool                _invert_y_axis;
    Map                 _key_bindings_map;
    Bool                _initialized;
    ByteSize            _memory_size;
} InputModuleState;

typedef struct {
    ConstStr     _name;
    InputKeyCode _code;
} KeyBindingEntry;

static KeyBindingEntry key_binding_table[] = {
    {"w", VYTAL_KEYCODE_W},
    {"a", VYTAL_KEYCODE_A},
    {"s", VYTAL_KEYCODE_S},
    {"d", VYTAL_KEYCODE_D},
    {"space", VYTAL_KEYCODE_SPACE},
    {"ctrl", VYTAL_KEYCODE_LEFT_CONTROL},
    {"shift", VYTAL_KEYCODE_LEFT_SHIFT},
    {"enter", VYTAL_KEYCODE_ENTER},
    {"escape", VYTAL_KEYCODE_ESCAPE},
};
static InputModuleState *state = NULL;

ConstStr event_code_names[] = {
    // application events
    "VYTAL_EVENTCODE_WINDOW_CLOSE",

    // key events
    "VYTAL_EVENTCODE_KEY_PRESSED",
    "VYTAL_EVENTCODE_KEY_RELEASED",

    // mouse events
    "VYTAL_EVENTCODE_MOUSE_PRESSED",
    "VYTAL_EVENTCODE_MOUSE_RELEASED",
    "VYTAL_EVENTCODE_MOUSE_MOVED",
    "VYTAL_EVENTCODE_MOUSE_SCROLLED",

    // window events
    "VYTAL_EVENTCODE_RESIZED",

    // test events
    "VYTAL_EVENTCODE_TESTUNIT_00",
    "VYTAL_EVENTCODE_TESTUNIT_01",
    "VYTAL_EVENTCODE_TESTUNIT_02",
    "VYTAL_EVENTCODE_TESTUNIT_03",
    "VYTAL_EVENTCODE_TESTUNIT_04",
};

InputModuleResult input_module_startup(File *file) {
    if (!file) return INPUT_MODULE_ERROR_INVALID_PARAM;

    // allocate module state and configure its members
    ByteSize state_allocated_size_ = 0;
    if (memory_zone_allocate("modules", sizeof(InputModuleState), (VoidPtr *)&state, &state_allocated_size_) != MEMORY_ZONE_SUCCESS)
        return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(InputModuleState));
    state->_memory_size = state_allocated_size_;

    // configure input states
    {
        ByteSize alloc_size_ = 0;

        // current keyboard state
        {
            if (memory_zone_allocate("input", sizeof(InputKeyboardState), (VoidPtr *)&state->_curr_keyboard_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_curr_keyboard_state, 0, sizeof(InputKeyboardState));
            state->_curr_keyboard_state->_memory_size = alloc_size_;
        }

        // previous keyboard state
        {
            if (memory_zone_allocate("input", sizeof(InputKeyboardState), (VoidPtr *)&state->_prev_keyboard_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_prev_keyboard_state, 0, sizeof(InputKeyboardState));
            state->_prev_keyboard_state->_memory_size = alloc_size_;
        }

        // current mouse state
        {
            if (memory_zone_allocate("input", sizeof(InputMouseState), (VoidPtr *)&state->_curr_mouse_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_curr_mouse_state, 0, sizeof(InputMouseState));
            state->_curr_mouse_state->_memory_size = alloc_size_;
        }

        // previous mouse state
        {
            if (memory_zone_allocate("input", sizeof(InputMouseState), (VoidPtr *)&state->_prev_mouse_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_prev_mouse_state, 0, sizeof(InputMouseState));
            state->_prev_mouse_state->_memory_size = alloc_size_;
        }
    }

    // key bindings
    if (container_map_construct(sizeof(InputKeyCode), &state->_key_bindings_map) != CONTAINER_SUCCESS) {
        if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;

        return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
    }

    Char section_[FILENAME_BUFFER_MAX_SIZE] = {0};

    ByteSize seek_length_ = 0;
    Str      line_        = calloc(1, LINE_BUFFER_MAX_SIZE);
    while (platform_filesystem_read_line(file, &seek_length_, &line_) == FILE_SUCCESS) {
        Str trimmed_ = line_;

        if (parse_trim_whitespace(&trimmed_) != PARSE_SUCCESS)
            return INPUT_MODULE_ERROR_PARSE_FAILED;

        if (*trimmed_ == '#' || *trimmed_ == '\0') continue;

        // check for new section header (e.g. [section_name])
        if (*trimmed_ == '[') {
            sscanf(trimmed_, "[%[^]]]", section_);

            if (strncmp(section_, "input", 5)) {
                platform_filesystem_seek_from_current(file, -seek_length_);
                break;
            } else
                continue;
        }

        Char key_[LINE_BUFFER_MAX_SIZE]   = {0};
        Char value_[LINE_BUFFER_MAX_SIZE] = {0};
        if (!parse_key_value(trimmed_, key_, value_)) continue;

        Str end_;

        // handle general settings
        if (!strcmp(section_, "input.general")) {
            // mouse sensitivity
            if (!strcmp(key_, "mouse_sensitivity")) {
                Flt32 parsed_value_ = strtof(value_, &end_);
                if (*end_ != '\0') return INPUT_MODULE_ERROR_PARSE_FAILED;
                state->_mouse_sensitivity = parsed_value_;
            }

            // invert y-axis
            else if (!strcmp(key_, "invert_y_axis")) {
                state->_invert_y_axis = (!strcmp(value_, "true"));
            }
        }

        // handle bindings
        else if (!strcmp(section_, "input.bindings")) {
            ByteSize table_length_ = sizeof(key_binding_table) / sizeof(*key_binding_table);
            for (Int32 i = 0; i < table_length_; ++i) {
                if (!strcmp(key_binding_table[i]._name, key_)) {
                    if (container_map_insert(&state->_key_bindings_map, key_, &key_binding_table[i]._code) != CONTAINER_SUCCESS)
                        return INPUT_MODULE_ERROR_DATA_INSERT_FAILED;
                }
            }
        }
    }
    free(line_);

    state->_initialized = true;
    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_shutdown(void) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    // key bindings map
    {
        if (container_map_destruct(state->_key_bindings_map) != CONTAINER_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;

        state->_key_bindings_map = NULL;
    }

    // deallocate state members
    {
        // current keyboard state
        if (memory_zone_deallocate("input", state->_curr_keyboard_state, state->_curr_keyboard_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_curr_keyboard_state = NULL;

        // previous keyboard state
        if (memory_zone_deallocate("input", state->_prev_keyboard_state, state->_prev_keyboard_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_prev_keyboard_state = NULL;

        // current mouse state
        if (memory_zone_deallocate("input", state->_curr_mouse_state, state->_curr_mouse_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_curr_mouse_state = NULL;

        // previous mouse state
        if (memory_zone_deallocate("input", state->_prev_mouse_state, state->_prev_mouse_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_prev_mouse_state = NULL;
    }

    // deallocate state self
    if (memory_zone_deallocate("modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_update(void) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    // record curr_state to prev_state
    {
        memcpy(state->_prev_keyboard_state, state->_curr_keyboard_state, sizeof(InputKeyboardState));
        memcpy(state->_prev_mouse_state, state->_curr_mouse_state, sizeof(InputMouseState));
    }

    // reset scroll value
    state->_curr_mouse_state->_scroll_value = 0;

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_register_event(const InputEventCode code, const VoidPtr listener, const DelegateFunction callback) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (delegate_unicast_bind(event_code_names[code], listener, callback) != DELEGATE_SUCCESS)
        return INPUT_MODULE_ERROR_EVENT_BIND_FAILED;

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_unregister_event(const InputEventCode code, const DelegateFunction callback) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (delegate_unicast_unbind(event_code_names[code]) != DELEGATE_SUCCESS)
        return INPUT_MODULE_ERROR_EVENT_UNBIND_FAILED;

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_invoke_event(const InputEventCode code, const VoidPtr sender, const VoidPtr data) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (delegate_unicast_invoke(event_code_names[code], sender, data) != DELEGATE_SUCCESS)
        return INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED;

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_process_key_pressed(const InputKeyCode code, const Bool pressed) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (state->_curr_keyboard_state->_keys[code] != pressed) {
        // update current keyboard state
        state->_curr_keyboard_state->_keys[code] = pressed;

        // immediately invoke the key event
        {
            InputKeyEventData data_ = {
                ._event_code = pressed ? VYTAL_EVENTCODE_KEY_PRESSED : VYTAL_EVENTCODE_KEY_RELEASED,
                ._key_code   = code,
            };

            if (delegate_unicast_invoke(event_code_names[data_._event_code], NULL, &data_) != DELEGATE_SUCCESS)
                return INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED;
        }
    }

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_process_mouse_pressed(const InputMouseCode code, const Bool pressed) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (state->_curr_mouse_state->_values[code] != pressed) {
        // update current mouse state
        state->_curr_mouse_state->_values[code] = pressed;

        // immediately invoke the mouse event
        {
            InputMouseEventData data_ = {
                ._event_code = pressed ? VYTAL_EVENTCODE_MOUSE_PRESSED : VYTAL_EVENTCODE_MOUSE_RELEASED,
                ._mouse_code = code,
            };

            if (delegate_unicast_invoke(event_code_names[data_._event_code], NULL, &data_) != DELEGATE_SUCCESS)
                return INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED;
        }
    }

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_process_mouse_moved(const Int32 x, const Int32 y) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if ((state->_curr_mouse_state->_x != x) || (state->_curr_mouse_state->_y != y)) {
        // update current mouse positions
        {
            state->_curr_mouse_state->_x = x;
            state->_curr_mouse_state->_y = y;
        }

        // immediately invoke the mouse move event
        {
            InputMouseMoveEventData data_ = {
                ._event_code = VYTAL_EVENTCODE_MOUSE_MOVED,
                ._x          = x,
                ._y          = y,
            };

            if (delegate_unicast_invoke(event_code_names[data_._event_code], NULL, &data_) != DELEGATE_SUCCESS)
                return INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED;
        }
    }

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_process_mouse_scrolled(const Int8 scroll_value) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    if (state->_curr_mouse_state->_scroll_value == 0) {
        // update mouse scroll
        state->_curr_mouse_state->_scroll_value = scroll_value;

        // immediately invoke the mouse scroll event
        {
            InputMouseScrollEventData data_ = {
                ._event_code   = VYTAL_EVENTCODE_MOUSE_SCROLLED,
                ._scroll_value = scroll_value,
            };

            if (delegate_unicast_invoke(event_code_names[data_._event_code], NULL, &data_) != DELEGATE_SUCCESS)
                return INPUT_MODULE_ERROR_EVENT_INVOKE_FAILED;
        }
    }

    return INPUT_MODULE_SUCCESS;
}

VoidPtr input_module_get_state(void) {
    return state;
}

void input_module_set_state(VoidPtr new_state) {
    if (!state || !new_state)
        return;

    *state = *(InputModuleState *)new_state;
}
