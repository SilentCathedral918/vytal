#include "map.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/hash/hash.h"
#include "vytal/managers/memory/memmgr.h"

typedef struct Container_Map_Data_Item {
    UIntPtr   _pkey;
    HashedInt _hashed_key;
    UIntPtr   _pdata;
    ByteSize  _data_size;
} MapDataItem;

typedef struct Container_Map_Data {
    VoidPtr  _memory_block;
    ByteSize _length;
    ByteSize _capacity;
} MapData;

VT_INLINE MapData *_container_map_get_internal_data(Map map) {
    return (!map || !(map->_internal_data)) ? NULL : (map->_internal_data);
}
VT_API Bool _container_map_full(Map map) {
    return (_container_map_get_internal_data(map)->_length == _container_map_get_internal_data(map)->_capacity);
}
VT_API Bool _container_map_empty(Map map) { return (_container_map_get_internal_data(map)->_length == 0); }

Map container_map_construct(void) {
    // size set to 0 since 'containers' use pool allocator, where size is already determined
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the map container self
    Map map_ = VT_CAST(Map, chunk_);

    // element 2: the map internal data struct
    MapData *data_   = VT_CAST(MapData *, map_ + 1);
    data_->_length   = 0;
    data_->_capacity = VT_CONTAINER_DEFAULT_CAPACITY;

    // element 3: the memory block
    data_->_memory_block = VT_CAST(VoidPtr, data_ + 1);

    // assign internal data to string ownership
    map_->_internal_data = data_;

    return map_;
}

Bool container_map_destruct(Map map) {
    if (!map || !(map->_internal_data))
        return false;

    // free the entire string chunk
    {
        memory_manager_deallocate(map, MEMORY_TAG_CONTAINERS);
        map = NULL;
    }

    return true;
}

Bool container_map_insert(Map map, const VoidPtr key, const VoidPtr data, const ByteSize data_size) {
    if (!map || !map->_internal_data || !data || (data_size == 0) || _container_map_full(map))
        return false;

    MapData  *data_          = _container_map_get_internal_data(map);
    HashedInt hashed_        = hash_hashbuffer(key, sizeof(UIntPtr), HASH_MODE_XX64);
    ByteSize  idx_           = hashed_ % data_->_capacity;
    ByteSize  insert_offset_ = sizeof(MapDataItem) * idx_;

    // probe to find an available slot
    MapDataItem *item_ = VT_CAST(MapDataItem *, data_->_memory_block + insert_offset_);

    // linear probing
    // skip if key is found
    // otherwise (if the slot is occupied), probe next
    while ((item_->_pkey != 0x00) && (item_->_hashed_key != hashed_)) {
        idx_  = (idx_ + 1) % data_->_capacity;
        item_ = VT_CAST(MapDataItem *, data_->_memory_block + insert_offset_);
    }

    // insert the key and its data
    {
        item_->_pkey       = VT_CAST(UIntPtr, key);
        item_->_hashed_key = hashed_;
        item_->_pdata      = VT_CAST(UIntPtr, data);
        item_->_data_size  = data_size;
    }

    data_->_length++;
    return true;
}

Bool container_map_remove(Map map, const VoidPtr key) {
    if (!map || !map->_internal_data || !key)
        return false;

    if (_container_map_empty(map))
        return false;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashbuffer(key, sizeof(UIntPtr), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = sizeof(MapDataItem) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        if (check_slot_->_hashed_key == hashed_) {
            // perform removal...
            {
                // set item data to zero
                hal_mem_memzero(check_slot_, sizeof(MapDataItem));
            }

            data_->_length--;
            return true;
        }

        // linear probe in case the hashed value isn't matched
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = sizeof(MapDataItem) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);
        }
    }

    // item of specified key not found
    return false;
}

VoidPtr container_map_search(Map map, const VoidPtr key) {
    if (!map || !map->_internal_data || !key || _container_map_empty(map))
        return NULL;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashbuffer(key, sizeof(UIntPtr), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = sizeof(MapDataItem) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        if (check_slot_->_hashed_key == hashed_)
            return VT_CAST(VoidPtr, check_slot_->_pdata);

        // linear probe in case the hashed value isn't matched
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = sizeof(MapDataItem) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);
        }
    }

    return NULL;
}

Bool container_map_update(Map map, const VoidPtr key, const VoidPtr new_data, const ByteSize new_data_size) {
    if (!map || !map->_internal_data || _container_map_empty(map) || !key || !new_data || (new_data_size == 0))
        return false;

    MapData     *data_         = _container_map_get_internal_data(map);
    HashedInt    hashed_       = hash_hashbuffer(key, sizeof(UIntPtr), HASH_MODE_XX64);
    ByteSize     idx_          = hashed_ % data_->_capacity;
    ByteSize     check_offset_ = sizeof(MapDataItem) * idx_;
    MapDataItem *check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);

    while (check_slot_->_pkey != 0x00) {
        if (check_slot_->_hashed_key == hashed_) {
            check_slot_->_pdata     = VT_CAST(UIntPtr, new_data);
            check_slot_->_data_size = new_data_size;
            return true;
        }

        // linear probe in case the hashed value isn't matched
        {
            idx_          = (idx_ + 1) % data_->_capacity;
            check_offset_ = sizeof(MapDataItem) * idx_;
            check_slot_   = VT_CAST(MapDataItem *, VT_CAST(BytePtr, data_->_memory_block) + check_offset_);
        }
    }

    return false;
}

Bool container_map_clear(Map map) {
    if (!map || !map->_internal_data || _container_map_empty(map))
        return false;

    MapData *data_       = _container_map_get_internal_data(map);
    ByteSize clear_size_ = sizeof(MapDataItem) * data_->_capacity;

    hal_mem_memzero(data_->_memory_block, clear_size_);
    data_->_length = 0;
    return true;
}

Bool     container_map_contains(Map map, const VoidPtr key) { return (container_map_search(map, key) != NULL); }
Bool     container_map_isempty(Map map) { return _container_map_empty(map); }
Bool     container_map_isfull(Map map) { return _container_map_full(map); }
ByteSize container_map_length(Map map) { return (!map ? 0 : _container_map_get_internal_data(map)->_length); }
ByteSize container_map_capacity(Map map) { return (!map ? 0 : _container_map_get_internal_data(map)->_capacity); }
