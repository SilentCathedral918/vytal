#pragma once

#include "vytal/defines/core/container.h"
#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API Map  container_map_construct(const ByteSize data_size);
VT_API Map  container_map_construct_custom(const ByteSize data_size, const ArenaAllocator allocator, const ByteSize capacity);
VT_API Bool container_map_destruct(Map map);
VT_API Bool container_map_insert(Map map, ConstStr key, const VoidPtr data);
VT_API Bool container_map_remove(Map map, ConstStr key);
VT_API VoidPtr container_map_search(Map map, ConstStr key);
VT_API Bool    container_map_update(Map map, ConstStr key, const VoidPtr new_data);

VT_API Bool     container_map_contains(Map map, ConstStr key);
VT_API Bool     container_map_isempty(Map map);
VT_API Bool     container_map_isfull(Map map);
VT_API ByteSize container_map_length(Map map);
VT_API ByteSize container_map_capacity(Map map);
VT_API ByteSize container_map_data_size(Map map);

#define container_map_get(map, type, key) (VT_CAST(type *, container_map_search(map, key)))
#define container_map_get_value(map, type, key)                                                                                \
    ((container_map_get(map, type, key) != NULL) ? *(container_map_get(map, type, key)) : VT_STRUCT(type, 0))
