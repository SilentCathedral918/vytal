#include "arena.h"

#include "vytal/core/hal/memory/vtmem.h"

VT_INLINE VoidPtr _allocator_area_alignforward(VoidPtr block, ByteSize alignment) {
#define MISALIGNMENT (VT_CAST(UIntPtr, block) & (alignment - 1))
    return VT_CAST(VoidPtr, VT_CAST(UIntPtr, block) + ((MISALIGNMENT != 0) ? (alignment - MISALIGNMENT) : 0));
}

ArenaAllocator allocator_arena_construct(const ByteSize capacity) {
    ArenaAllocator arena_ = hal_mem_malloc(sizeof(Memory_Allocator_Arena));

    // init members
    {
        arena_->_used_mem  = 0;
        arena_->_num_alloc = 0;
        arena_->_capacity  = capacity;
        arena_->_mem_block = hal_mem_malloc(capacity);
    }

    return arena_;
}

void allocator_arena_destruct(ArenaAllocator arena) {
    // free members and set all to zero
    {
        free(arena->_mem_block);
        hal_mem_memzero(arena, sizeof(Memory_Allocator_Arena));
    }

    // free allocator self
    hal_mem_free(arena);
}

VoidPtr allocator_arena_allocate_aligned(ArenaAllocator arena, const ByteSize size, const ByteSize alignment) {
    if ((size == 0) || (alignment == 0) || !((alignment & (alignment - 1)) == 0))
        return NULL;

    BytePtr  pcurr_, paligned_;
    ByteSize adjustment_;

    // align the current offset forward to specified alignment
    {
        pcurr_    = VT_CAST(BytePtr, arena->_mem_block) + arena->_used_mem;
        paligned_ = _allocator_area_alignforward(pcurr_, alignment);

        adjustment_ = VT_CAST(PtrDiff, paligned_ - pcurr_);
    }

    // check for potential of overflow
    if ((arena->_used_mem + adjustment_ + size) > arena->_capacity)
        return NULL;

    // update members
    {
        arena->_used_mem += size + adjustment_;
        arena->_num_alloc++;
    }

    return hal_mem_memzero(paligned_, size);
}

void allocator_arena_clear(ArenaAllocator arena) {
    // set all members to zero
    hal_mem_memzero(arena->_mem_block, arena->_used_mem);
    arena->_used_mem  = 0;
    arena->_num_alloc = 0;
}
