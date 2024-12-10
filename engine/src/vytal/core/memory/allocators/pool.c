#include "pool.h"

#include <assert.h>

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"

typedef struct Memory_Allocator_Pool_Node Memory_Allocator_Pool_Node;
typedef Memory_Allocator_Pool_Node       *PoolAllocatorNode;
struct Memory_Allocator_Pool_Node {
    PoolAllocatorNode _next;
};

typedef struct Memory_Allocator_Pool_State {
    ByteSize          _capacity;
    ByteSize          _adjustment;
    PoolAllocatorNode _head;
    VoidPtr           _memory_block;
    ByteSize          _chunk_size;
    ByteSize          _chunk_count;
    ByteSize          _used_count;
} Memory_Allocator_Pool_State;

VT_INLINE Memory_Allocator_Pool_State *_allocator_pool_get_state(PoolAllocator pool) {
    return (!pool ? NULL : VT_CAST(Memory_Allocator_Pool_State *, pool->_internal_state));
}

VoidPtr _allocator_pool_alloc_aligned(const ByteSize size, const ByteSize alignment, ByteSize *get_adjustment) {
    if ((alignment == 0) || (alignment > VT_MEM_ALIGNMENT_MAX))
        return NULL;

    ByteSize ext_size_     = size + alignment;
    VoidPtr  ptr_          = hal_mem_malloc(ext_size_);
    UIntPtr  pnraw_        = VT_CAST(UIntPtr, ptr_);
    UIntPtr  mask_         = alignment - 1;
    UIntPtr  misalignment_ = pnraw_ & mask_;

    PtrDiff adjustment_ = VT_CAST(PtrDiff, alignment - misalignment_);
    *get_adjustment     = adjustment_;

    UIntPtr paligned_ = pnraw_ + adjustment_;
    return VT_CAST(VoidPtr, paligned_);
}

Bool _allocator_pool_free_aligned(VoidPtr ptr, const ByteSize size, const ByteSize adjustment) {
    if (!ptr)
        return false;

    UIntPtr paligned_ = VT_CAST(UIntPtr, ptr);
    UIntPtr pnraw_    = paligned_ - adjustment;

    hal_mem_free(VT_CAST(VoidPtr, pnraw_));
    ptr = NULL;
    return true;
}

VT_INLINE Bool _allocator_pool_address_isvalid(PoolAllocator pool, VoidPtr ptr) {
#define POOL_START (VT_CAST(BytePtr, _allocator_pool_get_state(pool)->_memory_block))
#define POOL_END (VT_CAST(BytePtr, _allocator_pool_get_state(pool)->_memory_block) + _allocator_pool_get_state(pool)->_capacity)
    return (ptr != NULL) && (pool != NULL) && (VT_CAST(BytePtr, ptr) >= POOL_START) && (VT_CAST(BytePtr, ptr) < POOL_END);
}

PoolAllocator allocator_pool_construct_aligned(const ByteSize capacity, const ByteSize chunk_count, const ByteSize alignment) {
    if ((capacity == 0) || (chunk_count == 0) || !((alignment & (alignment - 1)) == 0))
        return NULL;

    PoolAllocator                pool_       = hal_mem_malloc(sizeof(Memory_Allocator_Pool));
    Memory_Allocator_Pool_State *state_      = hal_mem_malloc(sizeof(Memory_Allocator_Pool_State));
    ByteSize                     chunk_size_ = capacity / chunk_count;

    // ensure capacity is a multiple of chunk size
    if (capacity % chunk_size_ != 0)
        return NULL;

    // init state members
    {
        state_->_capacity     = capacity;
        state_->_memory_block = _allocator_pool_alloc_aligned(capacity, alignment, &(state_->_adjustment));
        state_->_chunk_size   = chunk_size_;
        state_->_chunk_count  = chunk_count;
        state_->_head         = VT_CAST(PoolAllocatorNode, state_->_memory_block);
        state_->_used_count   = 0;

        PoolAllocatorNode curr_node_ = state_->_head;
        for (ByteSize i = 0; i < chunk_count - 1; ++i) {
            PoolAllocatorNode next_node_ = VT_CAST(PoolAllocatorNode, VT_CAST(UIntPtr, curr_node_) + chunk_size_);
            curr_node_->_next            = next_node_;

            // move to next node for offset
            curr_node_ = next_node_;
        }

        // terminate the free list
        curr_node_->_next = NULL;
    }

    pool_->_internal_state = state_;
    return pool_;
}

Bool allocator_pool_destruct(PoolAllocator pool) {
    if (!pool)
        return false;

    Memory_Allocator_Pool_State *state_ = _allocator_pool_get_state(pool);

    // free the state and its members
    {
        if (!_allocator_pool_free_aligned(state_->_memory_block, state_->_chunk_size * state_->_chunk_count,
                                          state_->_adjustment)) {
            return false;
        }

        hal_mem_memzero(state_, sizeof(Memory_Allocator_Pool_State));
        hal_mem_free(state_);
    }

    // free the allocator self
    {
        pool->_internal_state = NULL;
        hal_mem_free(pool);
    }

    pool = NULL;
    return true;
}

VoidPtr allocator_pool_allocate(PoolAllocator pool) {
    if (!pool)
        return NULL;

    Memory_Allocator_Pool_State *state_ = _allocator_pool_get_state(pool);

    PoolAllocatorNode node_ = state_->_head;
    state_->_head           = state_->_head->_next;

    state_->_used_count++;
    return hal_mem_memzero(node_, state_->_chunk_size);
}

Bool allocator_pool_deallocate(PoolAllocator pool, VoidPtr ptr) {
    if (!pool || !ptr || !_allocator_pool_address_isvalid(pool, ptr))
        return false;

    Memory_Allocator_Pool_State *state_ = _allocator_pool_get_state(pool);
    PoolAllocatorNode            node_  = VT_CAST(PoolAllocatorNode, ptr);

    // push free node to the list
    {
        node_->_next  = state_->_head;
        state_->_head = node_;
    }

    state_->_used_count--;
    return true;
}

Bool allocator_pool_deallocate_all(PoolAllocator pool) {
    if (!pool)
        return false;

    Memory_Allocator_Pool_State *state_ = _allocator_pool_get_state(pool);

    for (ByteSize i = 0; i < state_->_chunk_count - 1; ++i) {
        BytePtr           ptr_  = VT_CAST(BytePtr, state_->_memory_block) + (i * state_->_chunk_size);
        PoolAllocatorNode node_ = VT_CAST(PoolAllocatorNode, ptr_);

        // push free node to the list
        {
            node_->_next  = state_->_head;
            state_->_head = node_;
        }
    }

    return true;
}

VoidPtr allocator_pool_head(PoolAllocator pool) { return (!pool ? NULL : _allocator_pool_get_state(pool)->_head); }

ByteSize allocator_pool_usedmem(PoolAllocator pool) {
    return (!pool ? 0 : (_allocator_pool_get_state(pool)->_used_count * _allocator_pool_get_state(pool)->_chunk_size));
}

ByteSize allocator_pool_capacity(PoolAllocator pool) { return (!pool ? 0 : _allocator_pool_get_state(pool)->_capacity); }

ByteSize allocator_pool_chunksize(PoolAllocator pool) { return (!pool ? 0 : _allocator_pool_get_state(pool)->_chunk_size); }

ByteSize allocator_pool_chunkcount(PoolAllocator pool) { return (!pool ? 0 : _allocator_pool_get_state(pool)->_chunk_count); }
