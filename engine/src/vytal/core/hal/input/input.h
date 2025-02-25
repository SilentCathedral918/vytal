#pragma once

#include "vytal/defines/core/delegates.h"
#include "vytal/defines/core/input.h"
#include "vytal/defines/shared.h"

// keyboard ------------------------------------------------------------- //

VYTAL_API Bool input_is_key_down(const InputKeyCode key);
VYTAL_API Bool input_is_key_up(const InputKeyCode key);
VYTAL_API Bool input_was_key_down(const InputKeyCode key);
VYTAL_API Bool input_was_key_up(const InputKeyCode key);
VYTAL_API Bool input_is_key_pressed(const InputKeyCode key);

// mouse ---------------------------------------------------------------- //

VYTAL_API Bool input_is_mouse_down(const InputMouseCode code);
VYTAL_API Bool input_is_mouse_up(const InputMouseCode code);
VYTAL_API Bool input_was_mouse_down(const InputMouseCode code);
VYTAL_API Bool input_was_mouse_up(const InputMouseCode code);
VYTAL_API Bool input_is_mouse_pressed(const InputMouseCode code);

VYTAL_API Int32 input_get_mouse_x(void);
VYTAL_API Int32 input_get_mouse_y(void);
VYTAL_API Int32 input_get_prev_mouse_x(void);
VYTAL_API Int32 input_get_prev_mouse_y(void);
VYTAL_API Bool  input_is_mouse_moved(void);

VYTAL_API Int8 input_get_mouse_scroll_value(void);
VYTAL_API Int8 input_get_mouse_scroll_value_inverted(void);
VYTAL_API Int8 input_get_prev_mouse_scroll_value(void);
VYTAL_API Int8 input_get_prev_mouse_scroll_value_inverted(void);
VYTAL_API Bool input_is_mouse_scrolled(void);
