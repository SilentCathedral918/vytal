#include "array.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/memory/zone/memory_zone.h"

struct Container_Array {
    VoidPtr  _pool;
    ByteSize _data_size;
    ByteSize _size;
    ByteSize _capacity;

    // used for allocations/deallocations
    ByteSize _memory_size;
};

ContainerResult _container_array_resize(Array *array, const ByteSize new_capacity) {
    Array old_array_ = *array;
    Array new_array_ = NULL;

    ByteSize base_new_alloc_size_ = old_array_->_memory_size * CONTAINER_RESIZE_FACTOR;
    ByteSize new_alloc_size_      = 0;

    if (memory_zone_allocate("containers", base_new_alloc_size_, (VoidPtr *)&new_array_, &new_alloc_size_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;
    memset(new_array_, 0, new_alloc_size_);

    new_array_->_data_size   = old_array_->_data_size;
    new_array_->_size        = old_array_->_size;
    new_array_->_capacity    = new_capacity;
    new_array_->_memory_size = new_alloc_size_;
    new_array_->_pool        = (VoidPtr)((UIntPtr)new_array_ + sizeof(struct Container_Array));

    memmove(new_array_->_pool, old_array_->_pool, old_array_->_data_size * old_array_->_size);

    if (memory_zone_deallocate("containers", old_array_, old_array_->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    *array = new_array_;
    return CONTAINER_SUCCESS;
}

ContainerResult container_array_construct(const ByteSize data_size, Array *out_new_array) {
    if (!data_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize pool_size_  = data_size * CONTAINER_DEFAULT_CAPACITY;
    ByteSize alloc_size_ = VYTAL_APPLY_ALIGNMENT(sizeof(struct Container_Array) + pool_size_, MEMORY_ALIGNMENT_SIZE);

    if (memory_zone_allocate("containers", alloc_size_, (VoidPtr *)out_new_array, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;
    memset(*out_new_array, 0, alloc_size_);

    (*out_new_array)->_data_size   = data_size;
    (*out_new_array)->_capacity    = CONTAINER_DEFAULT_CAPACITY;
    (*out_new_array)->_size        = 0;
    (*out_new_array)->_memory_size = alloc_size_;
    (*out_new_array)->_pool        = (VoidPtr)((UIntPtr)(*out_new_array) + sizeof(struct Container_Array));
    memset((*out_new_array)->_pool, 0, pool_size_);

    return CONTAINER_SUCCESS;
}

ContainerResult container_array_destruct(Array array) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!array->_pool || !array->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    if (memory_zone_deallocate("containers", array, array->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    array = NULL;
    return CONTAINER_SUCCESS;
}

ContainerResult container_array_push(Array *array, VoidPtr new_data) {
    if (!array || !new_data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    // handle container resizing (if array is full)
    if ((*array)->_size == (*array)->_capacity) {
        ByteSize new_capacity_ = (*array)->_capacity * CONTAINER_RESIZE_FACTOR;

        ContainerResult resize_ = _container_array_resize(array, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    ByteSize push_offset_ = (*array)->_data_size * (*array)->_size;
    UIntPtr  push_addr_   = (UIntPtr)(*array)->_pool + push_offset_;

    memcpy((VoidPtr)push_addr_, new_data, (*array)->_data_size);
    ++(*array)->_size;

    return CONTAINER_SUCCESS;
}

ContainerResult container_array_pop(Array *array) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*array)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    --(*array)->_size;
    return CONTAINER_SUCCESS;
}

ContainerResult container_array_insert(Array *array, const ByteSize index, VoidPtr new_data) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!new_data || (index < 0) || (index > (*array)->_size)) return CONTAINER_ERROR_INVALID_PARAM;
    if (index == (*array)->_size) return container_array_push(array, new_data);

    // handle container resizing (if array is full)
    if ((*array)->_size == (*array)->_capacity) {
        ByteSize new_capacity_ = (*array)->_capacity * CONTAINER_RESIZE_FACTOR;

        ContainerResult resize_ = _container_array_resize(array, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    // move the trailing train right in data_size bytes
    {
        UIntPtr shift_src_  = (UIntPtr)((*array)->_pool) + ((*array)->_data_size * index);
        UIntPtr shift_dest_ = shift_src_ + (*array)->_data_size;
        memmove((VoidPtr)shift_dest_, (VoidPtr)shift_src_, (*array)->_data_size * ((*array)->_size - index));
    }

    // then slot in the new data
    {
        memcpy(
            (VoidPtr)((BytePtr)((*array)->_pool) + ((*array)->_data_size * index)),
            new_data,
            (*array)->_data_size);
        ++(*array)->_size;
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_array_remove(Array *array, const VoidPtr data, const Bool remove_all) {
    if (!array || !data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*array)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    for (ByteSize i = 0; i < (*array)->_size; ++i) {
        VoidPtr elem_ = (*array)->_pool + ((*array)->_data_size * i);

        if (!memcmp(elem_, data, (*array)->_data_size)) {
            // found element with matching data...

            // move the trailing train left in data_size bytes
            // set now-inactive region to 0
            {
                UIntPtr shift_src_  = (UIntPtr)((*array)->_pool) + ((*array)->_data_size * (i + 1));
                UIntPtr shift_dest_ = shift_src_ - (*array)->_data_size;
                memmove((VoidPtr)shift_dest_, (VoidPtr)shift_src_, (*array)->_data_size * ((*array)->_size - i));

                memset((VoidPtr)((BytePtr)(*array)->_pool + ((*array)->_data_size * ((*array)->_size - 1))), 0, (*array)->_data_size);
            }

            // update array size and proceed
            {
                --(*array)->_size;

                if (remove_all)  // remove all the matching elements
                    continue;
                else  // remove only the first found element
                    break;
            }
        }
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_array_remove_at_index(Array *array, const ByteSize index) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*array)->_size) return CONTAINER_ERROR_EMPTY_DATA;
    if ((index < 0) || (index >= (*array)->_size)) return CONTAINER_ERROR_INVALID_PARAM;

    // move the trailing train left in data_size bytes
    {
        UIntPtr shift_src_  = (UIntPtr)((*array)->_pool) + ((*array)->_data_size * (index + 1));
        UIntPtr shift_dest_ = shift_src_ - (*array)->_data_size;
        memmove((VoidPtr)shift_dest_, (VoidPtr)shift_src_, (*array)->_data_size * ((*array)->_size - index));
    }

    // set now-inactive region to 0 and update array size
    {
        memset((VoidPtr)((BytePtr)(*array)->_pool + ((*array)->_data_size * ((*array)->_size - 1))), 0, (*array)->_data_size);
        --(*array)->_size;
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_array_clear(Array *array) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*array)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    (*array)->_size = 0;
    return CONTAINER_SUCCESS;
}

ContainerResult container_array_sort(Array *array, Int32 (*compare)(const void *left, const void *right)) {
    if (!array) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*array)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;
    if (!(*array)->_size) return CONTAINER_ERROR_EMPTY_DATA;

    if ((*array)->_size > 1)
        qsort((*array)->_pool, (*array)->_size, (*array)->_data_size, compare);

    return CONTAINER_SUCCESS;
}

ByteSize container_array_data_size(Array array) {
    return (!array) ? 0 : array->_data_size;
}

ByteSize container_array_size(Array array) {
    return (!array) ? 0 : array->_size;
}

ByteSize container_array_capacity(Array array) {
    return (!array) ? 0 : array->_capacity;
}

VoidPtr container_array_get(Array array) {
    return (!array) ? NULL : array->_pool;
}

VoidPtr container_array_at_index(Array array, const ByteSize index) {
    if (!array->_memory_size) return NULL;
    if (!array->_size) return NULL;
    if ((index < 0) || (index >= array->_size)) return NULL;

    ByteSize search_offset_ = array->_data_size * index;
    UIntPtr  search_addr_   = (UIntPtr)array->_pool + search_offset_;

    return (VoidPtr)(search_addr_);
}

Bool container_array_empty(Array array) {
    return (!array) ? true : (!array->_size);
}

Bool container_array_full(Array array) {
    return (!array) ? false : (array->_size == array->_capacity);
}
