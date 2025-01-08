#pragma once

#include "vytal/defines/core/container.h"
#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API Array __vtimpl_ctnr_darr_construct(const ByteSize data_size);
VT_API Array __vtimpl_ctnr_darr_construct_custom(const ByteSize data_size, const VoidPtr allocator,
                                                 const MemMgrAllocType alloc_type, const ByteSize capacity);
VT_API Bool  container_array_destruct(Array array);
VT_API Bool  container_array_pop(Array array);
VT_API Bool  __vtimpl_ctrn_darr_setlen(Array array, const ByteSize length);
VT_API Bool  container_array_remove(Array array, VoidPtr data);
VT_API Bool  container_array_remove_at_index(Array array, const ByteSize index);
VT_API Bool  container_array_clear(Array array);
VT_API Bool  container_array_sort(Array array, Int32 (*compare)(const void *left, const void *right));

VT_API ByteSize       container_array_length(Array array);
VT_API ByteSize       container_array_capacity(Array array);
VT_API ByteSize       container_array_data_size(Array array);
VT_API VoidPtr        container_array_get(Array array);
VT_API VoidPtr        container_array_get_at_index(Array array, const ByteSize index);
VT_API VT_INLINE Bool container_array_isempty(Array array) { return (container_array_length(array) == 0); }
VT_API VT_INLINE Bool container_array_isfull(Array array) {
    return (container_array_length(array) == container_array_capacity(array));
}

#define container_array_construct(type) __vtimpl_ctnr_darr_construct(sizeof(type))
#define container_array_construct_custom(type, allocator, alloc_type, capacity)                                                \
    __vtimpl_ctnr_darr_construct_custom(sizeof(type), allocator, alloc_type, capacity)
#define container_array_push(array, type, ...)                                                                                 \
    {                                                                                                                          \
        if (array) {                                                                                                           \
            if (!container_array_isfull(array) && (sizeof(type) == container_array_data_size(array))) {                        \
                *(VT_CAST(type *, container_array_get(array)) + container_array_length(array)) = __VA_ARGS__;                  \
                __vtimpl_ctrn_darr_setlen(array, container_array_length(array) + 1);                                           \
            }                                                                                                                  \
        }                                                                                                                      \
    }

#define container_array_get_value_at_index(array, type, index) (*VT_CAST(type *, container_array_get_at_index(array, index)))
