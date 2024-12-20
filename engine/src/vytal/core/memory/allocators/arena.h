#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API ArenaAllocator allocator_arena_construct(const ByteSize capacity);
VT_API void           allocator_arena_destruct(ArenaAllocator arena);
VT_API VoidPtr        allocator_arena_allocate_aligned(ArenaAllocator arena, const ByteSize size, const ByteSize alignment);
VT_API void           allocator_arena_clear(ArenaAllocator arena);
VT_API VoidPtr        allocator_arena_head(ArenaAllocator arena);
VT_API VoidPtr        allocator_arena_tail(ArenaAllocator arena);
VT_API VoidPtr        allocator_arena_current(ArenaAllocator arena);
VT_API UInt64         allocator_arena_allocscount(ArenaAllocator arena);
VT_API UInt64         allocator_arena_usedmem(ArenaAllocator arena);
VT_API UInt64         allocator_arena_capacity(ArenaAllocator arena);

VT_API VT_INLINE VoidPtr allocator_arena_allocate(ArenaAllocator arena, const ByteSize size) {
    return allocator_arena_allocate_aligned(arena, size, VT_MEM_DEFAULT_ALIGNMENT);
}
VT_API VT_INLINE ConstStr allocator_arena_getname(void) { return "Arena"; }
