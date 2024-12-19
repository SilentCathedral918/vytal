#include "input.h"

#include "vytal/core/delegate/unicast/unicast.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/modules/input/input.h"

#define KEY_CODES 128
#define MOUSE_CODES 8

typedef struct Input_Keyboard_State {
    Bool _keys[KEY_CODES];
} InputKeyboardState;

typedef struct Input_Mouse_State {
    UInt16 _x, _y;
    Int8   _scroll_value;
    Bool   _values[MOUSE_CODES];
} InputMouseState;

typedef struct Input_Module_State {
    InputKeyboardState    _curr_keyboard_state, _prev_keyboard_state;
    InputMouseState       _curr_mouse_state, _prev_mouse_state;
    UnicastDelegate       _event_delegates[VT_EVENTCODES_TOTAL];
    UnicastDelegateHandle _event_handles[VT_EVENTCODES_TOTAL];
    Bool                  _initialized;
} InputModuleState;

#define input_module_state VT_CAST(InputModuleState *, input_module_get_state())

Bool hal_input_is_key_down(const InputKeyCode key) { return (input_module_state->_curr_keyboard_state._keys[key]); }
Bool hal_input_is_key_up(const InputKeyCode key) { return !(input_module_state->_curr_keyboard_state._keys[key]); }
Bool hal_input_was_key_down(const InputKeyCode key) { return (input_module_state->_prev_keyboard_state._keys[key]); }
Bool hal_input_was_key_up(const InputKeyCode key) { return !(input_module_state->_prev_keyboard_state._keys[key]); }
Bool hal_input_is_key_pressed(const InputKeyCode key) { return (hal_input_is_key_up(key) && hal_input_was_key_down(key)); }
