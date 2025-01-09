#include "arena.h"

#include "vytal/core/hal/memory/vtmem.h"

typedef struct Memory_Allocator_Arena_State {
    UInt64  _used_mem;
    UInt64  _capacity;
    UInt64  _num_alloc;
    VoidPtr _mem_block;
} Memory_Allocator_Arena_State;

VT_INLINE VoidPtr _allocator_arena_alignforward(VoidPtr block, ByteSize alignment) {
#define MISALIGNMENT (VT_CAST(UIntPtr, block) & (alignment - 1))
    return VT_CAST(VoidPtr, VT_CAST(UIntPtr, block) + ((MISALIGNMENT != 0) ? (alignment - MISALIGNMENT) : 0));
}

VT_INLINE Memory_Allocator_Arena_State *_allocator_arena_get_state(ArenaAllocator arena) { return (!arena ? NULL : VT_CAST(Memory_Allocator_Arena_State *, arena->_internal_state)); }

ArenaAllocator allocator_arena_construct(const ByteSize capacity) {
    ArenaAllocator                arena_ = hal_mem_malloc(sizeof(Memory_Allocator_Arena));
    Memory_Allocator_Arena_State *state_ = hal_mem_malloc(sizeof(Memory_Allocator_Arena_State));

    // init state members
    {
        state_->_used_mem  = 0;
        state_->_num_alloc = 0;
        state_->_capacity  = capacity;
        state_->_mem_block = hal_mem_malloc(capacity);
    }

    arena_->_internal_state = state_;
    return arena_;
}

void allocator_arena_destruct(ArenaAllocator arena) {
    if (!arena)
        return;

    Memory_Allocator_Arena_State *state_ = VT_CAST(Memory_Allocator_Arena_State *, arena->_internal_state);

    // free state members and set all to zero
    {
        hal_mem_free(state_->_mem_block);
        hal_mem_memzero(state_, sizeof(Memory_Allocator_Arena));
    }

    // free allocator state and self
    hal_mem_free(state_);
    arena->_internal_state = NULL;

    hal_mem_free(arena);
    arena = NULL;
}

VoidPtr allocator_arena_allocate_aligned(ArenaAllocator arena, const ByteSize size, const ByteSize alignment) {
    if ((size == 0) || (alignment == 0) || !((alignment & (alignment - 1)) == 0))
        return NULL;

    if (!arena->_internal_state)
        return NULL;

    Memory_Allocator_Arena_State *state_ = _allocator_arena_get_state(arena);
    BytePtr                       pcurr_, paligned_;
    ByteSize                      adjustment_;

    // align the current offset forward to specified alignment
    {
        pcurr_    = VT_CAST(BytePtr, state_->_mem_block) + state_->_used_mem;
        paligned_ = _allocator_arena_alignforward(pcurr_, alignment);

        adjustment_ = VT_CAST(PtrDiff, paligned_ - pcurr_);
    }

    // check for potential of overflow
    if ((state_->_used_mem + adjustment_ + size) > state_->_capacity)
        return NULL;

    // update members
    {
        state_->_used_mem += size + adjustment_;
        state_->_num_alloc++;
    }

    return hal_mem_memzero(paligned_, size);
}

void allocator_arena_clear(ArenaAllocator arena) {
    if (!arena->_internal_state)
        return;

    Memory_Allocator_Arena_State *state_ = _allocator_arena_get_state(arena);

    // set all members to zero
    hal_mem_memzero(state_->_mem_block, state_->_used_mem);
    state_->_used_mem  = 0;
    state_->_num_alloc = 0;
}

VoidPtr allocator_arena_head(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_mem_block); }
VoidPtr allocator_arena_tail(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_mem_block + _allocator_arena_get_state(arena)->_capacity); }
VoidPtr allocator_arena_current(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_mem_block + _allocator_arena_get_state(arena)->_used_mem); }
UInt64  allocator_arena_allocscount(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_num_alloc); }
UInt64  allocator_arena_usedmem(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_used_mem); }
UInt64  allocator_arena_capacity(ArenaAllocator arena) { return (_allocator_arena_get_state(arena)->_capacity); }
