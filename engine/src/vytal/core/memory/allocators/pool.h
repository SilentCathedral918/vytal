#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API PoolAllocator allocator_pool_construct_aligned(const ByteSize capacity, const ByteSize chunk_count, const ByteSize alignment);
VT_API Bool          allocator_pool_destruct(PoolAllocator pool);
VT_API VoidPtr       allocator_pool_allocate(PoolAllocator pool);
VT_API Bool          allocator_pool_deallocate(PoolAllocator pool, VoidPtr ptr);
VT_API Bool          allocator_pool_deallocate_all(PoolAllocator pool);
VT_API VoidPtr       allocator_pool_head(PoolAllocator pool);
VT_API ByteSize      allocator_pool_usedmem(PoolAllocator pool);
VT_API ByteSize      allocator_pool_capacity(PoolAllocator pool);
VT_API ByteSize      allocator_pool_chunksize(PoolAllocator pool);
VT_API ByteSize      allocator_pool_chunkcount(PoolAllocator pool);

VT_API VT_INLINE PoolAllocator allocator_pool_construct(const ByteSize capacity, const ByteSize chunk_count) { return allocator_pool_construct_aligned(capacity, chunk_count, VT_MEM_DEFAULT_ALIGNMENT); }
VT_API VT_INLINE ConstStr      allocator_pool_getname(void) { return "Pool"; }
