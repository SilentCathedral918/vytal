#include "input.h"

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

#define input_module_state ((InputModuleState *)input_module_get_state())

// keyboard ------------------------------------------------------------- //

Bool input_is_key_down(const InputKeyCode key) {
    return (input_module_state->_curr_keyboard_state->_keys[key]);
}

Bool input_is_key_up(const InputKeyCode key) {
    return !(input_module_state->_curr_keyboard_state->_keys[key]);
}

Bool input_was_key_down(const InputKeyCode key) {
    return (input_module_state->_prev_keyboard_state->_keys[key]);
}

Bool input_was_key_up(const InputKeyCode key) {
    return !(input_module_state->_prev_keyboard_state->_keys[key]);
}

Bool input_is_key_pressed(const InputKeyCode key) {
    return (input_module_state->_curr_keyboard_state->_keys[key] &&
            !input_module_state->_prev_keyboard_state->_keys[key]);
}

Bool input_is_mouse_down(const InputMouseCode code) {
    return (input_module_state->_curr_mouse_state->_values[code]);
}

Bool input_is_mouse_up(const InputMouseCode code) {
    return !(input_module_state->_curr_mouse_state->_values[code]);
}

Bool input_was_mouse_down(const InputMouseCode code) {
    return (input_module_state->_prev_mouse_state->_values[code]);
}

Bool input_was_mouse_up(const InputMouseCode code) {
    return !(input_module_state->_prev_mouse_state->_values[code]);
}

Bool input_is_mouse_pressed(const InputMouseCode code) {
    return (input_module_state->_curr_mouse_state->_values[code] &&
            !input_module_state->_prev_mouse_state->_values[code]);
}

Int32 input_get_mouse_x(void) {
    return (input_module_state->_curr_mouse_state->_x);
}

Int32 input_get_mouse_y(void) {
    return (input_module_state->_curr_mouse_state->_y);
}

Int32 input_get_prev_mouse_x(void) {
    return (input_module_state->_prev_mouse_state->_x);
}

Int32 input_get_prev_mouse_y(void) {
    return (input_module_state->_prev_mouse_state->_y);
}

Bool input_is_mouse_moved(void) {
    return (input_get_mouse_x() != input_get_prev_mouse_x()) || (input_get_mouse_y() != input_get_prev_mouse_y());
}

Int8 input_get_mouse_scroll_value(void) {
    return (input_module_state->_curr_mouse_state->_scroll_value);
}

Int8 input_get_mouse_scroll_value_inverted(void) {
    return (input_module_state->_curr_mouse_state->_scroll_value * -1);
}

Int8 input_get_prev_mouse_scroll_value(void) {
    return (input_module_state->_prev_mouse_state->_scroll_value);
}

Int8 input_get_prev_mouse_scroll_value_inverted(void) {
    return (input_module_state->_prev_mouse_state->_scroll_value * -1);
}

Bool input_is_mouse_scrolled(void) {
    return (input_get_mouse_scroll_value() != 0);
}
