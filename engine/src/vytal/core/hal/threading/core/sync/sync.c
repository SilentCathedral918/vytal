#include "sync.h"

#include "vytal/core/hal/threading/backend/posix/condvar_posix.h"
#include "vytal/core/hal/threading/backend/posix/mutex_posix.h"
#include "vytal/core/hal/threading/backend/posix/rwlock_posix.h"
#include "vytal/core/hal/threading/backend/win32/condvar_win32.h"
#include "vytal/core/hal/threading/backend/win32/mutex_win32.h"
#include "vytal/core/hal/threading/backend/win32/rwlock_win32.h"
#include "vytal/core/hal/threading/module/thread.h"

Mutex *hal_mutex_construct(void) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_mutex_posix_construct();

    case THREAD_BACKEND_WIN32:
        return hal_mutex_win32_construct();

    default:
        return NULL;
    }
}
Bool hal_mutex_destruct(Mutex *mutex) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_mutex_posix_destruct(mutex);

    case THREAD_BACKEND_WIN32:
        return hal_mutex_win32_destruct(mutex);

    default:
        return false;
    }
}
Bool hal_mutex_lock(Mutex *mutex) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_mutex_posix_lock(mutex);

    case THREAD_BACKEND_WIN32:
        return hal_mutex_win32_lock(mutex);

    default:
        return false;
    }
}
Bool hal_mutex_trylock(Mutex *mutex) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_mutex_posix_trylock(mutex);

    case THREAD_BACKEND_WIN32:
        return hal_mutex_win32_trylock(mutex);

    default:
        return false;
    }
}
Bool hal_mutex_unlock(Mutex *mutex) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_mutex_posix_unlock(mutex);

    case THREAD_BACKEND_WIN32:
        return hal_mutex_win32_unlock(mutex);

    default:
        return false;
    }
}
CondVar *hal_condvar_construct(void) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_construct();

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_construct();

    default:
        return NULL;
    }
}
Bool hal_condvar_destruct(CondVar *cv) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_destruct(cv);

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_destruct(cv);

    default:
        return false;
    }
}
Bool hal_condvar_wait(CondVar *cv, Mutex *mutex) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_wait(cv, mutex);

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_wait(cv, mutex);

    default:
        return false;
    }
}
Bool hal_condvar_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_wait_timed(cv, mutex, timeout_ms);

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_wait_timed(cv, mutex, timeout_ms);

    default:
        return false;
    }
}
Bool hal_condvar_signal(CondVar *cv) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_signal(cv);

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_signal(cv);

    default:
        return false;
    }
}
Bool hal_condvar_broadcast(CondVar *cv) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_condvar_posix_broadcast(cv);

    case THREAD_BACKEND_WIN32:
        return hal_condvar_win32_broadcast(cv);

    default:
        return false;
    }
}
RWLock *hal_rwlock_construct(void) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_rwlock_posix_construct();

    case THREAD_BACKEND_WIN32:
        return hal_rwlock_win32_construct();

    default:
        return NULL;
    }
}
Bool hal_rwlock_destruct(RWLock *rwlock) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_rwlock_posix_destruct(rwlock);

    case THREAD_BACKEND_WIN32:
        return hal_rwlock_win32_destruct(rwlock);

    default:
        return false;
    }
}
Bool hal_rwlock_readlock(RWLock *rwlock) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_rwlock_posix_readlock(rwlock);

    case THREAD_BACKEND_WIN32:
        return hal_rwlock_win32_readlock(rwlock);

    default:
        return false;
    }
}
Bool hal_rwlock_writelock(RWLock *rwlock) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_rwlock_posix_writelock(rwlock);

    case THREAD_BACKEND_WIN32:
        return hal_rwlock_win32_writelock(rwlock);

    default:
        return false;
    }
}
Bool hal_rwlock_unlock(RWLock *rwlock, const Bool shared) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_rwlock_posix_unlock(rwlock);

    case THREAD_BACKEND_WIN32:
        return shared ? hal_rwlock_win32_unlock_shared(rwlock) : hal_rwlock_win32_unlock_exclusive(rwlock);

    default:
        return false;
    }
}
