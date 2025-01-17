#include "thread_posix.h"

#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

typedef struct Thread_Handle {
    pthread_t _handle;
    ByteSize  _size;
} ThreadHandle;

VT_INLINE ThreadHandle *_hal_thread_posix_get_handle(Thread *thread) { return VT_CAST(ThreadHandle *, thread->_handle); }

Thread *hal_thread_posix_construct(VoidPtr func, VoidPtr args) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    Thread *state_    = VT_CAST(Thread *, chunk_);
    state_->_state    = THREAD_STATE_CREATED;
    state_->_priority = 0;

    ThreadHandle *handle_ = VT_CAST(ThreadHandle *, state_ + 1);
    handle_->_size        = sizeof(pthread_t);
    if (pthread_create(&(handle_->_handle), NULL, func, args) != 0) {
        memory_manager_deallocate(chunk_, MEMORY_TAG_THREADING);
        return NULL;
    }

    state_->_handle = handle_;
    state_->_state  = THREAD_STATE_RUNNING;
    return state_;
}
Bool hal_thread_posix_join(Thread *thread) {
    if (!thread)
        return false;

    if (pthread_join(_hal_thread_posix_get_handle(thread)->_handle, NULL) != 0)
        return false;

    thread->_state = THREAD_STATE_TERMINATED;
    return true;
}
Bool hal_thread_posix_destroy(Thread *thread) { return (!thread || (thread->_state != THREAD_STATE_TERMINATED)) ? false : memory_manager_deallocate(thread, MEMORY_TAG_THREADING); }
