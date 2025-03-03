#pragma once

#include "vytal/defines/core/delegates.h"
#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/core/input.h"
#include "vytal/defines/shared.h"

VYTAL_API InputModuleResult input_module_startup(File *file);
VYTAL_API InputModuleResult input_module_shutdown(void);
VYTAL_API InputModuleResult input_module_update(void);
VYTAL_API InputModuleResult input_module_register_event(const InputEventCode code, const VoidPtr listener, const DelegateFunction callback);
VYTAL_API InputModuleResult input_module_unregister_event(const InputEventCode code, const DelegateFunction callback);
VYTAL_API InputModuleResult input_module_invoke_event(const InputEventCode code, const VoidPtr sender, const VoidPtr data);
VYTAL_API InputModuleResult input_module_process_key_pressed(const InputKeyCode code, const Bool pressed);
VYTAL_API InputModuleResult input_module_process_mouse_pressed(const InputMouseCode code, const Bool pressed);
VYTAL_API InputModuleResult input_module_process_mouse_moved(const Int32 x, const Int32 y);
VYTAL_API InputModuleResult input_module_process_mouse_scrolled(const Int8 scroll_value);

VYTAL_API VoidPtr input_module_get_state(void);
VYTAL_API void    input_module_set_state(VoidPtr new_state);