#include "map.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/containers/string/string.h"
#include "vytal/core/hash/hash.h"
#include "vytal/core/memory/zone/memory_zone.h"

#define TOMBSTONE_HASHED_KEY ((HashedInt)(-1))
#define MAX_PROBE_LENGTH(capacity) ((capacity * 3) / 5)  // 60% of map capacity

typedef struct Container_Map_Data_Item {
    String    _key;
    HashedInt _hashed_key;
    UIntPtr   _pdata;
} MapDataItem;

struct Container_Map {
    VoidPtr  _pool;
    ByteSize _data_size;
    ByteSize _size;
    ByteSize _capacity;

    // used for allocations/deallocations
    ByteSize _memory_size;
};

ContainerResult _container_map_rehash_insert(Map *map, MapDataItem *old_slot) {
    if (!map) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    ByteSize     idx_          = old_slot->_hashed_key % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)(*map)->_pool + check_offset_);

    ByteSize probe_ = 1;
    while (check_slot_->_hashed_key != 0) {
        // perform double-hashing probe
        UInt64 hash2_ = old_slot->_hashed_key;
        idx_          = (idx_ + (probe_ * hash2_)) % (*map)->_capacity;

        check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
        check_slot_   = (MapDataItem *)((BytePtr)(*map)->_pool + check_offset_);

        ++probe_;
    }

    check_slot_->_hashed_key = old_slot->_hashed_key;

    if (container_string_construct(container_string_get(old_slot->_key), &check_slot_->_key) != CONTAINER_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    check_slot_->_pdata = (UIntPtr)((BytePtr)check_slot_ + sizeof(MapDataItem));
    memmove((VoidPtr)check_slot_->_pdata, (VoidPtr)old_slot->_pdata, (*map)->_data_size);

    return CONTAINER_SUCCESS;
}

ContainerResult _container_map_resize(Map *map, const ByteSize new_capacity) {
    if (!map) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    Map old_map_ = *map;
    Map new_map_ = NULL;

    ByteSize item_size_           = sizeof(MapDataItem) + old_map_->_data_size;
    ByteSize base_new_alloc_size_ = old_map_->_memory_size * CONTAINER_RESIZE_FACTOR;
    ByteSize new_alloc_size_      = 0;

    if (memory_zone_allocate("containers", base_new_alloc_size_, (VoidPtr *)&new_map_, &new_alloc_size_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;
    memset(new_map_, 0, new_alloc_size_);

    new_map_->_data_size   = old_map_->_data_size;
    new_map_->_size        = old_map_->_size;
    new_map_->_capacity    = new_capacity;
    new_map_->_memory_size = new_alloc_size_;
    new_map_->_pool        = (VoidPtr)((UIntPtr)new_map_ + sizeof(struct Container_Map));

    for (ByteSize i = 0; i < old_map_->_capacity; ++i) {
        ByteSize     old_slot_offset_ = i * item_size_;
        MapDataItem *old_slot_        = (MapDataItem *)((BytePtr)old_map_->_pool + old_slot_offset_);

        if (old_slot_->_hashed_key != 0 && old_slot_->_hashed_key != TOMBSTONE_HASHED_KEY) {
            _container_map_rehash_insert(&new_map_, old_slot_);

            if (container_string_destruct(old_slot_->_key) != CONTAINER_SUCCESS)
                return CONTAINER_ERROR_DEALLOCATION_FAILED;
        }
    }

    if (memory_zone_deallocate("containers", old_map_, old_map_->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    *map = new_map_;

    return CONTAINER_SUCCESS;
}

ContainerResult container_map_construct(const ByteSize data_size, Map *out_new_map) {
    if (!data_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize item_size_  = sizeof(MapDataItem) + data_size;
    ByteSize pool_size_  = item_size_ * CONTAINER_DEFAULT_CAPACITY;
    ByteSize alloc_size_ = VYTAL_APPLY_ALIGNMENT(sizeof(struct Container_Map) + pool_size_, MEMORY_ALIGNMENT_SIZE);

    if (memory_zone_allocate("containers", alloc_size_, (VoidPtr *)out_new_map, &alloc_size_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;
    memset(*out_new_map, 0, alloc_size_);

    (*out_new_map)->_data_size   = data_size;
    (*out_new_map)->_size        = 0;
    (*out_new_map)->_capacity    = CONTAINER_DEFAULT_CAPACITY;
    (*out_new_map)->_memory_size = alloc_size_;
    (*out_new_map)->_pool        = (VoidPtr)((UIntPtr)(*out_new_map) + sizeof(struct Container_Map));

    return CONTAINER_SUCCESS;
}

ContainerResult container_map_destruct(Map map) {
    if (!map) return CONTAINER_ERROR_INVALID_PARAM;
    if (!map->_pool || !map->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    for (ByteSize i = 0; i < map->_capacity; ++i) {
        MapDataItem *slot_ = (MapDataItem *)((BytePtr)map->_pool + i * (sizeof(MapDataItem) + map->_data_size));

        if ((slot_->_hashed_key != 0) && (slot_->_hashed_key != TOMBSTONE_HASHED_KEY)) {
            if (container_string_destruct(slot_->_key) != CONTAINER_SUCCESS)
                return CONTAINER_ERROR_DEALLOCATION_FAILED;
        }
    }

    if (memory_zone_deallocate("containers", map, map->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    memset(map, 0, map->_memory_size);
    return CONTAINER_SUCCESS;
}

ContainerResult container_map_insert(Map *map, ConstStr key, const VoidPtr data) {
    if (!map || !key || !data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    // handle container resizing (when map is 75% full)
    if ((*map)->_size >= ((*map)->_capacity * 3) / 4) {
        ByteSize new_capacity_ = (*map)->_capacity * CONTAINER_RESIZE_FACTOR;

        ContainerResult resize_result = _container_map_resize(map, new_capacity_);
        if (resize_result != CONTAINER_SUCCESS)
            return resize_result;
    }

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    ByteSize probe_        = 1;
    ByteSize probe_length_ = 0;

    while ((check_slot_->_hashed_key != 0) && (check_slot_->_hashed_key != TOMBSTONE_HASHED_KEY)) {
        // slot with matching hash already exists
        if (check_slot_->_hashed_key == hashed_)
            return CONTAINER_ERROR_MAP_KEY_ALREADY_EXISTS;

        // perform double-hashing probing
        UInt64 hash2_ = check_slot_->_hashed_key;
        idx_          = (idx_ + (probe_ * hash2_)) % (*map)->_capacity;

        check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
        check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

        if (++probe_length_ > MAX_PROBE_LENGTH((*map)->_capacity)) {
            // scam for any empty slot
            for (ByteSize i = 0; i < (*map)->_capacity; ++i) {
                check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * i;
                check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

                if (check_slot_->_hashed_key == 0 || check_slot_->_hashed_key == TOMBSTONE_HASHED_KEY) {
                    idx_ = i;
                    break;
                }
            }

            if (check_slot_->_hashed_key != 0 && check_slot_->_hashed_key != TOMBSTONE_HASHED_KEY)
                return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;
        }

        ++probe_;
    }

    // found available slot
    {
        Bool is_tombstone_ = (check_slot_->_hashed_key == TOMBSTONE_HASHED_KEY);

        if (container_string_construct(key, &check_slot_->_key) != CONTAINER_SUCCESS)
            return CONTAINER_ERROR_ALLOCATION_FAILED;

        check_slot_->_hashed_key = hashed_;
        check_slot_->_pdata      = (UIntPtr)((BytePtr)check_slot_ + sizeof(MapDataItem));

        memmove((VoidPtr)check_slot_->_pdata, data, (*map)->_data_size);

        if (!is_tombstone_) ++(*map)->_size;
    }

    return CONTAINER_SUCCESS;
}

ContainerResult container_map_remove(Map *map, ConstStr key) {
    if (!map || !key) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    ByteSize probe_        = 1;
    ByteSize probe_length_ = 0;
    while (check_slot_->_hashed_key != 0) {
        if (check_slot_->_hashed_key == TOMBSTONE_HASHED_KEY) {
            // skip tombstone
        } else {
            // found matching slot
            if (container_string_destruct(check_slot_->_key) != CONTAINER_SUCCESS)
                return CONTAINER_ERROR_DEALLOCATION_FAILED;

            check_slot_->_hashed_key = TOMBSTONE_HASHED_KEY;
            check_slot_->_pdata      = 0;
            --(*map)->_size;
            return CONTAINER_SUCCESS;
        }

        if (++probe_length_ > MAX_PROBE_LENGTH((*map)->_capacity))
            return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

        // continue probing
        {
            UInt64 hash2_ = check_slot_->_hashed_key;
            idx_          = (idx_ + (probe_ * hash2_)) % (*map)->_capacity;

            check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
            check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);
        }

        ++probe_;
    }

    return CONTAINER_ERROR_MAP_KEY_NOT_FOUND;
}

ContainerResult container_map_update(Map *map, ConstStr key, const VoidPtr new_data) {
    if (!map || !key) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    ByteSize probe_        = 1;
    ByteSize probe_length_ = 0;
    while (check_slot_->_hashed_key != 0) {
        if (check_slot_->_hashed_key == hashed_) {
            // found matching slot
            memmove((VoidPtr)check_slot_->_pdata, new_data, (*map)->_data_size);
            return CONTAINER_SUCCESS;
        }

        if (++probe_length_ > MAX_PROBE_LENGTH((*map)->_capacity))
            return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

        // continue probing
        {
            UInt64 hash2_ = check_slot_->_hashed_key;
            idx_          = (idx_ + (probe_ * hash2_)) % (*map)->_capacity;

            check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
            check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);
        }

        ++probe_;
    }

    return CONTAINER_ERROR_MAP_KEY_NOT_FOUND;
}

ContainerResult container_map_search(Map map, ConstStr key, VoidPtr *out_data) {
    if (!map || !key || !out_data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!map->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % map->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + map->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)(map->_pool) + check_offset_);

    ByteSize probe_        = 1;
    ByteSize probe_length_ = 0;
    while (check_slot_->_hashed_key != 0) {
        // found matching slot
        if ((check_slot_->_hashed_key != TOMBSTONE_HASHED_KEY) &&
            (check_slot_->_hashed_key == hashed_) &&
            (!strcmp(container_string_get(check_slot_->_key), key))) {
            *out_data = (VoidPtr)check_slot_->_pdata;
            return CONTAINER_SUCCESS;
        }

        // otherwise
        else {
            if (++probe_length_ > MAX_PROBE_LENGTH(map->_capacity))
                return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

            // continue probing
            {
                UInt64 hash2_ = check_slot_->_hashed_key;
                idx_          = (idx_ + (probe_ * hash2_)) % map->_capacity;

                check_offset_ = (sizeof(MapDataItem) + map->_data_size) * idx_;
                check_slot_   = (MapDataItem *)((BytePtr)(map->_pool) + check_offset_);
            }

            ++probe_;
        }
    }

    *out_data = NULL;
    return CONTAINER_SUCCESS;
}

Bool container_map_contains(Map map, ConstStr key) {
    if (!map || !key) return false;
    if (!map->_memory_size) return false;

    VoidPtr data_ = NULL;
    if (container_map_search(map, key, &data_) != CONTAINER_SUCCESS) return false;

    return (data_ != NULL);
}

Bool container_map_empty(Map map) {
    return (!map ? true : !map->_size);
}

Bool container_map_full(Map map) {
    return (!map ? false : (map->_size == map->_capacity));
}

ByteSize container_map_size(Map map) {
    return (!map ? 0 : map->_size);
}

ByteSize container_map_capacity(Map map) {
    return (!map ? 0 : map->_capacity);
}

ByteSize container_map_data_size(Map map) {
    return (!map ? 0 : map->_data_size);
}

VoidPtr container_map_at_index(Map map, const ByteSize index) {
    if (!map) return NULL;
    if (!map->_memory_size || !map->_pool) return NULL;
    if ((index < 0) || (index > map->_capacity)) return NULL;

    ByteSize     check_offset_ = (sizeof(MapDataItem) + map->_data_size) * index;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)(map->_pool) + check_offset_);

    return (VoidPtr)((BytePtr)check_slot_ + sizeof(MapDataItem));
}
