#pragma once

#include "vytal/defines/core/delegate.h"
#include "vytal/defines/core/input.h"
#include "vytal/defines/shared.h"

// --------------------------- keyboard --------------------------- //

VT_API Bool hal_input_is_key_down(const InputKeyCode key);
VT_API Bool hal_input_is_key_up(const InputKeyCode key);
VT_API Bool hal_input_was_key_down(const InputKeyCode key);
VT_API Bool hal_input_was_key_up(const InputKeyCode key);
VT_API Bool hal_input_is_key_pressed(const InputKeyCode key);

// --------------------------- mouse --------------------------- //

VT_API Bool  hal_input_is_mouse_down(const InputMouseCode code);
VT_API Bool  hal_input_is_mouse_up(const InputMouseCode code);
VT_API Bool  hal_input_was_mouse_down(const InputMouseCode code);
VT_API Bool  hal_input_was_mouse_up(const InputMouseCode code);
VT_API Bool  hal_input_is_mouse_pressed(const InputMouseCode code);
VT_API Int32 hal_input_get_mouse_x(void);
VT_API Int32 hal_input_get_mouse_y(void);
VT_API Int32 hal_input_get_prev_mouse_x(void);
VT_API Int32 hal_input_get_prev_mouse_y(void);
VT_API Bool  hal_input_is_mouse_moved(void);
VT_API Int8  hal_input_get_mouse_scroll_value(void);
VT_API Int8  hal_input_get_mouse_scroll_value_inverted(void);
VT_API Int8  hal_input_get_prev_mouse_scroll_value(void);
VT_API Int8  hal_input_get_prev_mouse_scroll_value_inverted(void);
VT_API Bool  hal_input_is_mouse_scrolled(void);