#pragma once

#include "types.h"

// types ---------------------------------------------------------------- //

typedef struct Memory_Zone_Size_Class {
    VoidPtr *_blocks;
    ByteSize _size;
    ByteSize _num_blocks;
    ByteSize _capacity;
} MemoryZoneSizeClass;

typedef struct Memory_Zone {
    ConstStr _name;
    VoidPtr  _start_addr;

    ByteSize _used_memory;
    ByteSize _capacity;

    MemoryZoneSizeClass *_size_classes;
    ByteSize             _num_classes;
} MemoryZone;

typedef struct Memory_Manager {
    MemoryZone *_zones;
    ByteSize    _zone_count;
    ByteSize    _used_memory;
    ByteSize    _capacity;
    VoidPtr     _pool;
} MemoryManager;

// return codes --------------------------------------------------------- //

typedef enum Memory_Manager_Result {
    MEMORY_MANAGER_SUCCESS                   = 0,
    MEMORY_MANAGER_ERROR_NOT_INITIALIZED     = -1,
    MEMORY_MANAGER_ERROR_ALREADY_INITIALIZED = -2,
    MEMORY_MANAGER_ERROR_FILE_OPEN_FAILED    = -3,
    MEMORY_MANAGER_ERROR_FILE_CLOSE_FAILED   = -4,
    MEMORY_MANAGER_ERROR_OUT_OF_MEMORY       = -5,
    MEMORY_MANAGER_ERROR_INVALID_FORMAT      = -6
} MemoryManagerResult;

typedef enum Memory_Zone_Result {
    MEMORY_ZONE_SUCCESS                   = 0,
    MEMORY_ZONE_ERROR_NOT_EXIST           = -1,
    MEMORY_ZONE_ERROR_INVALID_PARAM       = -2,
    MEMORY_ZONE_ERROR_INSUFFICIENT_MEMORY = -3,
    MEMORY_ZONE_ERROR_INVALID_POINTER     = -4,
    MEMORY_ZONE_ERROR_MEMORY_ALLOCATION   = -5
} MemoryZoneResult;

// endian-check --------------------------------------------------------- //

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define IS_LITTLE_ENDIAN 1
#        define IS_BIG_ENDIAN 0
#    elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#        define IS_LITTLE_ENDIAN 0
#        define IS_BIG_ENDIAN 1
#    else
#        error "Unknown endianness detected!"
#    endif

#elif defined(_WIN32) || defined(__LITTLE_ENDIAN__) || defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
#    define IS_LITTLE_ENDIAN 1
#    define IS_BIG_ENDIAN 0

#elif defined(__BIG_ENDIAN__) || defined(__sparc__) || defined(__powerpc__) || defined(__s390__)
#    define IS_LITTLE_ENDIAN 0
#    define IS_BIG_ENDIAN 1

#else
#    include <stdint.h>
VT_INLINE Int32 check_endianness(void) {
    UInt16 test = 0x0001;
    return (*((UInt8 *)&test) == 0x01) ? 1 : 0;
}
#    define IS_LITTLE_ENDIAN check_endianness()
#    define IS_BIG_ENDIAN (!IS_LITTLE_ENDIAN)
#endif

// binary-swap ---------------------------------------------------------- //

#if defined(__GNUC__) || defined(__clang__)
#    define VYTAL_BSWAP32(X) __builtin_bswap32(x)
#    define VYTAL_BSWAP64(x) __builtin_bswap64(x)
#else
#    define VYTAL_BSWAP32(x) ((((x) & 0xff000000U) >> 24) | (((x) & 0x00ff0000U) >> 8) | (((x) & 0x0000ff00U) << 8) | (((x) & 0x000000ffU) << 24))
#    define VYTAL_BSWAP64(x)                                                                                                                                                                                                                           \
        ((((x) & 0xff00000000000000ULL) >> 56) | (((x) & 0x00ff000000000000ULL) >> 40) | (((x) & 0x0000ff0000000000ULL) >> 24) | (((x) & 0x000000ff00000000ULL) >> 8) | (((x) & 0x00000000ff000000ULL) << 8) | (((x) & 0x0000000000ff0000ULL) << 24) | \
         (((x) & 0x000000000000ff00ULL) << 40) | (((x) & 0x00000000000000ffULL) << 56))
#endif

// bit-flag operations -------------------------------------------------- //

#define VYTAL_BITFLAG_FIELD(bit) (1 << bit)
#define VYTAL_BITFLAG_TOGGLE(flags, bitmask) (flags ^= (bitmask))
#define VYTAL_BITFLAG_SET(flags, bitmask) (flags |= (bitmask))
#define VYTAL_BITFLAG_CLEAR(flags, bitmask) (flags &= ~(bitmask))
#define VYTAL_BITFLAG_IF_SET(flags, bitmask) (flags & (bitmask))
#define VYTAL_BITFLAG_IF_ALL_SET(flags, bitmask) ((flags & (bitmask)) == (bitmask))
#define VYTAL_BITFLAG_IF_NOT_SET(flags, bitmask) (!bitflag_if_set(flags, bitmask))

// memory alignment ----------------------------------------------------- //

#define VYTAL_APPLY_ALIGNMENT(size, alignment) (((size + (alignment - 1)) / alignment) * alignment)
