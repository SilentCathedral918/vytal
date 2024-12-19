#pragma once

#include "vytal/defines/core/delegate.h"
#include "vytal/defines/core/input.h"
#include "vytal/defines/shared.h"

VT_API ByteSize input_module_get_size(void);

VT_API Bool input_module_startup(VoidPtr module);
VT_API Bool input_module_shutdown(void);
VT_API Bool input_module_update(void);
VT_API Bool input_module_register_event(const InputEventCode code, const DelegateFunc callback);
VT_API Bool input_module_unregister_event(const InputEventCode code, const DelegateFunc callback);
VT_API Bool input_module_invoke_event(const InputEventCode code, const VoidPtr data);
VT_API Bool input_module_process_key_pressed(const InputKeyCode code, const Bool pressed);
VT_API Bool input_module_process_mouse_pressed(const InputMouseCode code, const Bool pressed);
VT_API Bool input_module_process_mouse_moved(const UInt16 x, const UInt16 y);
VT_API Bool input_module_process_mouse_scrolled(const Int8 scroll_value);

VT_API VoidPtr input_module_get_state(void);
VT_API Bool    input_module_set_state(VoidPtr new_state);