#include "thread_posix.h"

#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

Thread *hal_thread_posix_construct(VoidPtr func, VoidPtr args) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    Thread *state_    = VT_CAST(Thread *, chunk_);
    state_->_state    = THREAD_STATE_CREATED;
    state_->_priority = 0;

    pthread_t *handle_ = VT_CAST(pthread_t *, state_ + 1);
    if (pthread_create(handle_, NULL, func, args) != 0) {
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

    if (pthread_join(*VT_CAST(pthread_t *, thread->_handle), NULL) != 0)
        return false;

    thread->_state = THREAD_STATE_TERMINATED;
    return true;
}
Bool hal_thread_posix_destruct(Thread *thread) { return (!thread || (thread->_state != THREAD_STATE_TERMINATED)) ? false : (pthread_detach(*VT_CAST(pthread_t *, thread->_handle)) == 0) && memory_manager_deallocate(thread, MEMORY_TAG_THREADING); }
