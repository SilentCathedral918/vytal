#pragma once

#include "vytal/defines/core/filesystem.h"
#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API MemoryManagerResult memory_manager_startup(File *file);
VYTAL_API MemoryManagerResult memory_manager_shutdown(void);

VYTAL_API MemoryManager *memory_manager_get(void);
VYTAL_API ByteSize       memory_manager_used_memory(void);
VYTAL_API ByteSize       memory_manager_capacity(void);
VYTAL_API MemoryZone    *memory_manager_zones(void);
VYTAL_API ByteSize       memory_manager_zone_count(void);
