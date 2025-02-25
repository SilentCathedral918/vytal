#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/core/window.h"
#include "vytal/defines/shared.h"

VYTAL_API WindowModuleResult window_module_startup(void);
VYTAL_API WindowModuleResult window_module_shutdown(void);
VYTAL_API WindowModuleResult window_module_update(void);

VYTAL_API WindowModuleResult window_module_construct_main(void);
VYTAL_API WindowModuleResult window_module_destruct_main(void);

VYTAL_API Window           window_module_get_main(void);
VYTAL_API WindowProperties window_module_get_properties(void);