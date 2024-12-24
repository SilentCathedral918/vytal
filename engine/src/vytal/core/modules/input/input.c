#include "input.h"

#include "vytal/core/delegate/unicast/unicast.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"

#define KEY_CODES 512
#define MOUSE_CODES 8

typedef struct Input_Keyboard_State {
    Bool _keys[KEY_CODES];
} InputKeyboardState;

typedef struct Input_Mouse_State {
    Int32 _x, _y;
    Int8  _scroll_value;
    Bool  _values[MOUSE_CODES];
} InputMouseState;

typedef struct Input_Module_State {
    InputKeyboardState    _curr_keyboard_state, _prev_keyboard_state;
    InputMouseState       _curr_mouse_state, _prev_mouse_state;
    UnicastDelegate       _event_delegates[VT_EVENTCODES_TOTAL];
    UnicastDelegateHandle _event_handles[VT_EVENTCODES_TOTAL];
    Bool                  _initialized;
} InputModuleState;
static InputModuleState *state = NULL;

ByteSize input_module_get_size(void) { return sizeof(InputModuleState); }

Bool input_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // assign module to state and init its members
    state = VT_CAST(InputModuleState *, module);
    {
        // keyboard states
        hal_mem_memzero(&state->_curr_keyboard_state, sizeof(InputKeyboardState));
        hal_mem_memzero(&state->_prev_keyboard_state, sizeof(InputKeyboardState));

        // mouse states
        hal_mem_memzero(&state->_curr_mouse_state, sizeof(InputMouseState));
        hal_mem_memzero(&state->_prev_mouse_state, sizeof(InputMouseState));

        // event delegates
        hal_mem_memzero(&state->_event_delegates, sizeof(state->_event_delegates));

        // event delegate handles
        for (ByteSize i = 0; i < VT_EVENTCODES_TOTAL; ++i)
            state->_event_handles[i] = delegate_unicast_handle_construct();

        state->_initialized = true;
    }

    return true;
}

Bool input_module_shutdown(void) {
    if (!state)
        return false;

    // free and set members to zero
    {
        for (ByteSize i = 0; i < VT_EVENTCODES_TOTAL; ++i) {
            if (!delegate_unicast_handle_destruct(state->_event_handles[i]))
                return false;
        }

        hal_mem_memzero(state, sizeof(InputModuleState));
    }

    state = NULL;
    return true;
}

Bool input_module_update(void) {
    if (!state)
        return false;

    // copy current states to previous states
    {
        hal_mem_memcpy(&state->_prev_keyboard_state, &state->_curr_keyboard_state, sizeof(InputKeyboardState));
        hal_mem_memcpy(&state->_prev_mouse_state, &state->_curr_mouse_state, sizeof(InputMouseState));
    }

    // reset scroll
    state->_curr_mouse_state._scroll_value = 0;

    return true;
}

Bool input_module_register_event(const InputEventCode code, const DelegateFunc callback) {
    if (!state)
        return false;

    if (!state->_event_delegates[code])
        state->_event_delegates[code] = delegate_unicast_construct(state->_event_handles[code], callback);

    if (!delegate_unicast_handle_bind(state->_event_handles[code], state->_event_delegates[code]))
        return false;

    return true;
}

Bool input_module_unregister_event(const InputEventCode code, const DelegateFunc callback) {
    if (!state)
        return false;

    UnicastDelegate del_ = state->_event_delegates[code];
    if (!del_)
        return false;

    if (!delegate_unicast_handle_unbind(state->_event_handles[code], state->_event_delegates[code]))
        return false;

    if (!delegate_unicast_destruct(state->_event_delegates[code]))
        return false;
    state->_event_delegates[code] = NULL;

    return true;
}

Bool input_module_invoke_event(const InputEventCode code, const VoidPtr data) {
    if (!state)
        return false;

    UnicastDelegateHandle handle_ = state->_event_handles[code];
    if (!handle_)
        return false;

    return delegate_unicast_handle_invoke(state->_event_handles[code], NULL, data);
}

Bool input_module_process_key_pressed(const InputKeyCode code, const Bool pressed) {
    if (!state)
        return false;

    if (state->_curr_keyboard_state._keys[code] != pressed) {
        // update current keyboard state
        state->_curr_keyboard_state._keys[code] = pressed;

        // immediately invoke the key event
        {
            InputKeyEventData data_ = {._event_code = pressed ? VT_EVENTCODE_KEY_PRESSED : VT_EVENTCODE_KEY_RELEASED,
                                       ._key_code   = code};

            UnicastDelegate del_ = state->_event_delegates[data_._event_code];
            if (!del_)
                return false;

            UnicastDelegateHandle handle_ = state->_event_handles[data_._event_code];
            if (!handle_)
                return false;

            if (!delegate_unicast_handle_invoke(handle_, NULL, &data_))
                return false;
        }
    }

    return true;
}

Bool input_module_process_mouse_pressed(const InputMouseCode code, const Bool pressed) {
    if (!state)
        return false;

    if (state->_curr_mouse_state._values[code] != pressed) {
        // update current mouse state
        state->_curr_mouse_state._values[code] = pressed;

        // immediately invoke the mouse event
        {
            InputMouseEventData data_ = {._event_code = pressed ? VT_EVENTCODE_MOUSE_PRESSED : VT_EVENTCODE_MOUSE_RELEASED,
                                         ._mouse_code = code};

            UnicastDelegate del_ = state->_event_delegates[data_._event_code];
            if (!del_)
                return false;

            UnicastDelegateHandle handle_ = state->_event_handles[data_._event_code];
            if (!handle_)
                return false;

            if (!delegate_unicast_handle_invoke(handle_, NULL, &data_))
                return false;
        }
    }

    return true;
}

Bool input_module_process_mouse_moved(const Int32 x, const Int32 y) {
    if (!state)
        return false;

    if ((state->_curr_mouse_state._x != x) || (state->_curr_mouse_state._y != y)) {
        // update current mouse positions
        {
            state->_curr_mouse_state._x = x;
            state->_curr_mouse_state._y = y;
        }

        // immediately invoke the mouse move event
        {
            InputMouseMoveEventData data_ = {._event_code = VT_EVENTCODE_MOUSE_MOVED, ._x = x, ._y = y};

            UnicastDelegate del_ = state->_event_delegates[data_._event_code];
            if (!del_)
                return false;

            UnicastDelegateHandle handle_ = state->_event_handles[data_._event_code];
            if (!handle_)
                return false;

            if (!delegate_unicast_handle_invoke(handle_, NULL, &data_))
                return false;
        }
    }

    return true;
}

Bool input_module_process_mouse_scrolled(const Int8 scroll_value) {
    if (!state)
        return false;

    if (state->_curr_mouse_state._scroll_value == 0) {
        // update mouse scroll
        state->_curr_mouse_state._scroll_value = scroll_value;

        // immediately invoke the mouse scroll event
        {
            InputMouseScrollEventData data_ = {._event_code = VT_EVENTCODE_MOUSE_SCROLLED, ._scroll_value = scroll_value};

            UnicastDelegate del_ = state->_event_delegates[data_._event_code];
            if (!del_)
                return false;

            UnicastDelegateHandle handle_ = state->_event_handles[data_._event_code];
            if (!handle_)
                return false;

            if (!delegate_unicast_handle_invoke(handle_, NULL, &data_))
                return false;
        }
    }

    return true;
}

VoidPtr input_module_get_state(void) { return state; }

Bool input_module_set_state(VoidPtr new_state) {
    return !state ? false : (hal_mem_memcpy(state, new_state, sizeof(InputModuleState)));
}
