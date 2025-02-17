#pragma once

#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API MemoryManagerResult memory_manager_startup(void);
VYTAL_API MemoryManagerResult memory_manager_shutdown(void);

VYTAL_API MemoryManager *memory_manager_get(void);
VYTAL_API ConstStr       memory_manager_get_zones_cvar_filepath(void);
