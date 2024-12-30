#include "map.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/hash/hash.h"
#include "vytal/core/memory/allocators/pool.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/managers/memory/memmgr.h"

#include "vytal/core/misc/console/console.h"

#define CONTAINER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB
#define MAX_PROBE_LENGTH(capacity) ((capacity) / 2)

typedef struct Container_Map_Data_Item {
    UIntPtr   _pkey;
    HashedInt _hashed_key;
    UIntPtr   _pdata;
} MapDataItem;

typedef struct Container_Map_Data {
    VoidPtr       _bucket;
    ByteSize      _data_size;
    ByteSize      _length;
    ByteSize      _capacity;
    PoolAllocator _allocator;
} MapData;

static UInt32 vytal_container_map_probe_length = 0;

VT_INLINE MapData *_container_map_get_internal_data(Map map) {
    return (!map || !(map->_internal_data)) ? NULL : (map->_internal_data);
}
VT_INLINE Bool _container_map_empty(Map map) { return (_container_map_get_internal_data(map)->_length == 0); }
VT_INLINE Bool _container_map_full(Map map) {
    return (_container_map_get_internal_data(map)->_length == _container_map_get_internal_data(map)->_capacity);
}

Map container_map_construct(const ByteSize data_size, const PoolAllocator allocator) {
    if (data_size == 0)
        return NULL;

    ByteSize item_size_   = sizeof(MapDataItem) + data_size;
    ByteSize bucket_size_ = allocator ? (allocator_pool_capacity(allocator) - (sizeof(Container_Map) + sizeof(MapData)))
                                      : (CONTAINER_MAX_SIZE - (sizeof(Container_Map) + sizeof(MapData)));

    VoidPtr chunk_ = NULL;
    if (allocator)
        chunk_ = allocator_pool_allocate(allocator);
    else
        // size set to 0 since 'containers' use pool allocator, where size is already determined
        chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);

    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the map container self
    Map map_ = VT_CAST(Map, chunk_);

    // element 2: the map internal data struct
    MapData *data_    = VT_CAST(MapData *, map_ + 1);
    data_->_data_size = data_size;
    data_->_length    = 0;
    data_->_capacity  = bucket_size_ / item_size_;
    data_->_allocator = allocator;

    // element 3: the memory block
    data_->_bucket = VT_CAST(VoidPtr, data_ + 1);

    // assign internal data to map ownership
    map_->_internal_data = data_;

    return map_;
}

Bool container_map_destruct(Map map) {
    if (!map || !(map->_internal_data))
        return false;

    // free the entire map
    {
        PoolAllocator allocator_ = _container_map_get_internal_data(map)->_allocator;

        if (allocator_)
            allocator_pool_deallocate(allocator_, map);
        else
            memory_manager_deallocate(map, MEMORY_TAG_CONTAINERS);

        map = NULL;
    }

    return true;
}

Bool container_map_insert(Map map, const VoidPtr key, const VoidPtr data) {
    if (!map || !key || !data || _container_map_full(map))
        return false;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashstr(VT_CAST(ConstStr, key), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        if (check_slot_->_hashed_key == hashed_) {
            vytal_container_map_probe_length = 0;
            return false; // key already exists
        }

        // linear probe
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);
        }

        if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH(data_->_capacity))
            return false; // failed due to excessive probing
    }

    // found available slot...
    {
        check_slot_->_pkey       = VT_CAST(UIntPtr, key);
        check_slot_->_hashed_key = hashed_;
        check_slot_->_pdata      = VT_CAST(UIntPtr, VT_CAST(BytePtr, check_slot_) + sizeof(MapDataItem));

        hal_mem_memcpy(VT_CAST(VoidPtr, check_slot_->_pdata), data, data_->_data_size);
    }

    data_->_length++;
    return true;
}

Bool container_map_remove(Map map, const VoidPtr key) {
    if (!map || !key || _container_map_empty(map))
        return false;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashstr(VT_CAST(ConstStr, key), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        // if found the item of key
        if (check_slot_->_hashed_key == hashed_) {
            hal_mem_memzero(check_slot_, sizeof(MapDataItem));
            --data_->_length;
            vytal_container_map_probe_length = 0;
            return true;
        }

        if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH(data_->_capacity))
            return false; // failed due to excessive probing

        // otherwise, linear probe
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);
        }
    }

    return false;
}

Bool container_map_update(Map map, const VoidPtr key, const VoidPtr new_data) {
    if (!map || !key || _container_map_empty(map))
        return false;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashstr(VT_CAST(ConstStr, key), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        // if found the item of key
        if (check_slot_->_hashed_key == hashed_) {
            hal_mem_memcpy(VT_CAST(VoidPtr, check_slot_->_pdata), new_data, data_->_data_size);
            vytal_container_map_probe_length = 0;
            return true;
        }

        // otherwise, linear probe
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);
        }

        if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH(data_->_capacity))
            return false; // failed due to excessive probing
    }

    return false;
}

VoidPtr container_map_search(Map map, const VoidPtr key) {
    if (!map || !key || _container_map_empty(map))
        return NULL;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashstr(VT_CAST(ConstStr, key), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        // if found the item of key
        if (check_slot_->_hashed_key == hashed_) {
            vytal_container_map_probe_length = 0;
            return VT_CAST(VoidPtr, check_slot_->_pdata);
        }

        // otherwise, linear probe
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = (sizeof(MapDataItem) + data_->_data_size) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_bucket) + check_offset_);
        }

        if (vytal_container_map_probe_length++ > MAX_PROBE_LENGTH(data_->_capacity))
            return NULL; // failed due to excessive probing
    }

    return NULL;
}

Bool container_map_contains(Map map, const VoidPtr key) { return (!map ? false : (container_map_search(map, key) != NULL)); }
Bool container_map_isempty(Map map) { return (!map ? true : _container_map_empty(map)); }
Bool container_map_isfull(Map map) { return (!map ? false : _container_map_full(map)); }
ByteSize container_map_length(Map map) { return (!map ? 0 : _container_map_get_internal_data(map)->_length); }
ByteSize container_map_capacity(Map map) { return (!map ? 0 : _container_map_get_internal_data(map)->_capacity); }
ByteSize container_map_data_size(Map map) { return (!map ? 0 : _container_map_get_internal_data(map)->_data_size); }
