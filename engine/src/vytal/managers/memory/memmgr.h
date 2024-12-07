#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API void    memory_manager_startup(MemoryManager *manager);
VT_API void    memory_manager_shutdown(MemoryManager *manager);
VT_API VoidPtr memory_manager_allocate(MemoryManager *manager, const ByteSize memory_size, const MemMgrAllocType alloc_type);
VT_API VoidPtr memory_manager_deallocate(MemoryManager *manager, const MemMgrAllocType alloc_type);
VT_API void    memory_manager_clear(MemoryManager *manager, const MemMgrAllocType alloc_type);
VT_API void    memory_manager_report(const MemoryManager *manager);