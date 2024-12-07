#pragma once

#include "types.h"

// ----------------------------- memory alignment ----------------------------- //

#if !defined(VT_MEM_DEFAULT_ALIGNMENT)
#    define VT_MEM_DEFAULT_ALIGNMENT (sizeof(VoidPtr))
#endif

#if !defined(VT_MEM_ALIGNMENT_MAX)
#    define VT_MEM_ALIGNMENT_MAX 128
#endif

// ----------------------------- allocators ----------------------------- //

// --- arena allocator

typedef struct Memory_Allocator_Arena {
    UInt64  _used_mem;
    UInt64  _capacity;
    UInt64  _num_alloc;
    VoidPtr _mem_block;
} Memory_Allocator_Arena;
typedef Memory_Allocator_Arena *ArenaAllocator;

// ----------------------------- manager ----------------------------- //

typedef enum Memory_Manager_AllocationType { ALLOCTYPE_ARENA } MemMgrAllocType;

typedef struct Memory_Manager_Struct {
    ArenaAllocator _arena;
    UInt64         _internal_mem_use;
} MemoryManager;

// --------------------------- binary swapping --------------------------- //

#if defined(__clang__) || defined(__GNUC__)
#    define VT_BSWAP32(X) __builtin_bswap32(x)
#    define VT_BSWAP64(x) __builtin_bswap64(x)

#else
#    define VT_BSWAP32(x)                                                                                                      \
        ((((x) & 0xff000000U) >> 24) | (((x) & 0x00ff0000U) >> 8) | (((x) & 0x0000ff00U) << 8) | (((x) & 0x000000ffU) << 24))

#    define VT_BSWAP64(x)                                                                                                      \
        ((((x) & 0xff00000000000000ULL) >> 56) | (((x) & 0x00ff000000000000ULL) >> 40) |                                       \
         (((x) & 0x0000ff0000000000ULL) >> 24) | (((x) & 0x000000ff00000000ULL) >> 8) | (((x) & 0x00000000ff000000ULL) << 8) | \
         (((x) & 0x0000000000ff0000ULL) << 24) | (((x) & 0x000000000000ff00ULL) << 40) |                                       \
         (((x) & 0x00000000000000ffULL) << 56))
#endif

// ------------------------- bit-flag operations ------------------------- //

#define VT_BITFLAG_FIELD(bit) (1 << bit)
#define VT_BITFLAG_TOGGLE(flags, bitmask) (flags ^= (bitmask))
#define VT_BITFLAG_SET(flags, bitmask) (flags |= (bitmask))
#define VT_BITFLAG_CLEAR(flags, bitmask) (flags &= ~(bitmask))
#define VT_BITFLAG_IF_SET(flags, bitmask) (flags & (bitmask))
#define VT_BITFLAG_IF_ALL_SET(flags, bitmask) ((flags & (bitmask)) == (bitmask))
#define VT_BITFLAG_IF_NOT_SET(flags, bitmask) (!bitflag_if_set(flags, bitmask))

// ------------------------- size multipliers ------------------------- //

#define VT_SIZE_KB_MULT(byte) (byte * 1024ul)
#define VT_SIZE_MB_MULT(byte) (VT_SIZE_KB_MULT(byte) * 1024ul)
#define VT_SIZE_GB_MULT(byte) (VT_SIZE_MB_MULT(byte) * 1024ul)
