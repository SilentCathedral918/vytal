#include "memmgr.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/memory/allocators/arena.h"

#define not_enough_mem(target, mem_size) ((target->_used_mem + mem_size) > target->_capacity)

void memory_manager_startup(MemoryManager *manager) {
    // capacity for each allocator: 128MB
    ByteSize capacity_ = VT_SIZE_MB_MULT(128);

    // construct allocators
    manager->_arena = allocator_arena_construct(capacity_);
    manager->_internal_mem_use += sizeof(Memory_Allocator_Arena) + capacity_;
}

void memory_manager_shutdown(MemoryManager *manager) {
    // destruct allocators
    allocator_arena_destruct(manager->_arena);

    // reset memory usage
    manager->_internal_mem_use = 0;
}

VoidPtr memory_manager_allocate(MemoryManager *manager, const ByteSize memory_size, const MemMgrAllocType alloc_type) {
    switch (alloc_type) {
    // --- arena
    case ALLOCTYPE_ARENA:
        if (not_enough_mem(manager->_arena, memory_size))
            return NULL;

        return allocator_arena_allocate(manager->_arena, memory_size);

    default:
        break;
    }

    return NULL;
}

VoidPtr memory_manager_deallocate(MemoryManager *manager, const MemMgrAllocType alloc_type) {
    switch (alloc_type) {
    // --- arena
    case ALLOCTYPE_ARENA:
        // no deallocation in arena allocator
        return (VT_CAST(BytePtr, manager->_arena->_mem_block) + manager->_arena->_used_mem);

    default:
        break;
    }

    return NULL;
}

void memory_manager_clear(MemoryManager *manager, const MemMgrAllocType alloc_type) {
    switch (alloc_type) {
    // --- arena
    case ALLOCTYPE_ARENA:
        allocator_arena_clear(manager->_arena);
        break;

    default:
        break;
    }
}

void memory_manager_report(const MemoryManager *manager) {
    // will work on reporting memory use...
}
