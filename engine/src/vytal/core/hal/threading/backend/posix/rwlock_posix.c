#include "rwlock_posix.h"

#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

typedef struct RWLock_Handle {
    pthread_rwlock_t _handle;
    ByteSize         _size;
} RWLockHandle;

VT_INLINE RWLockHandle *_hal_rwlock_posix_get_handle(RWLock *rwlock) { return VT_CAST(RWLockHandle *, rwlock->_handle); }

RWLock *hal_rwlock_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    RWLock       *state_  = VT_CAST(RWLock *, chunk_);
    RWLockHandle *handle_ = VT_CAST(RWLockHandle *, state_ + 1);
    if (pthread_rwlock_init(&(handle_->_handle), NULL) != 0) {
        memory_manager_deallocate(state_, MEMORY_TAG_THREADING);
        return NULL;
    }

    state_->_handle = handle_;
    return state_;
}
Bool hal_rwlock_destruct(RWLock *rwlock) {
    if (!rwlock)
        return false;

    if (pthread_rwlock_destroy(&(_hal_rwlock_posix_get_handle(rwlock)->_handle)) != 0)
        return false;

    return memory_manager_deallocate(rwlock, MEMORY_TAG_THREADING);
}
Bool hal_rwlock_readlock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_rdlock(&(_hal_rwlock_posix_get_handle(rwlock)->_handle)) == 0); }
Bool hal_rwlock_writelock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_wrlock(&(_hal_rwlock_posix_get_handle(rwlock)->_handle)) == 0); }
Bool hal_rwlock_unlock(RWLock *rwlock) { return !rwlock ? false : (pthread_rwlock_unlock(&(_hal_rwlock_posix_get_handle(rwlock)->_handle)) == 0); }
