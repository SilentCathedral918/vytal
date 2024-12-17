#pragma once

#include "vytal/defines/core/ctnr.h"
#include "vytal/defines/shared.h"

VT_API Map     container_map_construct(const ByteSize data_size);
VT_API Bool    container_map_destruct(Map map);
VT_API Bool    container_map_insert(Map map, const VoidPtr key, const VoidPtr data);
VT_API Bool    container_map_remove(Map map, const VoidPtr key);
VT_API VoidPtr container_map_search(Map map, const VoidPtr key);
VT_API VoidPtr container_map_search_by_index(Map map, const ByteSize index);
VT_API Bool    container_map_update(Map map, const VoidPtr key, const VoidPtr new_data);

VT_API Bool     container_map_contains(Map map, const VoidPtr key);
VT_API Bool     container_map_isempty(Map map);
VT_API Bool     container_map_isfull(Map map);
VT_API ByteSize container_map_length(Map map);
VT_API ByteSize container_map_capacity(Map map);
VT_API ByteSize container_map_data_size(Map map);

#define container_map_get(map, type, key) (VT_CAST(type *, container_map_search(map, VT_CAST(VoidPtr, key))))

#define container_map_get_by_index(map, type, index) (VT_CAST(type *, container_map_search_by_index(map, index)))

#define container_map_get_value(map, type, key) (container_map_get(map, type, key) ? *(container_map_get(map, type, key)) : VT_STRUCT(type, 0))

#define container_map_get_value_by_index(map, type, index) (container_map_get_by_index(map, type, index) ? *(container_map_get_by_index(map, type, index)) : VT_STRUCT(type, 0))