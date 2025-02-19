#pragma once

#include "vytal/defines/core/memory.h"
#include "vytal/defines/shared.h"

VYTAL_API MemoryZoneResult memory_zone_get(ConstStr zone_name, MemoryZone **out_zone);
VYTAL_API MemoryZoneResult memory_zone_clear(ConstStr zone_name);

VYTAL_API MemoryZoneResult memory_zone_allocate(ConstStr zone_name, const ByteSize size, VoidPtr *out_ptr);
VYTAL_API MemoryZoneResult memory_zone_deallocate(ConstStr zone_name, const VoidPtr ptr, const ByteSize size);

VYTAL_API void memory_zone_compute_size_classes(ByteSize *out_num_classes, MemoryZoneSizeClass **out_size_classes, const ByteSize capacity);
