#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API void    memory_manager_startup(MemoryManager *manager);
VT_API void    memory_manager_shutdown(MemoryManager *manager);
VT_API VoidPtr memory_manager_allocate(MemoryManager *manager, const ByteSize size, const MemoryTag tag);
VT_API VoidPtr memory_manager_deallocate(MemoryManager *manager, const ByteSize size, const MemoryTag tag);
VT_API VoidPtr memory_manager_construct_allocator(MemoryManager *manager, ConstStr id, const ByteSize capacity,
                                                  const MemMgrAllocType type);
VT_API Bool    memory_manager_destruct_allocator(MemoryManager *manager, ConstStr id);
VT_API Bool    memory_manager_report(const MemoryManager *manager, ConstStr id);
VT_API void    memory_manager_report_all(const MemoryManager *manager);
