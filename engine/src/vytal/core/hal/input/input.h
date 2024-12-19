#pragma once

#include "vytal/defines/core/delegate.h"
#include "vytal/defines/core/input.h"
#include "vytal/defines/shared.h"

VT_API Bool hal_input_is_key_down(const InputKeyCode key);
VT_API Bool hal_input_is_key_up(const InputKeyCode key);
VT_API Bool hal_input_was_key_down(const InputKeyCode key);
VT_API Bool hal_input_was_key_up(const InputKeyCode key);
VT_API Bool hal_input_is_key_pressed(const InputKeyCode key);
