#include "memory_zone.h"

#include <stdlib.h>
#include <string.h>

#include "vytal/core/memory/manager/memory_manager.h"

#define MEMORY_ZONE_SIZE_CLASSES_DEFAULT_CAPACITY 10

VYTAL_INLINE ByteSize _memory_zone_apply_alignment(const ByteSize size, const ByteSize alignment) {
    return ((size + (alignment - 1)) / alignment) * alignment;
}

void _memory_zone_compute_size_classes(ByteSize *out_num_classes, MemoryZoneSizeClass *out_size_classes, const ByteSize capacity) {
    if (!out_num_classes || !capacity) return;

    const Flt32 ratio_ = 1.618f;

    ByteSize current_size_ = 4;
    ByteSize index_        = 0;

    for (; current_size_ <= capacity; ++index_) {
        if (out_size_classes)
            out_size_classes[index_]._size = _memory_zone_apply_alignment(current_size_, MEMORY_ALIGNMENT_SIZE);

        current_size_ = (ByteSize)((Flt32)current_size_ * ratio_);
    }

    if (out_size_classes)
        out_size_classes[index_]._size = capacity;

    *out_num_classes = ++index_;
}

VYTAL_INLINE ByteSize _memory_zone_log2_size(ByteSize size) {
    ByteSize log2_ = 0;

#if __x86_64__
    if (size >= (1ull << 32)) {
        size >>= 32;
        log2_ += 32;
    }
#endif

    if (size >= (1 << 16)) {
        size >>= 16;
        log2_ += 16;
    }
    if (size >= (1 << 8)) {
        size >>= 8;
        log2_ += 8;
    }
    if (size >= (1 << 4)) {
        size >>= 4;
        log2_ += 4;
    }
    if (size >= (1 << 2)) {
        size >>= 2;
        log2_ += 2;
    }
    if (size >= (1 << 1)) {
        size >>= 1;
        log2_ += 1;
    }

    return log2_;
}

ByteSize _memory_zone_get_size_class_index(MemoryZone *zone, const ByteSize size) {
    const Flt32 log2_ratio_ = 0.694f;

    ByteSize aligned_size_ = _memory_zone_apply_alignment(size, MEMORY_ALIGNMENT_SIZE);
    ByteSize log2_size_    = _memory_zone_log2_size(aligned_size_);

    ByteSize index_ = (ByteSize)((Flt32)log2_size_ / log2_ratio_);
    while (index_ > 0 && aligned_size_ <= zone->_size_classes[index_]._size) --index_;

    return (index_ >= zone->_num_classes) ? zone->_num_classes - 1 : index_ + 1;
}

MemoryZoneResult memory_zone_get(ConstStr zone_name, MemoryZone **out_zone) {
    MemoryManager *manager_ = memory_manager_get();

    for (size_t i = 0; i < manager_->_zone_count; ++i) {
        MemoryZone *zone_ = &manager_->_zones[i];

        if (!strcmp(zone_->_name, zone_name)) {
            *out_zone = zone_;
            return MEMORY_ZONE_SUCCESS;
        }
    }

    return MEMORY_ZONE_ERROR_NOT_EXIST;
}

MemoryZoneResult memory_zone_clear(ConstStr zone_name) {
    MemoryZone      *zone_;
    MemoryZoneResult get_zone_ = memory_zone_get(zone_name, &zone_);
    if (get_zone_ != MEMORY_ZONE_SUCCESS) return get_zone_;

    for (size_t i = 0; i < zone_->_num_classes; ++i)
        zone_->_size_classes[i]._num_blocks = 0;

    memset((VoidPtr)zone_->_start_addr, 0, zone_->_capacity);
    zone_->_used_memory = 0;

    return MEMORY_ZONE_SUCCESS;
}

MemoryZoneResult memory_zone_allocate(ConstStr zone_name, const ByteSize size, VoidPtr *out_ptr) {
    if (!zone_name || !size || !out_ptr) return MEMORY_ZONE_ERROR_INVALID_PARAM;

    MemoryZone      *zone_;
    MemoryZoneResult get_zone_ = memory_zone_get(zone_name, &zone_);
    if (get_zone_ != MEMORY_ZONE_SUCCESS) return get_zone_;

    ByteSize             index_      = _memory_zone_get_size_class_index(zone_, size);
    MemoryZoneSizeClass *size_class_ = &zone_->_size_classes[index_];

    if (zone_->_used_memory + size_class_->_size > zone_->_capacity)
        return MEMORY_ZONE_ERROR_INSUFFICIENT_MEMORY;

    // if free block of fitting size is found
    if ((size_class_->_capacity > 0) && (size_class_->_num_blocks > 0)) {
        zone_->_used_memory += size_class_->_size;
        *out_ptr = size_class_->_blocks[--size_class_->_num_blocks];

        return MEMORY_ZONE_SUCCESS;
    }

    // otherwise, allocate from zone memory
    *out_ptr = (VoidPtr)((UIntPtr)zone_->_start_addr + zone_->_used_memory);
    zone_->_used_memory += size;

    return MEMORY_ZONE_SUCCESS;
}

MemoryZoneResult memory_zone_deallocate(ConstStr zone_name, const VoidPtr ptr, const ByteSize size) {
    if (!zone_name || !ptr || !size) return MEMORY_ZONE_ERROR_INVALID_PARAM;

    MemoryZone      *zone_;
    MemoryZoneResult get_zone_ = memory_zone_get(zone_name, &zone_);
    if (get_zone_ != MEMORY_ZONE_SUCCESS) return get_zone_;

    const float ratio_ = 1.618f;

    ByteSize             index_      = _memory_zone_get_size_class_index(zone_, size);
    MemoryZoneSizeClass *size_class_ = &zone_->_size_classes[index_];

    if (size_class_->_num_blocks == size_class_->_capacity) {
        ByteSize new_capacity_ = !size_class_->_capacity ? MEMORY_ZONE_SIZE_CLASSES_DEFAULT_CAPACITY : (ByteSize)((Flt32)size_class_->_capacity * ratio_);

        VoidPtr *new_blocks_ = realloc(size_class_->_blocks, sizeof(VoidPtr) * new_capacity_);
        if (!new_blocks_)
            return MEMORY_ZONE_ERROR_MEMORY_ALLOCATION;

        size_class_->_blocks   = new_blocks_;
        size_class_->_capacity = new_capacity_;
    }

    size_class_->_blocks[size_class_->_num_blocks++] = ptr;
    zone_->_used_memory -= size_class_->_size;

    return MEMORY_ZONE_SUCCESS;
}

void memory_zone_compute_size_classes(ByteSize *out_num_classes, MemoryZoneSizeClass *out_size_classes, const ByteSize capacity) {
    _memory_zone_compute_size_classes(out_num_classes, out_size_classes, capacity);
}
