#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API void memory_manager_startup(MemoryManager *manager);
VT_API void memory_manager_shutdown(MemoryManager *manager);
VT_API void memory_manager_report(const MemoryManager *manager);


