#pragma once

#include "vytal/defines/core/container.h"
#include "vytal/defines/core/mem.h"
#include "vytal/defines/shared.h"

VT_API Queue    container_queue_construct(void);
VT_API Queue    container_queue_construct_custom(const ArenaAllocator allocator, const ByteSize capacity);
VT_API Bool     container_queue_destruct(Queue queue);
VT_API Bool     container_queue_enqueue(Queue queue, VoidPtr data, const ByteSize data_size);
VT_API Bool     container_queue_dequeue(Queue queue, VoidPtr data);
VT_API Bool     container_queue_is_empty(Queue queue);
VT_API Bool     container_queue_is_full(Queue queue);
VT_API ByteSize container_queue_get_length(Queue queue);
VT_API ByteSize container_queue_get_capacity(Queue queue);
VT_API VoidPtr  container_queue_get_front(Queue queue);
VT_API VoidPtr  container_queue_get_back(Queue queue);
