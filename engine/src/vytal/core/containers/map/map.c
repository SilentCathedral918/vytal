#include "map.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "vytal/core/hash/hash.h"
#include "vytal/core/memory/zone/memory_zone.h"

#define TOMBSTONE_HASHED_KEY ((HashedInt)(-1))
#define MAX_PROBE_LENGTH(capacity) ((capacity * 3) / 4)

typedef struct Container_Map_Data_Item {
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

// forward declare these two funcs since they call on each other
ContainerResult _container_map_insert(Map *map, HashedInt hashed_key, const VoidPtr data);
ContainerResult _container_map_resize(Map *map, const ByteSize new_capacity);

static UInt32 vytal_container_map_probe_length = 0;

ContainerResult _container_map_insert(Map *map, HashedInt hashed_key, const VoidPtr data) {
    if (!map || !data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    ByteSize     idx_          = hashed_key % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    // handle container resizing (when map pool is 75% full)
    if ((*map)->_size >= (*map)->_capacity * 4 / 5) {
        ByteSize new_capacity_ = (*map)->_capacity * CONTAINER_RESIZE_FACTOR;

        ContainerResult resize_ = _container_map_resize(map, new_capacity_);
        if (resize_ != CONTAINER_SUCCESS)
            return resize_;
    }

    while ((check_slot_->_hashed_key != 0) && (check_slot_->_hashed_key != TOMBSTONE_HASHED_KEY)) {
        if (check_slot_->_hashed_key == hashed_key) {
            vytal_container_map_probe_length = 0;
            return CONTAINER_ERROR_MAP_KEY_ALREADY_EXISTS;
        }

        // linear probe
        {
            idx_          = (idx_ + 1) % (*map)->_capacity;
            check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
            check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);
        }

        if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH((*map)->_capacity))
            return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing
    }

    // found available slot
    {
        Bool is_tombstone_ = (check_slot_->_hashed_key == TOMBSTONE_HASHED_KEY);

        check_slot_->_hashed_key = hashed_key;
        check_slot_->_pdata      = (UIntPtr)((BytePtr)check_slot_ + sizeof(MapDataItem));
        memcpy((VoidPtr)check_slot_->_pdata, data, (*map)->_data_size);

        if (is_tombstone_)
            memset(check_slot_, 0, sizeof(MapDataItem));
        else
            ++(*map)->_size;
    }

    return CONTAINER_SUCCESS;
}

ContainerResult _container_map_resize(Map *map, const ByteSize new_capacity) {
    Map old_map_ = *map;
    Map new_map_ = NULL;

    ByteSize item_size_      = sizeof(MapDataItem) + old_map_->_data_size;
    ByteSize pool_size_      = item_size_ * CONTAINER_DEFAULT_CAPACITY;
    ByteSize new_alloc_size_ = VYTAL_APPLY_ALIGNMENT(sizeof(struct Container_Map) + pool_size_, MEMORY_ALIGNMENT_SIZE);

    if (memory_zone_allocate("Containers", new_alloc_size_, (VoidPtr *)&new_map_) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    new_map_->_data_size   = old_map_->_data_size;
    new_map_->_size        = old_map_->_size;
    new_map_->_capacity    = new_capacity;
    new_map_->_memory_size = new_alloc_size_;
    new_map_->_pool        = (VoidPtr)((UIntPtr)new_map_ + sizeof(struct Container_Map));

    for (ByteSize i = 0; i < old_map_->_capacity; i++) {
        MapDataItem *old_slot_ = (MapDataItem *)((BytePtr)old_map_->_pool + i * (sizeof(MapDataItem) + old_map_->_data_size));

        if (old_slot_->_hashed_key != 0 && old_slot_->_hashed_key != TOMBSTONE_HASHED_KEY)
            _container_map_insert(&new_map_, old_slot_->_hashed_key, (VoidPtr)old_slot_->_pdata);
    }

    if (memory_zone_deallocate("Containers", old_map_, old_map_->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    *map = new_map_;
    return CONTAINER_SUCCESS;
}

ContainerResult container_map_construct(const ByteSize data_size, Map *out_new_map) {
    if (!data_size) return CONTAINER_ERROR_INVALID_PARAM;

    ByteSize item_size_  = sizeof(MapDataItem) + data_size;
    ByteSize pool_size_  = item_size_ * CONTAINER_DEFAULT_CAPACITY;
    ByteSize alloc_size_ = VYTAL_APPLY_ALIGNMENT(sizeof(struct Container_Map) + pool_size_, MEMORY_ALIGNMENT_SIZE);

    if (memory_zone_allocate("Containers", alloc_size_, (VoidPtr *)out_new_map) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_ALLOCATION_FAILED;

    (*out_new_map)->_data_size   = data_size;
    (*out_new_map)->_size        = 0;
    (*out_new_map)->_capacity    = CONTAINER_DEFAULT_CAPACITY;
    (*out_new_map)->_memory_size = alloc_size_;
    (*out_new_map)->_pool        = (VoidPtr)((UIntPtr)(*out_new_map) + sizeof(struct Container_Map));
    memset((*out_new_map)->_pool, 0, pool_size_);

    return CONTAINER_SUCCESS;
}

ContainerResult container_map_destruct(Map map) {
    if (!map) return CONTAINER_ERROR_INVALID_PARAM;
    if (!map->_pool || !map->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    if (memory_zone_deallocate("Containers", map, map->_memory_size) != MEMORY_ZONE_SUCCESS)
        return CONTAINER_ERROR_DEALLOCATION_FAILED;

    memset(map, 0, sizeof(struct Container_Map));
    return CONTAINER_SUCCESS;
}

ContainerResult container_map_insert(Map *map, ConstStr key, const VoidPtr data) {
    if (!map || !key || !data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt hashed_ = hash_str(key, HASH_MODE_XX64);
    return _container_map_insert(map, hashed_, data);
}

ContainerResult container_map_remove(Map *map, ConstStr key) {
    if (!map || !key) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    while (check_slot_->_hashed_key != 0) {
        // found matching slot
        if (check_slot_->_hashed_key == hashed_) {
            check_slot_->_hashed_key = TOMBSTONE_HASHED_KEY;
            --(*map)->_size;

            vytal_container_map_probe_length = 0;
            return CONTAINER_SUCCESS;
        }

        // otherwise
        else {
            if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH((*map)->_capacity))
                return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

            // continue probing
            {
                idx_          = (idx_ + 1) % (*map)->_capacity;
                check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
                check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);
            }
        }
    }

    return CONTAINER_ERROR_MAP_KEY_NOT_FOUND;
}

ContainerResult container_map_update(Map *map, ConstStr key, const VoidPtr new_data) {
    if (!map || !key || !new_data) return CONTAINER_ERROR_INVALID_PARAM;
    if (!(*map)->_memory_size) return CONTAINER_ERROR_NOT_ALLOCATED;

    HashedInt    hashed_       = hash_str(key, HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % (*map)->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
    MapDataItem *check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);

    while (check_slot_->_hashed_key != 0) {
        // found matching slot
        if (check_slot_->_hashed_key == hashed_) {
            memcpy((VoidPtr)(check_slot_->_pdata), new_data, (*map)->_data_size);

            vytal_container_map_probe_length = 0;
            return CONTAINER_SUCCESS;
        }

        // otherwise
        else {
            if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH((*map)->_capacity))
                return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

            // continue probing
            {
                idx_          = (idx_ + 1) % (*map)->_capacity;
                check_offset_ = (sizeof(MapDataItem) + (*map)->_data_size) * idx_;
                check_slot_   = (MapDataItem *)((BytePtr)((*map)->_pool) + check_offset_);
            }
        }
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

    while (check_slot_->_hashed_key != 0) {
        // found matching slot
        if (check_slot_->_hashed_key == hashed_) {
            *out_data = (VoidPtr)check_slot_->_pdata;

            vytal_container_map_probe_length = 0;
            return CONTAINER_SUCCESS;
        }

        // otherwise
        else {
            if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH(map->_capacity))
                return CONTAINER_ERROR_MAP_REACHED_PROBING_LIMITS;  // failed due to excessive probing

            // continue probing
            {
                idx_          = (idx_ + 1) % map->_capacity;
                check_offset_ = (sizeof(MapDataItem) + map->_data_size) * idx_;
                check_slot_   = (MapDataItem *)((BytePtr)(map->_pool) + check_offset_);
            }
        }
    }

    return CONTAINER_ERROR_MAP_KEY_NOT_FOUND;
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
