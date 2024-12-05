#pragma once

#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

#include <stdlib.h>
#include <string.h>

VT_API VT_INLINE VoidPtr hal_mem_malloc(ByteSize size) { return malloc(size); }
VT_API VT_INLINE void    hal_mem_free(VoidPtr ptr) { return free(ptr); }
VT_API VT_INLINE VoidPtr hal_mem_calloc(ByteSize count, ByteSize size) { return calloc(count, size); }
VT_API VT_INLINE VoidPtr hal_mem_realloc(VoidPtr ptr, ByteSize new_size) { return realloc(ptr, new_size); }
VT_API VT_INLINE VoidPtr hal_mem_memset(VoidPtr ptr, Int32 value, ByteSize size) { return memset(ptr, value, size); }
VT_API VT_INLINE VoidPtr hal_mem_memcpy(VoidPtr dest, VoidPtr src, ByteSize size) { return memcpy(dest, src, size); }
VT_API VT_INLINE VoidPtr hal_mem_memmove(VoidPtr dest, VoidPtr src, ByteSize size) { return memmove(dest, src, size); }
VT_API VT_INLINE VoidPtr hal_mem_memzero(VoidPtr ptr, ByteSize size) { return memset(ptr, 0, size); }
VT_API VT_INLINE Int32   hal_mem_memcmp(VoidPtr left, VoidPtr right, ByteSize size) { return memcmp(left, right, size); }
VT_API void              hal_mem_memswap(VoidPtr left, VoidPtr right, ByteSize size);