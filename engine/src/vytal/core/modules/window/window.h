#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/core/types.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VYTAL_API WindowModuleResult window_module_startup(File *file);
VYTAL_API WindowModuleResult window_module_shutdown(void);
VYTAL_API WindowModuleResult window_module_update(void);

VYTAL_API WindowModuleResult window_module_construct_window(Window *out_new_window);
VYTAL_API WindowModuleResult window_module_destruct_window(Window window);

VYTAL_API WindowProperties window_module_default_properties(void);