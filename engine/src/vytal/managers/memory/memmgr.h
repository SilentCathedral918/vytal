#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

static VT_VAR_NOT_USED struct Memory_Manager_Struct {
    VoidPtr _internal_state;
} memory_manager;

VT_API void    memory_manager_startup(void);
VT_API void    memory_manager_shutdown(void);
VT_API VoidPtr memory_manager_allocate(const ByteSize size, const MemoryTag tag);
VT_API Bool    memory_manager_deallocate(VoidPtr ptr, const MemoryTag tag);
VT_API Bool    memory_manager_report(ConstStr id);
VT_API void    memory_manager_report_all(void);

VT_API VoidPtr memory_manager_construct_allocator_arena(ConstStr id, const ByteSize capacity);
VT_API VoidPtr memory_manager_construct_allocator_pool(ConstStr id, const ByteSize capacity, const ByteSize chunk_count);
VT_API Bool    memory_manager_destruct_allocator(ConstStr id);
