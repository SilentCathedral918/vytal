#include "condvar_posix.h"

#include "vytal/core/hal/clock/hiresclock.h"
#include "vytal/managers/memory/memmgr.h"

#include <pthread.h>

typedef struct CondVar_Handle {
    pthread_cond_t _handle;
} CondVarHandle;

typedef struct Mutex_Handle {
    pthread_mutex_t _handle;
} MutexHandle;

VT_INLINE CondVarHandle *_hal_condvar_posix_get_handle(CondVar *cv) { return VT_CAST(CondVarHandle *, cv->_handle); }

CondVar *hal_condvar_posix_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    CondVar       *state_  = VT_CAST(CondVar *, chunk_);
    CondVarHandle *handle_ = VT_CAST(CondVarHandle *, state_ + 1);
    if (pthread_cond_init(&(handle_->_handle), NULL) != 0) {
        memory_manager_deallocate(state_, MEMORY_TAG_THREADING);
        return NULL;
    }

    state_->_handle = handle_;
    return state_;
}
Bool hal_condvar_posix_destruct(CondVar *cv) {
    if (!cv)
        return false;

    if (pthread_cond_destroy(&(_hal_condvar_posix_get_handle(cv)->_handle)) != 0)
        return false;

    return memory_manager_deallocate(cv, MEMORY_TAG_THREADING);
}
Bool hal_condvar_posix_wait(CondVar *cv, Mutex *mutex) { return (!cv || !mutex) ? false : (pthread_cond_wait(&(_hal_condvar_posix_get_handle(cv)->_handle), &(VT_CAST(MutexHandle *, mutex->_handle)->_handle)) == 0); }
Bool hal_condvar_posix_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms) {
    if (!cv || !mutex)
        return false;

    HiResClock    clock_;
    HiResTimeSpec spec_;
    hal_hiresclock_init(&clock_);
    Flt64 elapsed_ns_ = hal_hiresclock_getelapsed_nanosec(&clock_);
    spec_.tv_sec      = elapsed_ns_ / 1000000000.0;
    spec_.tv_nsec     = VT_CAST(UInt64, elapsed_ns_) % 1000000000;

    // add timeout to current time
    spec_.tv_sec += timeout_ms / 1000;
    spec_.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (spec_.tv_nsec >= 1000000000) {
        ++spec_.tv_sec;
        spec_.tv_nsec -= 1000000000;
    }

    return (pthread_cond_timedwait(&(_hal_condvar_posix_get_handle(cv)->_handle), &(VT_CAST(MutexHandle *, mutex->_handle)->_handle), &spec_) == 0);
}
Bool hal_condvar_posix_signal(CondVar *cv) { return !cv ? false : (pthread_cond_signal(&(_hal_condvar_posix_get_handle(cv)->_handle)) == 0); }
Bool hal_condvar_posix_broadcast(CondVar *cv) { return !cv ? false : (pthread_cond_broadcast(&(_hal_condvar_posix_get_handle(cv)->_handle)) == 0); }
