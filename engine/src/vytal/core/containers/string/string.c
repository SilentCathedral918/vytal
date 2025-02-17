#include "string.h"

#include <string.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Container_String {
    Str      _data;
    ByteSize _size;
    ByteSize _capacity;
};

VYTAL_INLINE ByteSize _container_string_apply_alignment(const ByteSize size, const ByteSize alignment) {
    return ((size + (alignment - 1)) / alignment) * alignment;
}

ContainerResult container_string_construct(ConstStr content, String *out_new_str) {
    if (!content || !out_new_str) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize content_length_ = strlen(content);
    ByteSize capacity_       = _container_string_apply_alignment(content_length_ + 1, MEMORY_ALIGNMENT_SIZE) * 2;
    ByteSize alloc_size_     = sizeof(struct Container_String) + capacity_;

    if (memory_zone_allocate("Strings", alloc_size_, (VoidPtr *)out_new_str) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_str)->_size     = content_length_;
    (*out_new_str)->_capacity = capacity_;
    (*out_new_str)->_data     = (Str)((UIntPtr)(*out_new_str) + sizeof(struct Container_String));

    memcpy((*out_new_str)->_data, content, content_length_ + 1);

    return CONTAINER_SUCCESS;
}

ContainerResult container_string_destruct(String str) {
    if (!str) return CONTAINER_ERROR_INVALID_PARAM;
    if (!str->_data || !str->_capacity) return CONTAINER_ERROR_NOT_ALLOCATED;

    memset(str, 0, sizeof(struct Container_String));
    return CONTAINER_SUCCESS;
}

VYTAL_API Str container_string_get(String str) {
    return (!str) ? NULL : str->_data;
}

VYTAL_API ByteSize container_string_size(String str) {
    return (!str) ? 0 : str->_size;
}

VYTAL_API ByteSize container_string_capacity(String str) {
    return (!str) ? 0 : str->_capacity;
}
