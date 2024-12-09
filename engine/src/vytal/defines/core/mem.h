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
    VoidPtr _internal_state;
} Memory_Allocator_Arena;
typedef Memory_Allocator_Arena *ArenaAllocator;

// ----------------------------- manager ----------------------------- //

// --- self struct
// NOTE: there can only be one

typedef struct Memory_Manager_Struct {
    VoidPtr _internal_state;
} MemoryManager;

// --- allocation types

typedef enum Memory_Manager_AllocationType { ALLOCTYPE_ARENA } MemMgrAllocType;

// --- memory allocation tags

typedef enum Memory_Tag {
    MEMORY_TAG_APPLICATION, // general application
    MEMORY_TAG_PHYSICS,     // physics engine
    MEMORY_TAG_AI,          // artificial intelligence
    MEMORY_TAG_AUDIO,       // audio system allocations
    MEMORY_TAG_NETWORK,     // networking

    MEMORY_TAG_GAME_LOGIC,  // core game logic
    MEMORY_TAG_GAME_ENTITY, // game entities (players, npcs, etc.)
    MEMORY_TAG_GAME_SCENE,  // scene management

    MEMORY_TAG_CONTAINERS, // data structures (arrays, lists, maps, etc.)
    MEMORY_TAG_DELEGATES,  // delegates, callbacks, event handlers

    MEMORY_TAG_RENDERER,  // rendering engine
    MEMORY_TAG_RESOURCES, // game resources (textures, models, sounds, etc.)

    MEMORY_TAGS_TOTAL
} MemoryTag;

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

// ------------------------- sizes in bytes ------------------------- //

#define VT_SIZE_KB_IN_BYTES (1024ul)
#define VT_SIZE_MB_IN_BYTES (VT_SIZE_KB_IN_BYTES * 1024ul)
#define VT_SIZE_GB_IN_BYTES (VT_SIZE_MB_IN_BYTES * 1024ul)
