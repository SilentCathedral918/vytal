#include "mutex_win32.h"

#include "vytal/managers/memory/memmgr.h"

#include <windows.h>

typedef struct Mutex_Handle {
    LPCRITICAL_SECTION _handle;
} MutexHandle;

Mutex *hal_mutex_win32_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    Mutex       *state_  = VT_CAST(Mutex *, chunk_);
    MutexHandle *handle_ = VT_CAST(MutexHandle *, state_ + 1);
    InitializeCriticalSection(handle_->_handle);

    state_->_handle = handle_;
    return state_;
}
Bool hal_mutex_win32_destruct(Mutex *mutex) {
    if (!mutex)
        return false;

    DeleteCriticalSection(VT_CAST(MutexHandle *, mutex->_handle)->_handle);
    return memory_manager_deallocate(mutex, MEMORY_TAG_THREADING);
}
Bool hal_mutex_win32_lock(Mutex *mutex) {
    if (!mutex)
        return false;

    EnterCriticalSection(VT_CAST(MutexHandle *, mutex->_handle)->_handle);
    return true;
}
Bool hal_mutex_win32_trylock(Mutex *mutex) {
    if (!mutex)
        return false;

    TryEnterCriticalSection(VT_CAST(MutexHandle *, mutex->_handle)->_handle);
    return true;
}
Bool hal_mutex_win32_unlock(Mutex *mutex) {
    if (!mutex)
        return false;

    LeaveCriticalSection(VT_CAST(MutexHandle *, mutex->_handle)->_handle);
    return true;
}
