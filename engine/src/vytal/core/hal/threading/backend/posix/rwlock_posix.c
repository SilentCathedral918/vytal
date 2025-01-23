#include "rwlock_posix.h"

#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

RWLock *hal_rwlock_posix_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    RWLock           *state_  = VT_CAST(RWLock *, chunk_);
    pthread_rwlock_t *handle_ = VT_CAST(pthread_rwlock_t *, state_ + 1);
    if (pthread_rwlock_init(handle_, NULL) != 0) {
        memory_manager_deallocate(state_, MEMORY_TAG_THREADING);
        return NULL;
    }

    state_->_handle = handle_;
    return state_;
}
Bool hal_rwlock_posix_destruct(RWLock *rwlock) {
    if (!rwlock)
        return false;

    if (pthread_rwlock_destroy(rwlock->_handle) != 0)
        return false;

    return memory_manager_deallocate(rwlock, MEMORY_TAG_THREADING);
}
Bool hal_rwlock_posix_readlock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_rdlock(rwlock->_handle) == 0); }
Bool hal_rwlock_posix_writelock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_wrlock(rwlock->_handle) == 0); }
Bool hal_rwlock_posix_unlock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_unlock(rwlock->_handle) == 0); }
