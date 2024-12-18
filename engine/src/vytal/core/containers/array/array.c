#include "array.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#define CONTAINER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB

typedef struct Container_Array_Header {
    ByteSize _data_size;
    ByteSize _length;
    ByteSize _capacity;
    VoidPtr  _memory_block;
} ArrayHeader;

ArrayHeader *_container_array_get_header(Array array) { return !array ? NULL : VT_CAST(ArrayHeader *, array->_internal_data); }

Array __vtimpl_ctnr_darr_construct(const ByteSize data_size) {
    if (data_size == 0)
        return NULL;

    ByteSize bucket_size_ = CONTAINER_MAX_SIZE - (sizeof(Container_Array));

    // size set to 0 since 'containers' use pool allocator, where size is already determined
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);

    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the array container self
    Array arr_ = VT_CAST(Array, chunk_);

    // element 2: the array header
    ArrayHeader *header_ = VT_CAST(ArrayHeader *, arr_ + 1);
    header_->_data_size  = data_size;
    header_->_length     = 0;
    header_->_capacity   = bucket_size_ / data_size;

    // element 3: the array
    header_->_memory_block = VT_CAST(VoidPtr, header_ + 1);

    // assign header to array ownership
    arr_->_internal_data = header_;

    return arr_;
}

Bool container_array_destruct(Array array) {
    if (!array)
        return false;

    // free the entire array container
    {
        memory_manager_deallocate(array, MEMORY_TAG_CONTAINERS);
        array = NULL;
    }

    return true;
}

Bool container_array_pop(Array array) {
    if (!array)
        return false;

    ArrayHeader *header_ = _container_array_get_header(array);
    if (header_->_length == 0)
        return false; // empty;

    --header_->_length;
    return true;
}

Bool __vtimpl_ctrn_darr_setlen(Array array, const ByteSize length) {
    if (!array)
        return false;

    ArrayHeader *header_ = _container_array_get_header(array);
    header_->_length     = length;

    return true;
}

Bool container_array_remove(Array array, VoidPtr data) {
    if (!array)
        return false;

    if (container_array_isempty(array))
        return false;

    ArrayHeader *header_   = _container_array_get_header(array);
    BytePtr      ptr_      = VT_CAST(BytePtr, header_->_memory_block);
    ByteSize     itr_size_ = header_->_data_size;

    for (ByteSize i = 0; i < header_->_length; ++i) {
        if (hal_mem_memcmp(VT_CAST(VoidPtr, ptr_ + (itr_size_ * i)), data, header_->_data_size) == 0) {
            hal_mem_memmove(ptr_ + (itr_size_ * i), ptr_ + (itr_size_ * (i + 1)), itr_size_ * (--header_->_length - i));
            return true;
        }
    }

    return false;
}

Bool container_array_remove_at_index(Array array, const ByteSize index) {
    if (!array)
        return false;

    if (container_array_isempty(array))
        return false;

    ArrayHeader *header_   = _container_array_get_header(array);
    BytePtr      ptr_      = VT_CAST(BytePtr, header_->_memory_block);
    ByteSize     itr_size_ = header_->_data_size;

    hal_mem_memmove(ptr_ + (itr_size_ * index), ptr_ + (itr_size_ * (index + 1)), itr_size_ * (--header_->_length - index));
    return true;
}

Bool container_array_clear(Array array) {
    if (!array)
        return false;

    ArrayHeader *header_ = _container_array_get_header(array);
    header_->_length     = 0;

    return true;
}

ByteSize container_array_length(Array array) { return !array ? 0 : _container_array_get_header(array)->_length; }

ByteSize container_array_capacity(Array array) { return !array ? 0 : _container_array_get_header(array)->_capacity; }

ByteSize container_array_data_size(Array array) { return !array ? 0 : _container_array_get_header(array)->_data_size; }

VoidPtr container_array_get(Array array) { return !array ? NULL : _container_array_get_header(array)->_memory_block; }

VoidPtr container_array_get_at_index(Array array, const ByteSize index) {
    return !array ? NULL
                  : (VT_CAST(BytePtr, _container_array_get_header(array)->_memory_block) +
                     (container_array_data_size(array) * index));
}
