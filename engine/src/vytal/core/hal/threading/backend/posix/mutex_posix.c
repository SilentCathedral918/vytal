#include "mutex_posix.h"

#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

typedef struct Mutex_Handle {
    pthread_mutex_t _handle;
    ByteSize        _size;
} MutexHandle;

VT_INLINE MutexHandle *_hal_mutex_posix_get_handle(Mutex *mutex) { return VT_CAST(MutexHandle *, mutex->_handle); }

Mutex *hal_mutex_posix_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    Mutex       *state_  = VT_CAST(Mutex *, chunk_);
    MutexHandle *handle_ = VT_CAST(MutexHandle *, state_ + 1);
    handle_->_size       = sizeof(pthread_mutex_t);
    if (pthread_mutex_init(&(handle_->_handle), NULL) != 0) {
        memory_manager_deallocate(state_, MEMORY_TAG_THREADING);
        return NULL;
    }

    state_->_handle = handle_;
    return state_;
}
Bool hal_mutex_posix_destruct(Mutex *mutex) {
    if (!mutex)
        return false;

    if (pthread_mutex_destroy(&(_hal_mutex_posix_get_handle(mutex)->_handle)) != 0)
        return false;

    return memory_manager_deallocate(mutex, MEMORY_TAG_THREADING);
}
Bool hal_mutex_posix_lock(Mutex *mutex) { return !mutex ? false : (pthread_mutex_lock(&(_hal_mutex_posix_get_handle(mutex)->_handle)) == 0); }
Bool hal_mutex_posix_trylock(Mutex *mutex) { return !mutex ? false : (pthread_mutex_trylock(&(_hal_mutex_posix_get_handle(mutex)->_handle)) == 0); }
Bool hal_mutex_posix_unlock(Mutex *mutex) { return !mutex ? false : (pthread_mutex_unlock(&(_hal_mutex_posix_get_handle(mutex)->_handle)) == 0); }
