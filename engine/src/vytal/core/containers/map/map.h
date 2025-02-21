#pragma once

#include "vytal/defines/core/containers.h"
#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API ContainerResult container_map_construct(const ByteSize data_size, Map *out_new_map);
VYTAL_API ContainerResult container_map_destruct(Map map);

VYTAL_API ContainerResult container_map_insert(Map *map, ConstStr key, const VoidPtr data);
VYTAL_API ContainerResult container_map_remove(Map *map, ConstStr key);
VYTAL_API ContainerResult container_map_update(Map *map, ConstStr key, const VoidPtr new_data);
VYTAL_API ContainerResult container_map_search(Map map, ConstStr key, VoidPtr *out_data);

VYTAL_API Bool     container_map_contains(Map map, ConstStr key);
VYTAL_API Bool     container_map_empty(Map map);
VYTAL_API Bool     container_map_full(Map map);
VYTAL_API ByteSize container_map_size(Map map);
VYTAL_API ByteSize container_map_capacity(Map map);
VYTAL_API ByteSize container_map_data_size(Map map);
VYTAL_API VoidPtr  container_map_at_index(Map map, const ByteSize index);
