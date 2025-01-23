#include "queue.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/managers/memory/memmgr.h"

#define CONTAINER_MAX_SIZE VT_SIZE_KB_MULT(32) // 32 KB

typedef struct Container_Queue_Item_Data {
    VoidPtr  _data;
    ByteSize _data_size;
} QueueItemData;

typedef struct Container_Queue_Data {
    QueueItemData *_items;
    ByteSize       _length;
    ByteSize       _capacity;
    ByteSize       _front;
    ByteSize       _back;
    ArenaAllocator _allocator;
} QueueData;

VT_INLINE QueueData *_container_queue_get_internal_data(Queue queue) { return (!queue || !(queue->_internal_data)) ? NULL : (queue->_internal_data); }
VT_INLINE Bool       _container_queue_is_empty(Queue queue) { return (_container_queue_get_internal_data(queue)->_length == 0); }
VT_INLINE Bool       _container_queue_is_full(Queue queue) { return (_container_queue_get_internal_data(queue)->_length == _container_queue_get_internal_data(queue)->_capacity); }

Queue container_queue_construct(void) {
    ByteSize item_size_   = sizeof(QueueItemData);
    ByteSize bucket_size_ = CONTAINER_MAX_SIZE - (sizeof(Container_Queue) + sizeof(QueueData));

    VoidPtr chunk_ = NULL;
    chunk_         = memory_manager_allocate(0, MEMORY_TAG_CONTAINERS);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the queue container self
    Queue queue_ = VT_CAST(Queue, chunk_);

    // element 2: the queue internal data
    QueueData *data_  = VT_CAST(QueueData *, queue_ + 1);
    data_->_length    = 0;
    data_->_capacity  = bucket_size_ / item_size_;
    data_->_front     = 0;
    data_->_back      = 0;
    data_->_allocator = NULL;

    // element 3: the queue items
    data_->_items = VT_CAST(VoidPtr, data_ + 1);

    // assign internal data to queue ownership
    queue_->_internal_data = data_;

    return queue_;
}
Queue container_queue_construct_custom(const ArenaAllocator allocator, const ByteSize capacity) {
    if (!allocator || !capacity)
        return NULL;

    ByteSize item_size_   = sizeof(QueueItemData);
    ByteSize bucket_size_ = CONTAINER_MAX_SIZE - (sizeof(Container_Queue) + sizeof(QueueData));

    VoidPtr chunk_ = allocator_arena_allocate(allocator, capacity);
    if (!chunk_)
        return NULL;

    // there would be three elements...

    // element 1: the queue container self
    Queue queue_ = VT_CAST(Queue, chunk_);

    // element 2: the queue internal data
    QueueData *data_  = VT_CAST(QueueData *, queue_ + 1);
    data_->_length    = 0;
    data_->_capacity  = bucket_size_ / item_size_;
    data_->_front     = 0;
    data_->_back      = 0;
    data_->_allocator = NULL;

    // element 3: the queue items
    data_->_items = VT_CAST(VoidPtr, data_ + 1);

    // assign internal data to queue ownership
    queue_->_internal_data = data_;

    return queue_;
}
Bool container_queue_destruct(Queue queue) {
    if (!queue || !(queue->_internal_data))
        return false;

    // free the entire queue
    {
        ArenaAllocator allocator_ = _container_queue_get_internal_data(queue)->_allocator;
        if (!allocator_)
            memory_manager_deallocate(queue, MEMORY_TAG_CONTAINERS);

        queue = NULL;
    }

    return true;
}
Bool container_queue_enqueue(Queue queue, VoidPtr data, const ByteSize data_size) {
    if (!queue || !data || !data_size)
        return false;

    QueueData *data_ = _container_queue_get_internal_data(queue);
    if (!data_ || _container_queue_is_full(queue))
        return false;

    QueueItemData item_         = {._data = data, ._data_size = data_size};
    data_->_items[data_->_back] = item_;
    data_->_back                = (data_->_back + 1) % data_->_capacity;

    ++data_->_length;
    return true;
}
Bool container_queue_dequeue(Queue queue, VoidPtr data) {
    if (!queue || !data)
        return false;

    QueueData *data_ = _container_queue_get_internal_data(queue);
    if (!data_ || _container_queue_is_empty(queue))
        return false;

    QueueItemData front_item_ = data_->_items[data_->_front];
    memcpy(data, front_item_._data, front_item_._data_size);
    data_->_front = (data_->_front + 1) % data_->_capacity;

    --data_->_length;
    return true;
}
Bool     container_queue_is_empty(Queue queue) { return _container_queue_is_empty(queue); }
Bool     container_queue_is_full(Queue queue) { return _container_queue_is_full(queue); }
ByteSize container_queue_get_length(Queue queue) { return (!queue) ? 0 : (_container_queue_get_internal_data(queue)->_length); }
ByteSize container_queue_get_capacity(Queue queue) { return (!queue) ? 0 : (_container_queue_get_internal_data(queue)->_capacity); }
VoidPtr  container_queue_get_front(Queue queue) { return (!queue) ? 0 : (_container_queue_get_internal_data(queue)->_items[_container_queue_get_internal_data(queue)->_front]._data); }
VoidPtr  container_queue_get_back(Queue queue) { return (!queue) ? 0 : (_container_queue_get_internal_data(queue)->_items[_container_queue_get_internal_data(queue)->_back]._data); }
