#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

VT_API Bool module_manager_startup_modules(void);
VT_API Bool module_manager_update_modules(const Flt32 delta_time);
VT_API Bool module_manager_shutdown_modules(void);
