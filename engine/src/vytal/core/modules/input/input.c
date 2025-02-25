#include "input.h"

#include <string.h>

#include "vytal/core/delegates/unicast/unicast.h"
#include "vytal/core/memory/zone/memory_zone.h"
#include "vytal/core/misc/console/console.h"

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
    Bool                _initialized;
    ByteSize            _memory_size;
} InputModuleState;

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

InputModuleResult input_module_startup(void) {
    if (state) return INPUT_MODULE_ERROR_ALREADY_INITIALIZED;

    // allocate module state
    ByteSize state_allocated_size_ = 0;
    if (memory_zone_allocate("Modules", sizeof(InputModuleState), (VoidPtr *)&state, &state_allocated_size_) != MEMORY_ZONE_SUCCESS)
        return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
    memset(state, 0, sizeof(InputModuleState));

    // configure module state
    {
        ByteSize alloc_size_ = 0;

        // current keyboard state
        {
            if (memory_zone_allocate("Input", sizeof(InputKeyboardState), (VoidPtr *)&state->_curr_keyboard_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_curr_keyboard_state, 0, sizeof(InputKeyboardState));
            state->_curr_keyboard_state->_memory_size = alloc_size_;
        }

        // previous keyboard state
        {
            if (memory_zone_allocate("Input", sizeof(InputKeyboardState), (VoidPtr *)&state->_prev_keyboard_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_prev_keyboard_state, 0, sizeof(InputKeyboardState));
            state->_prev_keyboard_state->_memory_size = alloc_size_;
        }

        // current mouse state
        {
            if (memory_zone_allocate("Input", sizeof(InputMouseState), (VoidPtr *)&state->_curr_mouse_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_curr_mouse_state, 0, sizeof(InputMouseState));
            state->_curr_mouse_state->_memory_size = alloc_size_;
        }

        // previous mouse state
        {
            if (memory_zone_allocate("Input", sizeof(InputMouseState), (VoidPtr *)&state->_prev_mouse_state, &alloc_size_) != MEMORY_ZONE_SUCCESS)
                return INPUT_MODULE_ERROR_ALLOCATION_FAILED;
            memset(state->_prev_mouse_state, 0, sizeof(InputMouseState));
            state->_prev_mouse_state->_memory_size = alloc_size_;
        }

        state->_memory_size = state_allocated_size_;
        state->_initialized = true;
    }

    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_shutdown(void) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    // deallocate state members
    {
        // current keyboard state
        if (memory_zone_deallocate("Input", state->_curr_keyboard_state, state->_curr_keyboard_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_curr_keyboard_state = NULL;

        // previous keyboard state
        if (memory_zone_deallocate("Input", state->_prev_keyboard_state, state->_prev_keyboard_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_prev_keyboard_state = NULL;

        // current mouse state
        if (memory_zone_deallocate("Input", state->_curr_mouse_state, state->_curr_mouse_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_curr_mouse_state = NULL;

        // previous mouse state
        if (memory_zone_deallocate("Input", state->_prev_mouse_state, state->_prev_mouse_state->_memory_size) != MEMORY_ZONE_SUCCESS)
            return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;
        state->_prev_mouse_state = NULL;
    }

    // deallocate state self
    if (memory_zone_deallocate("Modules", state, state->_memory_size) != MEMORY_ZONE_SUCCESS)
        return INPUT_MODULE_ERROR_DEALLOCATION_FAILED;

    state = NULL;
    return INPUT_MODULE_SUCCESS;
}

InputModuleResult input_module_update(void) {
    if (!state) return INPUT_MODULE_ERROR_NOT_INITIALIZED;
    if (!state->_initialized) return INPUT_MODULE_ERROR_NOT_INITIALIZED;

    // swap curr_states to prev_states for record
    {
        InputKeyboardState *temp_keyboard_state_ = state->_prev_keyboard_state;
        state->_prev_keyboard_state              = state->_curr_keyboard_state;
        state->_curr_keyboard_state              = temp_keyboard_state_;

        InputMouseState *temp_mouse_state_ = state->_prev_mouse_state;
        state->_prev_mouse_state           = state->_curr_mouse_state;
        state->_curr_mouse_state           = temp_mouse_state_;
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
