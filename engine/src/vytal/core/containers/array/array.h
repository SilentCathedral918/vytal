#pragma once

#include "vytal/defines/core/containers.h"
#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API ContainerResult container_array_construct(const ByteSize data_size, Array *out_new_array);
VYTAL_API ContainerResult container_array_destruct(Array array);

VYTAL_API ContainerResult container_array_push(Array *array, VoidPtr new_data);
VYTAL_API ContainerResult container_array_pop(Array *array);
VYTAL_API ContainerResult container_array_insert(Array *array, const ByteSize index, VoidPtr new_data);
VYTAL_API ContainerResult container_array_remove(Array *array, const VoidPtr data, const Bool remove_all);
VYTAL_API ContainerResult container_array_remove_at_index(Array *array, const ByteSize index);
VYTAL_API ContainerResult container_array_clear(Array *array);
VYTAL_API ContainerResult container_array_sort(Array *array, Int32 (*compare)(const void *left, const void *right));

VYTAL_API ByteSize container_array_data_size(Array array);
VYTAL_API ByteSize container_array_size(Array array);
VYTAL_API ByteSize container_array_capacity(Array array);
VYTAL_API VoidPtr  container_array_get(Array array);
VYTAL_API VoidPtr  container_array_at_index(Array array, const ByteSize index);
VYTAL_API Bool     container_array_empty(Array array);
VYTAL_API Bool     container_array_full(Array array);
